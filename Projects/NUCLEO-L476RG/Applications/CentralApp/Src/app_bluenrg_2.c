/**
 ******************************************************************************
 * @file    app_bluenrg_2.c
 * @author  SRA Application Team
 * @brief   BlueNRG-2 initialization and applicative code
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

/* Includes ------------------------------------------------------------------*/
#include "app_bluenrg_2.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "bluenrg1_aci.h"
#include "bluenrg1_hci_le.h"
#include "bluenrg1_events.h"
#include "hci_tl.h"
#include "gatt_db.h"

#define BLUENRG2_AUTHENTICATION_PASS_KEY ( 123456U )
#define BLUENRG2_FSM_GENERIC_TIMEOUT_MS  ( 60 * 1000U )

//RSSI
#define BLUENRG2_Q_SZ                ( 5U )   // in byte
#define BLUENRG2_RSSI_READING_PERIOD ( 500U ) // in ms, Read RSSI Value every 0.5 sec
#define BLUENRG2_RSSI_THRESHOLD_FAR  ( -85 )  // in dBm, ~3M
#define BLUENRG2_RSSI_THRESHOLD_MID  ( -75 )  // in dBm,~1M
#define BLUENRG2_RSSI_THRESHOLD_NEAR ( -65 )  // in dBm, ~0.5M
#define BLUENRG2_RSSI_INVALID_VALUE  ( 127 )

// From user manual
#define BLUENRG2_TX_POWER_HIGH    ( 1U )
#define BLUENRG2_TX_POWER_N15_DBM ( 0U ) // -15 dBm
#define BLUENRG2_TX_POWER_N12_DBM ( 1U ) // -12 dBm
#define BLUENRG2_TX_POWER_N8_DBM  ( 2U ) //  -8 dBm
#define BLUENRG2_TX_POWER_N5_DBM  ( 3U ) //  -5 dBm
#define BLUENRG2_TX_POWER_N2_DBM  ( 4U ) //  -2 dBm
#define BLUENRG2_TX_POWER_1_DBM   ( 5U ) //   1 dBm
#define BLUENRG2_TX_POWER_5_DBM   ( 6U ) //   5 dBm
#define BLUENRG2_TX_POWER_8_DBM   ( 7U ) //   8 dBm

#define BLUENRG2_AUTHENTICATION_ADDR_PUBLIC_ID ( 0x00U )
#define BLUENRG2_AUTHENTICATION_ADDR_RANDOM_ID ( 0x01U )

#define BLUENRG2_BDADDR_BYTE_NUM ( 6U )

#define BLUENRG2_MASTER_ROLE ( 0x00U )
#define BLUENRG2_SLAVE_ROLE  ( 0x00U )

#define BLUENRG2_CHAR_VALUE_OFFSET       ( 1U )
#define BLUENRG2_CHAR_CONFIG_DESC_OFFSET ( 2U ) // char config descriptor offset to char definition handle

#define BLUENRG2_GENERIC_HANDLE_MIN ( 0x0001U )
#define BLUENRG2_GENERIC_HANDLE_MAX ( 0xFFFFU )

typedef enum
{
    BLUENRG2_STAT_INIT = 0U, //  Reset variables, Start scan
    BLUENRG2_STAT_SCANNING,
    BLUENRG2_STAT_SCAN_DONE,
    BLUENRG2_STAT_START_CONNECT,
    BLUENRG2_STAT_CONNECTING,
    BLUENRG2_STAT_GET_REMOTE_TX,
    BLUENRG2_STAT_GET_REMOTE_RX,
    BLUENRG2_STAT_FORCE_REMOTE_TX_NOTIFY,
    BLUENRG2_STAT_MAIN_CONNECTED,
    BLUENRG2_STAT_IDLE,
} BLUENRG2_tenState;

// Peripheral Dev struct to be talk
typedef struct
{
    bool    bValid;
    uint8_t u8Type;
    uint8_t au8Addr[BLUENRG2_BDADDR_BYTE_NUM];
} BLUENRG2_tstDeviceInfo;

// GATT Characteristics  struct
typedef struct
{
    bool     bValid;
    UUID_t   uUUID;
    uint16_t u16Handle;
} BLUENRG2_tstCharContext;

// Connection struct
typedef struct
{
    bool bHasConnection;

    uint16_t u16Handle;
    uint8_t  u8Role;

    BLUENRG2_tstCharContext RemoteTx;
    BLUENRG2_tstCharContext RemoteRx;

    BLUENRG2_tstDeviceInfo stDevice;
} BLUENRG2_tstConnectionContext;

extern uint16_t u16LocalTxCharHandle, u16LocalRxCharHandle;

static const uint8_t BLUENRG2__cau8RemoteTxCharUUID[] = { 0x66, 0x9a, 0x0c, 0x20, 0x00, 0x08, 0x96, 0x9e,
                                                          0xe2, 0x11, 0x9e, 0xb1, 0xe1, 0xf2, 0x73, 0xd9 };

static const uint8_t BLUENRG2__cau8RemoteRxCharUUID[] = { 0x66, 0x9a, 0x0c, 0x20, 0x00, 0x08, 0x96, 0x9e,
                                                          0xe2, 0x11, 0x9e, 0xb1, 0xe2, 0xf2, 0x73, 0xd9 };

static struct
{
    int8_t  i8Buffer[BLUENRG2_Q_SZ];
    uint8_t u8Idx;
    bool    bValid;
} BLUENRG2__st8Queue;

static BLUENRG2_tenState BLUENRG2__enState = BLUENRG2_STAT_INIT;

