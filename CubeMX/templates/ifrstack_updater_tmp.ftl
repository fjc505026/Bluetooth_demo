[#ftl]
[#assign knownUSARTInstance = false]

[#assign IpInstance = ""]
[#assign IpName = ""]
[#assign USARTInstance = ""]

[#if BspIpDatas??]
  [#list BspIpDatas as SWIP]
    [#if SWIP.bsp??]
      [#list SWIP.bsp as bsp]
		[#if bsp.bspName?contains("USART")]		
			[#assign USARTInstance = bsp.bspInstanceName]
			[#assign knownUSARTInstance = true]
		[/#if]
      [/#list]
    [/#if]
  [/#list]
[/#if]
/**
 * @brief  Initialize User process
 *
 * @param  None
 * @retval None
 */
static void User_Init(void)
{
  BSP_COM_Init(COM1);
  
  BSP_LED_Init(LED2);
}

/**
 * @brief  User Process
 *
 * @param  None
 * @retval None
 */
static void User_Process(void)
{
  BSP_LED_Toggle(LED2);
  HAL_Delay(500);
}

/**
 * @brief  Callback processing the ACI events
 * @note   Inside this function each event must be identified and correctly
 *         parsed
 * @param  void* Pointer to the ACI packet
 * @retval None
 */
void APP_UserEvtRx(void *pData)
{
  uint32_t i;

  hci_spi_pckt *hci_pckt = (hci_spi_pckt *)pData;

  if(hci_pckt->type == HCI_EVENT_PKT)
  {
    hci_event_pckt *event_pckt = (hci_event_pckt*)hci_pckt->data;

    if(event_pckt->evt == EVT_LE_META_EVENT)
    {
      evt_le_meta_event *evt = (void *)event_pckt->data;

      for (i = 0; i < (sizeof(hci_le_meta_events_table)/sizeof(hci_le_meta_events_table_type)); i++)
      {
        if (evt->subevent == hci_le_meta_events_table[i].evt_code)
        {
          hci_le_meta_events_table[i].process((void *)evt->data);
        }
      }
    }
    else if(event_pckt->evt == EVT_VENDOR)
    {
      evt_blue_aci *blue_evt = (void*)event_pckt->data;

      for (i = 0; i < (sizeof(hci_vendor_specific_events_table)/sizeof(hci_vendor_specific_events_table_type)); i++)
      {
        if (blue_evt->ecode == hci_vendor_specific_events_table[i].evt_code)
        {
          hci_vendor_specific_events_table[i].process((void *)blue_evt->data);
        }
      }
    }
    else
    {
      for (i = 0; i < (sizeof(hci_events_table)/sizeof(hci_events_table_type)); i++)
      {
        if (event_pckt->evt == hci_events_table[i].evt_code)
        {
          hci_events_table[i].process((void *)event_pckt->data);
        }
      }
    }
  }
}

#ifdef BLUENRG_STACK_UPDATER_YMODEM
/**
  * @brief This function handles USART2 global interrupt.
  */
[#if !knownUSARTInstance]
#warning "Check the correct USART instance is used for the IRQ Handler"
void USART2_IRQHandler(void)
[#else]
void ${USARTInstance}_IRQHandler(void)
[/#if]
{
  uint8_t tmp_rx_data = 0;
  
  if (__HAL_UART_GET_FLAG(&hcom_uart[COM1], UART_FLAG_RXNE) == 1) {
    
    HAL_UART_Receive(&hcom_uart[COM1], &tmp_rx_data, 1, 1000);
    
    processInputData(&tmp_rx_data, 1);
  }
  if (__HAL_UART_GET_FLAG(&hcom_uart[COM1], UART_FLAG_ORE) == 1) {
    __HAL_UART_CLEAR_OREFLAG(&hcom_uart[COM1]);
  }
}
#endif /* BLUENRG_STACK_UPDATER_YMODEM */
