/**
 ******************************************************************************
 * @file    app_state.h
 * @author  SRA Application Team
 * @brief   Defines for state machine
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
#ifndef APP_STATE_H
#define APP_STATE_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Exported variables ------------------------------------------------------- */
/**
 * @brief  Device Role
 */
#define MASTER_ROLE 0x00
#define SLAVE_ROLE  0x01



    /**
     * @brief  Variable which contains some flags useful for application
     */
    extern volatile int app_flags;

/**
 * @brief  Flags for application
 */
#define APP_STAT_SCANNING           ( 0x0100U )
#define APP_STAT_DEV_FOUND          ( 0x0200U )
#define APP_STAT_DEV_CONNECTED      ( 0x0400U )
#define APP_STAT_DEV_NOTIFY_ENABLED ( 0x0800U )

/* Added flags for handling TX, RX characteristics discovery */
#define START_READ_TX_CHAR_HANDLE 0x0800
#define END_READ_TX_CHAR_HANDLE   0x1000
#define START_READ_RX_CHAR_HANDLE 0x2000
#define END_READ_RX_CHAR_HANDLE   0x4000

/* GATT EVT_BLUE_GATT_TX_POOL_AVAILABLE event */
#define TX_BUFFER_FULL 0x8000

/* Exported macros -----------------------------------------------------------*/
#define APP_FLAG( flag ) ( app_flags & flag )

#define APP_FLAG_SET( flag )   ( app_flags |= flag )
#define APP_FLAG_CLEAR( flag ) ( app_flags &= ~flag )

#ifdef __cplusplus
}
#endif

#endif /* APP_STATE_H */