static BLUENRG2_tstConnectionContext BLUENRG2__stConnCTX = { 0 };

static uint32_t BLUENRG2__u32RxDataCnt = 0U;

static bool BLUENRG2__bTXbufferFull          = false;
static bool BLUENRG2__bRemoteTxNotifyEnabled = false;
static bool BLUENRG2__bMasterDevIsUnlocked   = false;

static uint8_t BLUENRG2__au8DataBuf[CHAR_VALUE_LENGTH - 3];

uint8_t  mtu_exchanged      = 0;
uint8_t  mtu_exchanged_wait = 0;
uint16_t write_char_len     = CHAR_VALUE_LENGTH - 3;

//*******************************************************************************
//                       Private function prototypes
//*******************************************************************************
static void    BLUENRG2__vUserInit( void );
static uint8_t BLUENRG2__u8CentralAppInit( void );
static void    BLUENRG2__vUserProcess( void );

static void BLUENRG2__vReceiveData( uint8_t *pu8Data, uint8_t u8DataLen_Byte );

static void BLUENRG2__vStartScan( void );
static void BLUENRG2__vStopScan( void );
static void BLUENRG2__vStartConnect( void );

static void    BLUENRG2__vResetConnectionContext( void );
static uint8_t BLUENRG2__u8PrintBLEFwInfo( void );
static bool    BLUENRG2__bFindDeviceName( uint8_t u8DataLen, uint8_t *pu8Data );

static void BLUENRG2__vUpdateLockStatus( int8_t i8Rssi, int8_t i8LockRssiTh, bool bDebounceEnable );
static void BLUENRG2__vAttributeModifiedCB( uint16_t u16AttributeHandle, uint8_t u8DataLen, uint8_t *pau8AttrData );

static int8_t BLUENRG2__i8GetProcessedRSSI( int8_t i8RSSIVal );
static bool   BLUENRG2__bIsProcessedRSSIValid( void );

//*****************************************************
//! \brief  This function is used to send data related to the sample service
//!        (to be sent over the air to the remote board).
//! @param  data_buffer : pointer to data to be sent
//! @param  Nb_bytes : number of bytes to send
//! \return
//!   None
//*****************************************************
static void sendData( uint8_t *data_buffer, uint8_t Nb_bytes ) // CAN BE REMOVED
{
    uint32_t U32Tickstart = HAL_GetTick();

    while( BLE_STATUS_NOT_ALLOWED ==
           aci_gatt_write_without_resp( BLUENRG2__stConnCTX.u16Handle,
                                        BLUENRG2__stConnCTX.RemoteRx.u16Handle + BLUENRG2_CHAR_VALUE_OFFSET, Nb_bytes,
                                        data_buffer ) )
    {
        hci_user_evt_proc();
        // Radio is busy (buffer full).
        if( ( HAL_GetTick() - U32Tickstart ) > ( 10 * HCI_DEFAULT_TIMEOUT_MS ) )
            break;
    }

    // if( BLUENRG2__stConnCTX.u8Role == SLAVE_ROLE )
    // {
    //     while( aci_gatt_update_char_value_ext( BLUENRG2__stConnCTX.u16Handle, sampleServHandle, u16LocalTxCharHandle, 1,
    //                                            Nb_bytes, 0, Nb_bytes,
    //                                            data_buffer ) == BLE_STATUS_INSUFFICIENT_RESOURCES )
    //     {
    //         BLUENRG2__bTXbufferFull = true;
    //         while( BLUENRG2__bTXbufferFull )
    //         {
    //             hci_user_evt_proc();
    //             // Radio is busy (buffer full).
    //             if( ( HAL_GetTick() - tickstart ) > ( 10 * HCI_DEFAULT_TIMEOUT_MS ) )
    //                 break;
    //         }
    //     }
    // }
}

//*******************************************************************************
//                       Global API function
//*******************************************************************************
//*****************************************************
//! \brief  BlueNRG2 application init
//!
//!
//! \pre
//!   None
//!
//! \post
//!   BLUENRG2_vProcess
//!
//! \return
//!   None
//*****************************************************
void BLUENRG2_vInit( void )
{
    BLUENRG2__vUserInit();
    hci_init( APP_UserEvtRx, NULL );
    PRINT_DBG( "BlueNRG-2 BLE Sample Application\r\n" );

    if( BLE_STATUS_SUCCESS != BLUENRG2__u8CentralAppInit() )
    {
        PRINT_DBG( "BLUENRG2__u8CentralAppInit() Failed\r\n" );
        while( true )
            ;
    }

    PRINT_DBG( "BLE Stack Initialized & Device Configured\r\n" );
}

//*****************************************************
//! \brief  BlueNRG2 application main loop
//!
//!
//! \pre
//!   BLUENRG2_vInit
//!
//! \post
//!   None
//!
//! \return
//!   None
//*****************************************************
void BLUENRG2_vProcess( void )
{

    hci_user_evt_proc();
    BLUENRG2__vUserProcess();
}

//*******************************************************************************
//                       Private functions
//*******************************************************************************
//*****************************************************
//! \brief  Initialize User process.
//!
//!
//! \pre
//!   BLUENRG2_vInit
//!
//! \post
//!   None
//!
//! \return
//!   None
//*****************************************************
static void BLUENRG2__vUserInit( void )
{
    BSP_PB_Init( BUTTON_KEY, BUTTON_MODE_GPIO );
    BSP_LED_Init( LED2 );

    BSP_COM_Init( COM1 );
}

