[#ftl]
/**
  ******************************************************************************
  * @file    ${name}
  * @author  SRA Application Team
  * @brief   Header file for ${name?replace(".h",".c")}
  ******************************************************************************
  * @attention
  *
  * Copyright (c) ${year} STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */#n
#ifndef SENSOR_H
#define SENSOR_H

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* Exported defines ----------------------------------------------------------*/
#define SENSOR_DEMO_NAME   'B','l','u','e','N','R','G'
#define BDADDR_SIZE        6

/* Exported function prototypes ----------------------------------------------*/
void Set_DeviceConnectable(void);
void APP_UserEvtRx(void *pData);

#endif /* SENSOR_H */
