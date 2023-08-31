/**
  ******************************************************************************
  * @file    app_bluenrg_2.h
  * @author  SRA Application Team
  * @brief   Header file for app_bluenrg_2.c
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef APP_BLUENRG_2_H
#define APP_BLUENRG_2_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_nucleo.h"


#define BLUENRG2_DEV_NAME   "LUMEN-BLE_Device"
#define BLUENRG2_DEV_NAME_LEN  ( sizeof( BLUENRG2_DEV_NAME ) )


void BLUENRG2_vInit(void);
void BLUENRG2_vProcess(void);



#ifdef __cplusplus
}
#endif
#endif /* APP_BLUENRG_2_H */