//*****************************************************
//! \brief  BLUENRG2__u8CentralAppInit.
//!          Init CentralApp.
//!
//! \return
//!   Status
//*****************************************************
static uint8_t BLUENRG2__u8CentralAppInit( void )
{
    uint8_t  u8Ret;
    uint16_t u16ServiceHandle, u16DevNameCharHandle, u16AppearanceCharHandle;
    uint8_t  au8BdAddr[BLUENRG2_BDADDR_BYTE_NUM];
    uint8_t  u8BaAddrLen;

    const uint8_t cu8NVMConfigDataAddr = 0x80U; // Offset of the static random address stored in NVM

    hci_reset(); // Sw reset of the BLE chip

    HAL_Delay( 2000 ); // BlueNRG device requires 2000ms delay for booting

    u8Ret = BLUENRG2__u8PrintBLEFwInfo();

    if( u8Ret != BLE_STATUS_SUCCESS )
    {
        PRINT_DBG( "Get Bluetooth Chip Info failed !\r\n" );
        return u8Ret;
    }

    u8Ret = aci_hal_read_config_data( cu8NVMConfigDataAddr, &u8BaAddrLen, au8BdAddr );

    if( ( u8Ret != BLE_STATUS_SUCCESS ) || ( 0xC0 != ( au8BdAddr[5U] & 0xC0 ) ) )
    {
        PRINT_DBG( "Read Broadcast Address failed !\r\n" );
        while( true )
            ;
        return u8Ret;
    }

    aci_hal_set_tx_power_level( BLUENRG2_TX_POWER_HIGH, BLUENRG2_TX_POWER_N2_DBM );

    u8Ret = aci_gatt_init();
    if( u8Ret != BLE_STATUS_SUCCESS )
    {
        PRINT_DBG( "GATT_Init failed: 0x%02x\r\n", u8Ret );
        return u8Ret;
    }

    u8Ret = aci_gap_init( GAP_CENTRAL_ROLE, 0x0, (uint8_t) BLUENRG2_DEV_NAME_LEN, &u16ServiceHandle,
                          &u16DevNameCharHandle, &u16AppearanceCharHandle );
    if( u8Ret != BLE_STATUS_SUCCESS )
    {
        PRINT_DBG( "GAP_Init failed: 0x%02x\r\n", u8Ret );
        return u8Ret;
    }

    // Update device name
    uint8_t au8TempDeviceName[BLUENRG2_DEV_NAME_LEN] = BLUENRG2_DEV_NAME;
    u8Ret = aci_gatt_update_char_value( u16ServiceHandle, u16DevNameCharHandle, (uint8_t) 0U,
                                        (uint8_t) BLUENRG2_DEV_NAME_LEN, au8TempDeviceName );

    // Clear security database: this implies that each time the application is executed
    // the full bonding process is executed (with PassKey generation and setting).
    u8Ret = aci_gap_clear_security_db();
    if( u8Ret != BLE_STATUS_SUCCESS )
    {
        PRINT_DBG( "aci_gap_clear_security_db() failed:\r\n" );
    }

    //  Set the I/O capability otherwise the Central device (e.g. the smartphone) will
    //  propose a PIN that will be accepted without any control.
    if( aci_gap_set_io_capability( IO_CAP_DISPLAY_ONLY ) == BLE_STATUS_SUCCESS )
    {
        PRINT_DBG( "I/O Capability Configurated\r\n" );
    }

    // BLE Security v4.2 is supported: BLE stack FW version >= 2.x (new API prototype)
    // clang-format off
    u8Ret = aci_gap_set_authentication_requirement( BONDING, MITM_PROTECTION_REQUIRED, SC_IS_SUPPORTED,
                                                    KEYPRESS_IS_NOT_SUPPORTED, 7, 16, USE_FIXED_PIN_FOR_PAIRING,
                                                    BLUENRG2_AUTHENTICATION_PASS_KEY, BLUENRG2_AUTHENTICATION_ADDR_PUBLIC_ID );
    // clang-format on

    if( u8Ret != BLE_STATUS_SUCCESS )
    {
        PRINT_DBG( "aci_gap_set_authentication_requirement()failed\r\n" );
        return u8Ret;
    }

    u8Ret = GATT_DB_u8AddService(); // Add Device Service & Characteristics
    if( u8Ret != BLE_STATUS_SUCCESS )
    {
        PRINT_DBG( "Error while adding service: 0x%02x\r\n", u8Ret );
        return u8Ret;
    }

    return BLE_STATUS_SUCCESS;
}
//*****************************************************
//! \brief  BlueNRG2 User Defined FSM
//!
//!
//! \pre
//!   BLUENRG2_vInit
//!
//! \post
//!   None
//!
//! \return
//!   None
//*****************************************************
static void BLUENRG2__vUserProcess( void )
{
    static uint32_t          u32LastRSSIReadTick;
    static uint32_t          u32CurrentFSMStartTick;
    static BLUENRG2_tenState enLastFSMState;

    if( enLastFSMState != BLUENRG2__enState )
    {
        PRINT_DBG( "FSM : %d -> %d \r\n", enLastFSMState, BLUENRG2__enState );
        enLastFSMState = BLUENRG2__enState;
    }

    switch( BLUENRG2__enState )
    {
        case BLUENRG2_STAT_INIT:
        {
            BLUENRG2__vResetConnectionContext(); // Reset Connection context
            BLUENRG2__vStartScan();              // Result in hci_le_advertising_report_event
            BLUENRG2__enState      = BLUENRG2_STAT_SCANNING;
            u32CurrentFSMStartTick = HAL_GetTick();
        }
        break;

        case BLUENRG2_STAT_SCANNING:
        {
            if( HAL_GetTick() - u32CurrentFSMStartTick > BLUENRG2_FSM_GENERIC_TIMEOUT_MS )
            {
                PRINT_DBG( "Timeout at STATE %d \r\n ", (uint8_t) BLUENRG2__enState );
                BLUENRG2__vStopScan();
                u32CurrentFSMStartTick = HAL_GetTick();
                BLUENRG2__enState      = BLUENRG2_STAT_IDLE;
            }
        }
        break;

        case BLUENRG2_STAT_SCAN_DONE:
        {
            BLUENRG2__vStopScan();
            BLUENRG2__enState = BLUENRG2_STAT_IDLE;
        }
        break;

        case BLUENRG2_STAT_START_CONNECT:
        {

            const char *apcDevAddrTypeStr[] = { "Public Device", "Random Device", "Public Identity",
                                                "Random Identity" };

            printf( "[Connecting] Device Addr Type : %s, Value : ",
                    apcDevAddrTypeStr[BLUENRG2__stConnCTX.stDevice.u8Type] );

            for( uint8_t u8ByteIdx = sizeof( BLUENRG2__stConnCTX.stDevice.au8Addr ); u8ByteIdx != 0U; u8ByteIdx-- )
            {
                printf( "%02X ", BLUENRG2__stConnCTX.stDevice.au8Addr[u8ByteIdx - 1] );
            }
            printf( "\r\n" );

            BLUENRG2__vStartConnect();
            BLUENRG2__enState      = BLUENRG2_STAT_CONNECTING;
            u32CurrentFSMStartTick = HAL_GetTick();
        }
        break;

        case BLUENRG2_STAT_CONNECTING:
        {
            if( HAL_GetTick() - u32CurrentFSMStartTick > BLUENRG2_FSM_GENERIC_TIMEOUT_MS )
            {
                PRINT_DBG( "Timeout at STATE %d \r\n ", (uint8_t) BLUENRG2__enState );
                BLUENRG2__enState = BLUENRG2_STAT_IDLE;
            }
        }
        break;

        case BLUENRG2_STAT_GET_REMOTE_TX:
        {
            if( !BLUENRG2__stConnCTX.RemoteTx.bValid )
            {
                aci_gatt_disc_char_by_uuid( BLUENRG2__stConnCTX.u16Handle, BLUENRG2_GENERIC_HANDLE_MIN,
                                            BLUENRG2_GENERIC_HANDLE_MAX, UUID_TYPE_128,
                                            &BLUENRG2__stConnCTX.RemoteTx.uUUID );
            }
            BLUENRG2__enState = BLUENRG2_STAT_IDLE;
        }
        break;

        case BLUENRG2_STAT_GET_REMOTE_RX:
        {
            if( !BLUENRG2__stConnCTX.RemoteRx.bValid )
            {
                aci_gatt_disc_char_by_uuid( BLUENRG2__stConnCTX.u16Handle, BLUENRG2_GENERIC_HANDLE_MIN,
                                            BLUENRG2_GENERIC_HANDLE_MAX, UUID_TYPE_128,
                                            &BLUENRG2__stConnCTX.RemoteRx.uUUID );
            }
            BLUENRG2__enState = BLUENRG2_STAT_IDLE;
        }
        break;

        case BLUENRG2_STAT_FORCE_REMOTE_TX_NOTIFY:
        {

            uint8_t au8ClientCharConfigData[] = { 0x01, 0x00 }; // Enable notifications

            u32CurrentFSMStartTick = HAL_GetTick();
            while( aci_gatt_write_char_desc( BLUENRG2__stConnCTX.u16Handle,
                                             BLUENRG2__stConnCTX.RemoteTx.u16Handle + BLUENRG2_CHAR_CONFIG_DESC_OFFSET,
                                             sizeof( au8ClientCharConfigData ),
                                             au8ClientCharConfigData ) == BLE_STATUS_NOT_ALLOWED )
            {
                // Radio is busy.
                if( ( HAL_GetTick() - u32CurrentFSMStartTick ) > ( 10 * HCI_DEFAULT_TIMEOUT_MS ) )
                    break;
            }
            BLUENRG2__bRemoteTxNotifyEnabled = true;

            BLUENRG2__enState = BLUENRG2_STAT_MAIN_CONNECTED;
        }
        break;

        case BLUENRG2_STAT_MAIN_CONNECTED:
        {

            if( HAL_GetTick() - u32LastRSSIReadTick > BLUENRG2_RSSI_READING_PERIOD )
            {
                int8_t u8TmpRssi;
                hci_read_rssi( BLUENRG2__stConnCTX.u16Handle, &u8TmpRssi );
                BLUENRG2__vUpdateLockStatus( u8TmpRssi, BLUENRG2_RSSI_THRESHOLD_NEAR, true );
                u32LastRSSIReadTick = HAL_GetTick();
            }

            if( BLUENRG2__bMasterDevIsUnlocked )
            {
                if( 1U == BSP_PB_GetState( BUTTON_KEY ) )
                {
                    BSP_LED_Off( LED2 );
                }
                else
                {
                    BSP_LED_On( LED2 );
                }
            }
            else
            {
                BSP_LED_On( LED2 );
            }
        }
        break;

        case BLUENRG2_STAT_IDLE:
        {
            if( HAL_GetTick() - u32CurrentFSMStartTick > BLUENRG2_FSM_GENERIC_TIMEOUT_MS )
            {
                PRINT_DBG( "Timeout at STATE %d \r\n ", (uint8_t) BLUENRG2__enState );
                u32CurrentFSMStartTick = HAL_GetTick();
                BLUENRG2__enState      = BLUENRG2_STAT_INIT;
            }
        }
        break;

        default:
            break;
    }
}

