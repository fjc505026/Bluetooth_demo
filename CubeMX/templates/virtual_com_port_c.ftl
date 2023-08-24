[#ftl]
/**
 * @brief  Initialize User process.
 *
 * @param  None
 * @retval None
 */
static void User_Init(void)
{
  BSP_COM_Init(COM1);
}

/**
 * @brief  Manage the communication with the BlueNRG2 via UART and SPI.
 *
 * @param  None
 * @retval None
 */
static void User_Process(void)
{
  HAL_StatusTypeDef status;
  uint8_t len;
  int i;
  
  /* Read uart header */
  status = HAL_UART_Receive(&hcom_uart[COM1], (uint8_t *)uart_header, UARTHEADERSIZE, BLE_UART_SHORT_TIMEOUT);
  if (status != HAL_OK && status != HAL_TIMEOUT)
  {
    Error_Handler();
  }

  if (status == HAL_OK) {
    //We actually received data from the GUI
    PRINT_DBG("From GUI ");

    len = uart_header[UARTHEADERSIZE-1];
      
    if (len > 0) {
      /*## Put UART peripheral in reception process ###########################*/  
      /* Any data received will be stored "aRxBuffer" buffer */    
      if(HAL_UART_Receive(&hcom_uart[COM1], (uint8_t *)aRxBuffer, len, BLE_UART_LONG_TIMEOUT) != HAL_OK)
      {
        Error_Handler();
      }
    }
      
#if PRINT_CSV_FORMAT
    print_csv_time();
    for (i = 0; i < UARTHEADERSIZE; i++) {
      PRINT_CSV(" %02x", uart_header[i]);
    }
    if (len > 0) {
      for (i = 0; i < len; i++) {
        PRINT_CSV(" %02x", aRxBuffer[i]);
      }
    }
    PRINT_CSV("\n");
#endif
      
    /* write data received from the vcom to the BlueNRG chip via SPI */
    if (uart_header[HCI_TYPE_OFFSET] == HCI_COMMAND_PKT) {
      //This is an HCI command so pass it to BlueNRG via SPI
      Hal_Write_Serial(uart_header, aRxBuffer, UARTHEADERSIZE, len);       
    } else {
      //Process the command locally without bothering with the BlueNRG chip
      handle_vendor_command(uart_header, UARTHEADERSIZE);
    }
  }

  while (HAL_GPIO_ReadPin(HCI_TL_SPI_EXTI_PORT, HCI_TL_SPI_EXTI_PIN) == GPIO_PIN_SET) {
    uint8_t rx_buffer[255];
    uint8_t rx_bytes;
      
    /* Data are available from BlueNRG: read them through SPI */    
    rx_bytes = HCI_TL_SPI_Receive(rx_buffer, sizeof(rx_buffer));
    
    /* Check if there is data is so, send it to VCOM */
    if (rx_bytes > 0) {
        
      PRINT_DBG("From BlueNRG to GUI ");
	  
#if PRINT_CSV_FORMAT
      print_csv_time();
      for (i = 0; i < rx_bytes; i++) {
        PRINT_CSV(" %02x", rx_buffer[i]);
      }
      PRINT_CSV("\n");
#endif
              
      for (i = 0; i < rx_bytes; i++) {
        if(HAL_UART_Transmit(&hcom_uart[COM1], (uint8_t*)&rx_buffer[i], 1, BLE_UART_LONG_TIMEOUT)!= HAL_OK) {
          Error_Handler();
        }         
      }
    }
  }
}

/** 
 * @brief  Handle vendor command
 * @param  cmd: the command
 * @param  cmd_len: the length of the command
 * @return None
 */
void handle_vendor_command(uint8_t* cmd, uint8_t cmd_len)
{
  int unsupported = 0;
  uint8_t len = 0;
  uint8_t response[MAX_RESP_SIZE];
  
  response[0] = HCI_VENDOR_PKT;
  response[RESP_VENDOR_CODE_OFFSET] = RESPONSE;
  response[RESP_CMDCODE_OFFSET] = cmd[HCI_VENDOR_CMDCODE_OFFSET];
  response[RESP_STATUS_OFFSET] = 0;  
  
  if (cmd[HCI_TYPE_OFFSET] == HCI_VENDOR_PKT) {
    switch (cmd[HCI_VENDOR_CMDCODE_OFFSET]) {
    case VERSION:
      response[RESP_PARAM_OFFSET] = FW_VERSION_MAJOR;
      response[RESP_PARAM_OFFSET+1] = FW_VERSION_MINOR;
      len = 2;
      break;

    case BLUENRG_RESET:   
      HCI_TL_SPI_Reset();
      break;

    case HW_BOOTLOADER:
      BlueNRG_HW_Bootloader();
      break;

    default:
      unsupported = 1;
    }
  } else {
    unsupported = 1;
  }
  
  if (unsupported) {
    response[RESP_STATUS_OFFSET] = UNKNOWN_COMMAND;
  }
  
  len += 2; //Status and Command code
  PACK_2_BYTE_PARAMETER(response + RESP_LEN_OFFSET_LSB, len);
  len += RESP_CMDCODE_OFFSET;
 
  PRINT_DBG("From Nucleo to GUI ");
#if PRINT_CSV_FORMAT
  print_csv_time();
  for (int i = 0; i < len; i++) {
    PRINT_CSV(" %02x", response[i]);
  }
  PRINT_CSV("\n");
#endif 
  
  if(HAL_UART_Transmit(&hcom_uart[COM1], (uint8_t*)response, len, BLE_UART_LONG_TIMEOUT) != HAL_OK) {
    Error_Handler();
  }
}

/**
 * @brief  Writes data to a serial interface.
 * @param  data1   :  1st buffer
 * @param  data2   :  2nd buffer
 * @param  n_bytes1: number of bytes in 1st buffer
 * @param  n_bytes2: number of bytes in 2nd buffer
 * @retval None
 */
void Hal_Write_Serial(const void* data1, const void* data2, int32_t n_bytes1,
                      int32_t n_bytes2)
{
  uint8_t buff[UARTHEADERSIZE + RXBUFFERSIZE];
  uint32_t tickstart;

  BLUENRG_memcpy(buff, (uint8_t *)data1, n_bytes1);
  BLUENRG_memcpy(buff + n_bytes1, (uint8_t *)data2, n_bytes2);
  
  tickstart = HAL_GetTick();
  while(1){
    /* Data are available for the BlueNRG: write them through SPI */    
    if (HCI_TL_SPI_Send(buff, n_bytes1+n_bytes2) == 0) break;
    if ((HAL_GetTick() - tickstart) > (HCI_DEFAULT_TIMEOUT_MS/10)) break;
  }
}

/**
 * @brief  Activate internal bootloader using pin.
 * @param  None
 * @retval None
 */
void BlueNRG_HW_Bootloader(void)
{
  Disable_SPI_IRQ();
  set_irq_as_output();
  
  HCI_TL_SPI_Reset();
  
  set_irq_as_input();
  Enable_SPI_IRQ();
}

/**
 * @brief  Enable SPI IRQ.
 * @param  None
 * @retval None
 */
void Enable_SPI_IRQ(void)
{
  HAL_NVIC_EnableIRQ(HCI_TL_SPI_EXTI_IRQn);  
}

/**
 * @brief  Disable SPI IRQ.
 * @param  None
 * @retval None
 */
void Disable_SPI_IRQ(void)
{ 
  HAL_NVIC_DisableIRQ(HCI_TL_SPI_EXTI_IRQn);
}

/**
 * @brief  Set in Output mode the IRQ.
 * @param  None
 * @retval None
 */
void set_irq_as_output(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  /* Pull IRQ high */
  GPIO_InitStructure.Pin   = HCI_TL_SPI_IRQ_PIN;
  GPIO_InitStructure.Mode  = GPIO_MODE_OUTPUT_PP;
[#if FamilyName?contains("F0") || FamilyName?contains("F1") || FamilyName?contains("F3")]
  GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
[#else]
  GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
[/#if] 
  GPIO_InitStructure.Pull  = GPIO_NOPULL;
  HAL_GPIO_Init(HCI_TL_SPI_IRQ_PORT, &GPIO_InitStructure);
  HAL_GPIO_WritePin(HCI_TL_SPI_IRQ_PORT, HCI_TL_SPI_IRQ_PIN, GPIO_PIN_SET);
}

/**
 * @brief  Set the IRQ in input mode.
 * @param  None
 * @retval None
 */
void set_irq_as_input(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  /* IRQ input */  
  GPIO_InitStructure.Pin       = HCI_TL_SPI_IRQ_PIN;
  GPIO_InitStructure.Mode      = GPIO_MODE_IT_RISING;
  GPIO_InitStructure.Pull      = GPIO_PULLDOWN;
[#if FamilyName?contains("F0") || FamilyName?contains("F1") || FamilyName?contains("F3")]
  GPIO_InitStructure.Speed     = GPIO_SPEED_FREQ_HIGH;
[#else]
  GPIO_InitStructure.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
[/#if]  
#ifdef IS_GPIO_AF
  GPIO_InitStructure.Alternate = 0; 
#endif
  HAL_GPIO_Init(HCI_TL_SPI_IRQ_PORT, &GPIO_InitStructure);
  
  GPIO_InitStructure.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(HCI_TL_SPI_IRQ_PORT, &GPIO_InitStructure);
}

