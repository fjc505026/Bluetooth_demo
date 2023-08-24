[#ftl]  
  User_Init();
  
  hci_init(NULL, NULL);
  
  /* Init the Device */
  Device_Init();

  /* Start Beacon Non Connectable Mode*/
  Start_Beaconing();
  
  PRINT_DBG("BlueNRG-2 BLE Beacon Application\r\n");