//*****************************************************
//! \brief  This function is used to receive data related to the sample service
//!          (received over the air from the remote board).
//!
//! \param  pu8Data : pointer to store in received data
//! \param  u8DataLen_Byte : number of bytes to be received
//!
//! \return
//!   None
//*****************************************************
static void BLUENRG2__vReceiveData( uint8_t *pu8Data, uint8_t u8DataLen_Byte )
{

    // PRINT_DBG("[RX]:");
    // for(int i = 0; i < u8DataLen_Byte; i++)
    // {
    //   PRINT_DBG("%d", pu8Data[i]);
    // }
    // fflush(stdout);

    if( 1U == pu8Data[0] )
    {
        BLUENRG2__u32RxDataCnt++;
        PRINT_DBG( "[RX] Notified\r\n" );
    }
}

//*****************************************************
//! \brief  BLUENRG2__u8CentralAppInit.
//!          Init CentralApp.
//!
//! \pre
//!   None
//!
//! \post
//!   None
//!
//! \return
//!   Status
//*****************************************************
static void BLUENRG2__vStartScan( void )
{
    uint8_t u8Ret = aci_gap_start_general_discovery_proc( SCAN_P, SCAN_L, PUBLIC_ADDR, 0x00 );
    if( BLE_STATUS_SUCCESS != u8Ret )
    {
        printf( "aci_gap_start_general_discovery_proc() failed, %#X\n", u8Ret );
    }
    else
    {
        printf( "aci_gap_start_general_discovery_proc() Done\r\n" );
    }
}

