[#ftl]
  uint8_t ret;
  
  User_Init();
  
  hci_init(APP_UserEvtRx, NULL);
      
  PRINT_DBG("BlueNRG-2 BLE Sample Application\r\n"); 
  
  /* Init Sample App Device */
  ret = SampleAppInit();
  if (ret != BLE_STATUS_SUCCESS)
  {
    PRINT_DBG("SampleAppInit()--> Failed 0x%02x\r\n", ret);
    while(1);
  }

  PRINT_DBG("BLE Stack Initialized & Device Configured\r\n");

