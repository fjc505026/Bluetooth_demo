/**
  ******************************************************************************
  * @file    gatt_db.h
  * @author  SRA Application Team
  * @brief   Header file for gatt_db.c
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
#ifndef GATT_DB_H
#define GATT_DB_H

#include "hci.h"


#define GATT_DB_CUSTOM_UUID_BYTE_NUM ( 16U )

// UUIDs:
// D973F2E0-B19E-11E2-9E96-0800200C9A66
// D973F2E1-B19E-11E2-9E96-0800200C9A66
// D973F2E2-B19E-11E2-9E96-0800200C9A66
#define GATT_DB_CONTROL_UUID   { 0x66U, 0x9aU, 0x0cU, 0x20U, 0x00U, 0x08U, 0x96U, 0x9eU, 0xe2U, 0x11U, 0x9eU, 0xb1U, 0xe0U, 0xf2U, 0x73U, 0xd9U }
#define GATT_DB_LOCAL_TX_UUID  { 0x66U, 0x9aU, 0x0cU, 0x20U, 0x00U, 0x08U, 0x96U, 0x9eU, 0xe2U, 0x11U, 0x9eU, 0xb1U, 0xe1U, 0xf2U, 0x73U, 0xd9U }
#define GATT_DB_LOCAL_RX_UUID  { 0x66U, 0x9aU, 0x0cU, 0x20U, 0x00U, 0x08U, 0x96U, 0x9eU, 0xe2U, 0x11U, 0x9eU, 0xb1U, 0xe2U, 0xf2U, 0x73U, 0xd9U }

/**
 * This sample application uses a char value length greater than 20 bytes
 * (typically used).
 * For using greater values for CHAR_VALUE_LENGTH (max 512) and
 * CLIENT_MAX_MTU_SIZE (max 247):
 * - increase both the two #define below to their max values
 * - increase both the HCI_READ_PACKET_SIZE and HCI_MAX_PAYLOAD_SIZE to 256
 *   (file bluenrg_conf.h)
 * - increase the CSTACK in the IDE project options (0xC00 is enough)
*/
#define CHAR_VALUE_LENGTH   ( 63U )
#define CLIENT_MAX_MTU_SIZE ( 158U )

uint8_t GATT_DB_u8AddService(void);
void APP_UserEvtRx(void *pData);

extern uint16_t sampleServHandle, TXCharHandle, RXCharHandle;

#endif /* GATT_DB_H */