//*****************************************************
//! \brief  BLUENRG2__u8CentralAppInit.
//!          Init CentralApp.
//!
//! \pre
//!   None
//!
//! \post
//!   None
//!
//! \return
//!   Status
//*****************************************************
static void BLUENRG2__vStopScan( void )
{
    uint8_t u8Ret = aci_gap_terminate_gap_proc( GAP_GENERAL_DISCOVERY_PROC );
    if( BLE_STATUS_SUCCESS != u8Ret )
    {
        printf( "aci_gap_terminate_gap_proc() failed, %#X\n", u8Ret );
    }
    else
    {
        printf( "aci_gap_terminate_gap_proc() Done\r\n" );
    }
}

//*****************************************************
//! \brief  BLUENRG2__u8CentralAppInit.
//!          Init CentralApp.
//!
//! \pre
//!   None
//!
//! \post
//!   None
//!
//! \return
//!   Status
//*****************************************************
static void BLUENRG2__vStartConnect( void )
{

    // Do connection with first discovered device
    uint8_t u8Ret =
    aci_gap_create_connection( SCAN_P, SCAN_L, BLUENRG2__stConnCTX.stDevice.u8Type,
                               BLUENRG2__stConnCTX.stDevice.au8Addr, PUBLIC_ADDR, 40, 40, 0, 60, 2000, 2000 );
    if( u8Ret != BLE_STATUS_SUCCESS )
    {
        PRINT_DBG( "aci_gap_create_connection() failed: 0x%02x\r\n", u8Ret );
    }
    else
    {
        PRINT_DBG( "aci_gap_create_connection() OK\r\n" );
    }
}

/*******************************************************************************
 * Function Name  : BLUENRG2__vResetConnectionContext.
 * Description    : Reset the discovery context.
 * Input          : None.
 * Return         : None.
 *******************************************************************************/
static void BLUENRG2__vResetConnectionContext( void )
{
    BSP_LED_On( LED2 );
    BLUENRG_memset( &BLUENRG2__stConnCTX, 0, sizeof( BLUENRG2__stConnCTX ) );

    BLUENRG2__stConnCTX.RemoteTx.bValid = false;
    BLUENRG_memcpy( BLUENRG2__stConnCTX.RemoteTx.uUUID.UUID_128, BLUENRG2__cau8RemoteTxCharUUID,
                    sizeof( BLUENRG2__cau8RemoteTxCharUUID ) );

    BLUENRG2__stConnCTX.RemoteRx.bValid = false;
    BLUENRG_memcpy( BLUENRG2__stConnCTX.RemoteRx.uUUID.UUID_128, BLUENRG2__cau8RemoteRxCharUUID,
                    sizeof( BLUENRG2__cau8RemoteRxCharUUID ) );

    BLUENRG2__bRemoteTxNotifyEnabled = false;
    BLUENRG2__bTXbufferFull          = false;
    BLUENRG2__bMasterDevIsUnlocked   = false;

    BLUENRG2__st8Queue.bValid = false;
    BLUENRG2__st8Queue.u8Idx  = 0U;

    mtu_exchanged      = 0;
    mtu_exchanged_wait = 0;
    write_char_len     = CHAR_VALUE_LENGTH - 3;

    for( uint16_t i = 0; i < ( CHAR_VALUE_LENGTH - 3 ); i++ )
    {
        BLUENRG2__au8DataBuf[i] = 0x31 + ( i % 10 );
        if( ( i + 1 ) % 10 == 0 )
        {
            BLUENRG2__au8DataBuf[i] = 'x';
        }
    }
}

/*******************************************************************************
 * Function Name  : BLUENRG2__u8PrintBLEFwInfo.
 * Description    : Setup the device address.
 * Input          : None.
 * Return         : None.
 *******************************************************************************/
