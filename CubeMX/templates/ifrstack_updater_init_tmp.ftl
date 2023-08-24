[#ftl]
  uint8_t ret;

  User_Init();

  /* LED ON for initial status */
  BSP_LED_On(LED2);

  hci_init(APP_UserEvtRx, NULL);

  printf("BlueNRG-2 IFR/Stack Updater.\r\n");

#if defined (BLUENRG_DEV_CONFIG_UPDATER)
  /* Verify if the DEV_CONFIG memory and then update it if needed */
  printf("Verify IFR configuration...\r\n");
  ret = verify_DEV_CONFIG(&deviceConfig);

  if (ret) {
    printf("Start IFR updater...\r\n");
    ret = program_DEV_CONFIG(&deviceConfig);
    if (ret) {
      printf("... error.\r\n\n");
      while(1) {
        BSP_LED_Toggle(LED2);
        HAL_Delay(3000);
      }
    }
  }
#endif /* BLUENRG_DEV_CONFIG_UPDATER */

#if defined (BLUENRG_STACK_UPDATER)
  /* Update the DTM image using the FW_IMAGE provided in the file update_fw_image.c */
  printf("Start Stack updater...\r\n");
  ret = program_device(FW_IMAGE, FW_IMAGE_SIZE);
  if (ret) {
    printf("... error.\r\n\n");
    while(1) {
      BSP_LED_Toggle(LED2);
      HAL_Delay(3000);
    }
  }
#endif /* BLUENRG_STACK_UPDATER */
  
#if defined (BLUENRG_STACK_UPDATER_YMODEM)
  /* Update the BlueNRG-2 image through PC using serial terminal application */
  printf("Start Stack updater using YMODEM...\r\n");
  
  /* Enable IRQ for reception  */
  __HAL_UART_ENABLE_IT(&hcom_uart[COM1], UART_IT_RXNE);
  
  Ymodem_Init();
  
  ret = program_device_UART();
  if (ret) {
    printf("... error.\r\n\n");
    while(1) {
      BSP_LED_Toggle(LED2);
      HAL_Delay(3000);
    }
  }
#endif /* BLUENRG_STACK_UPDATER_YMODEM */

  printf("... end.\r\n\n");
