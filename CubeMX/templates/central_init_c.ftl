[#ftl]
  uint8_t ret;
  
  User_Init();
  
  BSP_LED_On(LED2);
  
  hci_init(APP_UserEvtRx, NULL);
      
  PRINT_DBG("BlueNRG-2 Central Device\r\n"); 
  
  /* Initialize the Central Device */
  ret = CentralDevice_Init();
  if (ret != BLE_STATUS_SUCCESS)
  {
    PRINT_DBG("CentralDevice_Init()--> Failed 0x%02x\r\n", ret);
    message("/* **************** Init failed!!! ***************** */\r\n\n");
    while(1);
  }
  
  PRINT_DBG("\nBLE Stack Initialized & Central Device Configured\r\n");
  
  allow_console = TRUE;
  BSP_LED_Off(LED2);