static uint8_t BLUENRG2__u8PrintBLEFwInfo( void )
{
    uint8_t  u8DTMVerMajor, u8DTMVerMinor, u8DTMVerPatch, u8DTMVerVariant;
    uint16_t u16DTMBuildNum;

    uint8_t  u8BLEStackVerMajor, u8BLEStackVerMinor, u8BLEStackVerPatch, u8BLEStackDevelopment;
    uint16_t u16BLEStackVariant, u16BLEStackBuildNum;

    uint8_t au8Alphabet[] = { ' ', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
                              'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z' };
    // clang-format off
    uint8_t u8Status = aci_hal_get_firmware_details( &u8DTMVerMajor, &u8DTMVerMinor, &u8DTMVerPatch, &u8DTMVerVariant,
                                                     &u16DTMBuildNum, &u8BLEStackVerMajor, &u8BLEStackVerMinor,
                                                     &u8BLEStackVerPatch, &u8BLEStackDevelopment, &u16BLEStackVariant, 
                                                     &u16BLEStackBuildNum 
                                                    );
    // clang-format on
    PRINT_DBG( "\r\n--------------------------------------------------------\r\n\n" );
    PRINT_DBG( " Lumen BLE Central App v%d.%d.%d \r\n", 0, 0, 0 );

    if( u8Status == BLE_STATUS_SUCCESS )
    {
        PRINT_DBG( "- BlueNRG-2 FW v%d.%d%c \r\n", u8BLEStackVerMajor, u8BLEStackVerMinor,
                   au8Alphabet[u8BLEStackVerPatch] );

        PRINT_DBG( "- DTM %s v%d.%d%c \r\n",
                   u8DTMVerVariant == 0x01 ? "UART" : ( u8DTMVerVariant == 0x02 ? "SPI" : "Unknown" ), u8DTMVerMajor,
                   u8DTMVerMinor, au8Alphabet[u8DTMVerPatch] );
    }
    PRINT_DBG( "\r\n" );

    return u8Status;
}

/*******************************************************************************
 * Function Name  : BLUENRG2__bFindDeviceName.
 * Description    : Extracts the device name.
 * Input          : Data length.
 *                  Data value
 * Return         : TRUE if the local name found is the expected one, FALSE otherwise.
 *******************************************************************************/
static bool BLUENRG2__bFindDeviceName( uint8_t u8DataLen, uint8_t *pu8Data )
{
    uint8_t u8Index = 0U;

    while( u8Index < u8DataLen )
    {
        /* Advertising data fields: len, type, values */
        /* Check if field is complete local name and the length is the expected one for BLE
         * CentralApp  */
        if( pu8Data[u8Index + 1U] == AD_TYPE_COMPLETE_LOCAL_NAME )
        {
            // check if found device name is start with LUMEN
            uint8_t au8FindKeyStr[] = "BLE Device"; //TODO

            pu8Data[u8Index + 1 + sizeof( au8FindKeyStr )] = '\0'; // Limit device name string

            if( 0U == BLUENRG_memcmp( &pu8Data[u8Index + 2], au8FindKeyStr, sizeof( au8FindKeyStr ) - 1 ) )
            {
                return TRUE;
            }
            else
            {
                return FALSE;
            }
        }
        else
        {
            u8Index += ( pu8Data[u8Index] + 1U ); // move to next advertising field, Refer to Core 5.2 Figure 11.1
        }
    }

    return FALSE;
}

/*******************************************************************************
 * Function Name  : BLUENRG2__vAttributeModifiedCB,
 * Description    : Attribute modified callback.
 * Input          : Attribute handle modified.
 *                  Length of the data.
 *                  Attribute data.
 * Return         : None.
 *******************************************************************************/
static void BLUENRG2__vAttributeModifiedCB( uint16_t u16AttributeHandle, uint8_t u8DataLen, uint8_t *pau8AttrData )
{
    if( u16AttributeHandle == u16LocalRxCharHandle + BLUENRG2_CHAR_VALUE_OFFSET )
    {
        BLUENRG2__vReceiveData( pau8AttrData, u8DataLen );
    }
    else if( u16AttributeHandle == u16LocalTxCharHandle + BLUENRG2_CHAR_CONFIG_DESC_OFFSET )
    {
        if( pau8AttrData[0] == 0x01 )
        {
            BLUENRG2__bRemoteTxNotifyEnabled = true;
        }
    }
}

//*****************************************************
//! \brief  BLUENRG2__vUpdateLockStatus
//!
//! \param [in]  i8Rssi: Current RSSI value at run-time;
//! \param [in]  i8LockRssiTh : Lock/unlocked threshold
//! \param [in]  bDebounceEnable : Enable/Disable debouncing
//!
//! \return
//!   None
//*****************************************************
static void BLUENRG2__vUpdateLockStatus( int8_t i8Rssi, int8_t i8LockRssiTh, bool bDebounceEnable )
{
    static bool bLastLockState = false;

    if( i8Rssi != BLUENRG2_RSSI_INVALID_VALUE )
    {
        if( !bDebounceEnable )
        {
            if( i8Rssi >= i8LockRssiTh )
            {
                BLUENRG2__bMasterDevIsUnlocked = true;
            }
            else
            {
                BLUENRG2__bMasterDevIsUnlocked = false;
            }
            PRINT_DBG( "[RSSI] raw %d dBm\r\n", i8Rssi );
        }
        else
        {
            int8_t i8ProcessedRssi;

            if( ( i8ProcessedRssi = BLUENRG2__i8GetProcessedRSSI( i8Rssi ) ) && BLUENRG2__bIsProcessedRSSIValid() )
            {
                PRINT_DBG( "[RSSI] raw %d dBm, cali %d dBm\r\n", i8Rssi, i8ProcessedRssi );
                if( i8ProcessedRssi >= i8LockRssiTh )
                {
                    BLUENRG2__bMasterDevIsUnlocked = true;
                }
                else
                {
                    BLUENRG2__bMasterDevIsUnlocked = false;
                }
            }
        }
    }

    if( bLastLockState != BLUENRG2__bMasterDevIsUnlocked )
    {
        bLastLockState = BLUENRG2__bMasterDevIsUnlocked;
        PRINT_DBG( "%s !!! \r\n", BLUENRG2__bMasterDevIsUnlocked ? "UNLOCKED" : "LOCKED" );
    }
}

