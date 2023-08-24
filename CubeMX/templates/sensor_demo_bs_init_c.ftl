[#ftl]
  uint8_t ret;
  
  User_Init();
  
  /* Get the User Button initial state */
  user_button_init_state = BSP_PB_GetState(BUTTON_KEY);
  
  hci_init(APP_UserEvtRx, NULL);

  PRINT_DBG("\033[2J"); /* serial console clear screen */
  PRINT_DBG("\033[H");  /* serial console cursor to home */
  PRINT_DBG("BlueNRG-2 SensorDemo_BLESensor-App Application\r\n"); 
  
  /* Init Sensor Device */
  ret = Sensor_DeviceInit();
  if (ret != BLE_STATUS_SUCCESS)
  {
    BSP_LED_On(LED2);
    PRINT_DBG("SensorDeviceInit()--> Failed 0x%02x\r\n", ret);
    while(1);
  }

  PRINT_DBG("BLE Stack Initialized & Device Configured\r\n");

#ifdef STM32L476xx [#-- #if McuName=="STM32L476RGTx" --]
  /* Check the BootLoader Compliance */
  if (CheckBootLoaderCompliance()) {
    PRINT_DBG("BootLoader Compliant with FOTA procedure\r\n\n");
  } 
  else {
    PRINT_DBG("ERROR: BootLoader NOT Compliant with FOTA procedure\r\n\n");
  }
#endif /* STM32L476xx */ [#-- /#if --]