static int8_t BLUENRG2__i8GetProcessedRSSI( int8_t i8RSSIVal )
{
    int8_t i8ReturnValue = 0;

    BLUENRG2__st8Queue.i8Buffer[BLUENRG2__st8Queue.u8Idx] = i8RSSIVal;
    BLUENRG2__st8Queue.u8Idx++;

    if( BLUENRG2__st8Queue.u8Idx >= BLUENRG2_Q_SZ )
    {
        BLUENRG2__st8Queue.bValid = true;

        BLUENRG2__st8Queue.u8Idx = 0U;
    }

    if( BLUENRG2__st8Queue.bValid )
    {
        int32_t i32TempRssi = 0;

        int8_t i8MinValue = INT8_MAX;
        int8_t i8MaxValue = INT8_MIN;
        for( uint8_t u8Idx = 0U; u8Idx < BLUENRG2_Q_SZ; u8Idx++ )
        {
            int8_t i8Value = BLUENRG2__st8Queue.i8Buffer[u8Idx];

            if( i8Value > i8MaxValue )
            {
                i8MaxValue = i8Value;
            }

            if( i8Value < i8MinValue )
            {
                i8MinValue = i8Value;
            }
            i32TempRssi += i8Value;
        }

        i32TempRssi -= i8MinValue;
        i32TempRssi -= i8MaxValue;

        i8ReturnValue = (int8_t) ( i32TempRssi / (int32_t) ( BLUENRG2_Q_SZ - 2 ) );
    }

    return i8ReturnValue;
}

static bool BLUENRG2__bIsProcessedRSSIValid( void )
{
    return BLUENRG2__st8Queue.bValid;
}

//*******************************************************************************
//                       BlueNRG-1 Stack Callbacks
//*******************************************************************************

//*****************************************************
//! \brief  This event indicates the end of a connection procedure.
//!
//*****************************************************
void hci_le_connection_complete_event( uint8_t  Status,
                                       uint16_t Connection_Handle,
                                       uint8_t  Role,
                                       uint8_t  Peer_Address_Type,
                                       uint8_t  Peer_Address[6],
                                       uint16_t Conn_Interval,
                                       uint16_t Conn_Latency,
                                       uint16_t Supervision_Timeout,
                                       uint8_t  Master_Clock_Accuracy )

{
    BLUENRG2__stConnCTX.u16Handle = Connection_Handle;
    BLUENRG2__stConnCTX.u8Role    = Role;

    PRINT_DBG( "Connection Complete with peer address: " );
    for( int8_t i8Idx = BLUENRG2_BDADDR_BYTE_NUM - 1; i8Idx > 0; i8Idx-- )
    {
        PRINT_DBG( "%02X-", Peer_Address[i8Idx] );
    }
    PRINT_DBG( "%02X\r\n", Peer_Address[0] );

    if( BLUENRG2__stConnCTX.u8Role == BLUENRG2_MASTER_ROLE )
    {
        BLUENRG2__enState = BLUENRG2_STAT_GET_REMOTE_TX;
    }
    else
    {
        BLUENRG2__enState = BLUENRG2_STAT_IDLE;
    }
}

//*****************************************************
//! \brief  This event indicates the discconnection from a peer device.
//!
//*****************************************************
void hci_disconnection_complete_event( uint8_t Status, uint16_t Connection_Handle, uint8_t Reason )
{

    PRINT_DBG( "Disconnection with reason: 0x%02X\r\n", Reason );
    BLUENRG2__enState = BLUENRG2_STAT_INIT;
}

//*****************************************************
//! \brief  An advertising report is received.
//!
//*****************************************************
void hci_le_advertising_report_event( uint8_t Num_Reports, Advertising_Report_t Advertising_Report[] )
{
    /* Advertising_Report contains all the expected parameters */
    uint8_t u8DataLen = Advertising_Report[0].Length_Data;

    if( !BLUENRG2__stConnCTX.stDevice.bValid )
    {
        if( ( ADV_IND == Advertising_Report[0].Event_Type ) &&
            BLUENRG2__bFindDeviceName( u8DataLen, Advertising_Report[0].Data ) )
        {
            BLUENRG2__vUpdateLockStatus( Advertising_Report[0].RSSI, BLUENRG2_RSSI_THRESHOLD_NEAR, false );

            if( BLUENRG2__bMasterDevIsUnlocked )
            {
                BLUENRG2__stConnCTX.stDevice.bValid = true;
                BLUENRG2__stConnCTX.stDevice.u8Type = Advertising_Report[0].Address_Type;
                BLUENRG_memcpy( BLUENRG2__stConnCTX.stDevice.au8Addr, Advertising_Report[0].Address,
                                sizeof( Advertising_Report[0].Address ) );
                PRINT_DBG( "Device found\r\n" );
                BLUENRG2__enState = BLUENRG2_STAT_SCAN_DONE;
            }
        }
    }
}

//*****************************************************
//! \brief  This event indicates the end of a GAP procedure.
//!
//*****************************************************
void aci_gap_proc_complete_event( uint8_t Procedure_Code, uint8_t Status, uint8_t u8DataLen, uint8_t Data[] )
{
    if( Procedure_Code == GAP_GENERAL_DISCOVERY_PROC )
    {
        /* gap procedure complete has been raised as consequence of a GAP
           terminate procedure done after a device found event during the discovery procedure */
        if( BLUENRG2__stConnCTX.stDevice.bValid && !BLUENRG2__stConnCTX.bHasConnection )
        {
            BLUENRG2__enState = BLUENRG2_STAT_START_CONNECT;
        }
        else
        {
            // discovery procedure has been completed and no device found:
            BLUENRG2__enState = BLUENRG2_STAT_INIT;
        }
    }
}

//*****************************************************
//! \brief  Attribute modified from a peer device
//!
//*****************************************************
// clang-format off
void aci_gatt_attribute_modified_event( uint16_t Connection_Handle, 
                                        uint16_t Attr_Handle, 
                                        uint16_t Offset,
                                        uint16_t Attr_Data_Length,
                                        uint8_t Attr_Data[] )
// clang-format on
{
    BLUENRG2__vAttributeModifiedCB( Attr_Handle, Attr_Data_Length, Attr_Data );
}

//*****************************************************
//! \brief  Notification received from a peer device
//!
//*****************************************************
void aci_gatt_notification_event( uint16_t Connection_Handle,
                                  uint16_t Attribute_Handle,
                                  uint8_t  Attribute_Value_Length,
                                  uint8_t  Attribute_Value[] )
{
    if( Attribute_Handle == BLUENRG2__stConnCTX.RemoteTx.u16Handle + BLUENRG2_CHAR_VALUE_OFFSET )
    {
        BLUENRG2__vReceiveData( Attribute_Value, Attribute_Value_Length );
    }
}

//*****************************************************
//! \brief  Read characteristic by UUID from a peer device.
//!
//*****************************************************
void aci_gatt_disc_read_char_by_uuid_resp_event( uint16_t Connection_Handle,
                                                 uint16_t Attribute_Handle,
                                                 uint8_t  Attribute_Value_Length,
                                                 uint8_t  Attribute_Value[] )
{
    PRINT_DBG( "aci_gatt_disc_read_char_by_uuid_resp_event, Connection Handle: 0x%04X\r\n", Connection_Handle );

    if( !BLUENRG2__stConnCTX.RemoteTx.bValid )
    {
        BLUENRG2__stConnCTX.RemoteTx.u16Handle = Attribute_Handle;
        PRINT_DBG( "Remote TX Char Handle 0x%04X\r\n", BLUENRG2__stConnCTX.RemoteTx.u16Handle );
        BLUENRG2__stConnCTX.RemoteTx.bValid = true;
    }
    else
    {
        if( !BLUENRG2__stConnCTX.RemoteRx.bValid )
        {
            BLUENRG2__stConnCTX.RemoteRx.u16Handle = Attribute_Handle;
            PRINT_DBG( "Remote RX Char Handle 0x%04X\r\n", BLUENRG2__stConnCTX.RemoteRx.u16Handle );
            BLUENRG2__stConnCTX.RemoteRx.bValid = true;
        }
    }
}

//*****************************************************
//! \brief  GATT procedure complete event.
//!
//*****************************************************
void aci_gatt_proc_complete_event( uint16_t Connection_Handle, uint8_t Error_Code )
{
    if( BLUENRG2__stConnCTX.RemoteTx.bValid )
    {
        if( !BLUENRG2__stConnCTX.RemoteRx.bValid )
        {
            BLUENRG2__enState = BLUENRG2_STAT_GET_REMOTE_RX;
        }
        else
        {
            if( !BLUENRG2__bRemoteTxNotifyEnabled )
            {
                BLUENRG2__enState = BLUENRG2_STAT_FORCE_REMOTE_TX_NOTIFY;
            }
        }
    }
}

//*****************************************************
//! \brief  GATT TX pool available event
//!
//*****************************************************
void aci_gatt_tx_pool_available_event( uint16_t Connection_Handle, uint16_t Available_Buffers )
{
    BLUENRG2__bTXbufferFull = false;
}

//*****************************************************
//! \brief  GATT ATT exchange MTU response event.
//!
//*****************************************************
void aci_att_exchange_mtu_resp_event( uint16_t Connection_Handle, uint16_t Server_RX_MTU )
{
    PRINT_DBG( "aci_att_exchange_mtu_resp_event: Server_RX_MTU=%d\r\n", Server_RX_MTU );

    if( Server_RX_MTU <= CLIENT_MAX_MTU_SIZE )
    {
        write_char_len = Server_RX_MTU - 3;
    }
    else
    {
        write_char_len = CLIENT_MAX_MTU_SIZE - 3;
    }

    if( ( mtu_exchanged_wait == 0 ) || ( ( mtu_exchanged_wait == 1 ) ) )
    {
        /**
         * The aci_att_exchange_mtu_resp_event is received also if the
         * aci_gatt_exchange_config is called by the other peer.
         * Here we manage this case.
         */
        if( mtu_exchanged_wait == 0 )
        {
            mtu_exchanged_wait = 2;
        }
        mtu_exchanged = 1;
    }
}
