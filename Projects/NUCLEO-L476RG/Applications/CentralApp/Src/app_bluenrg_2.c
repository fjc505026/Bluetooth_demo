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

#include "app_state.h"
#include "bluenrg1_aci.h"
#include "bluenrg1_hci_le.h"
#include "bluenrg1_events.h"
#include "hci_tl.h"
#include "gatt_db.h"

#include <stdio.h>


#define BLE_SAMPLE_APP_COMPLETE_LOCAL_NAME_SIZE 18

#define APP_RSSI_THRESHOLD_FAR   (-85)   //~3M
#define APP_RSSI_THRESHOLD_MID   (-75)   //~1M 
#define APP_RSSI_THRESHOLD_NEAR  (-65)   //~0.5M  

#define APP_FAR_LED_TOGGLE_TIMEOUT   ( 2000U )   // 0.25HZ, On/Off cycle for 4 sec
#define APP_MID_LED_TOGGLE_TIMEOUT   ( 50U )   // 0.5HZ,  On/Off cycle for 2 sec
#define APP_NEAR_LED_ON_TIMEOUT      ( 250U )   //  2HZ,   On/Off cycle for 0.5 sec

#define APP_RSSI_READING_PERIOD      ( 2000U )   // Read Rssi Value every 2 sec

typedef enum 
{
  APP_RANGE_NEAR = 0U,
  APP_RANGE_MID,
  APP_RANGE_FAR,
  APP_RANGE_CONNECT,
  APP_RANGE_NONE,
} APP_tenRange;

typedef struct discoveryContext_s
{
  uint8_t check_disc_proc_timer;
  uint8_t check_disc_mode_timer;
  uint8_t is_device_found;
  uint8_t do_connect;
  uint32_t startTime;
  uint8_t device_found_address_type;
  uint8_t device_found_address[6];
  uint16_t device_state;
} discoveryContext_t;

uint8_t user_button_init_state;

static discoveryContext_t discovery;
volatile int app_flags = SET_CONNECTABLE;
volatile uint16_t connection_handle = 0;
extern uint16_t chatServHandle, TXCharHandle, RXCharHandle;

static APP_tenRange APP__enDetectRange = APP_RANGE_NONE;
static uint32_t APP__u32RxDataCnt = 0U;
static uint8_t  APP__u8LEDTurnOn = 0U;

/* UUIDs */
UUID_t UUID_Tx;
UUID_t UUID_Rx;

uint16_t tx_handle, rx_handle;
uint16_t discovery_time     = 0;
uint8_t  device_role        = 0xFF;
uint8_t  mtu_exchanged      = 0;
uint8_t  mtu_exchanged_wait = 0;
uint16_t write_char_len     = CHAR_VALUE_LENGTH-3;
uint8_t  data[CHAR_VALUE_LENGTH-3];
uint8_t  counter            = 0;
uint8_t  local_name[] = {AD_TYPE_COMPLETE_LOCAL_NAME,'B','L','E','e','N','R','G','_','S','a','m','p','l','e','A','p','p'};

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void User_Process(void);
static void User_Init(void);
static uint8_t CentralAppInit(void);
static void Reset_DiscoveryContext(void);
static void Setup_DeviceAddress(void);
static void Connection_StateMachine(void);
static uint8_t Find_DeviceName(uint8_t data_length, uint8_t *data_value);
static void Attribute_Modified_CB(uint16_t handle, uint8_t data_length, uint8_t *att_data);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

#if PRINT_CSV_FORMAT
extern volatile uint32_t ms_counter;
/**
 * @brief  This function is a utility to print the log time
 *         in the format HH:MM:SS:MSS (ST BlueNRG GUI time format)
 * @param  None
 * @retval None
 */
void print_csv_time(void){
  uint32_t ms = HAL_GetTick();
  PRINT_CSV("%02ld:%02ld:%02ld.%03ld", (long)(ms/(60*60*1000)%24), (long)(ms/(60*1000)%60), (long)((ms/1000)%60), (long)(ms%1000));
}
#endif

void MX_BlueNRG_2_Init(void)
{
  uint8_t ret;

  User_Init();

  hci_init(APP_UserEvtRx, NULL);

  PRINT_DBG("BlueNRG-2 BLE Sample Application\r\n");


  ret = CentralAppInit();
  if (ret != BLE_STATUS_SUCCESS)
  {
    PRINT_DBG("CentralAppInit()--> Failed 0x%02x\r\n", ret);
    while(1);
  }

  PRINT_DBG("BLE Stack Initialized & Device Configured\r\n");

}

/*
 * BlueNRG-2 background task
 */
void MX_BlueNRG_2_Process(void)
{

  hci_user_evt_proc();
  User_Process();
}

/**
 * @brief  This function is used to send data related to the sample service
 *         (to be sent over the air to the remote board).
 * @param  data_buffer : pointer to data to be sent
 * @param  Nb_bytes : number of bytes to send
 * @retval None
 */
static void sendData(uint8_t* data_buffer, uint8_t Nb_bytes)
{
  uint32_t tickstart = HAL_GetTick();

  if(device_role == SLAVE_ROLE)
  {
    while(aci_gatt_update_char_value_ext(connection_handle,
                                         sampleServHandle,
                                         TXCharHandle,
                                         1, Nb_bytes, 0 ,Nb_bytes, data_buffer) == BLE_STATUS_INSUFFICIENT_RESOURCES)
    {
      APP_FLAG_SET(TX_BUFFER_FULL);
      while(APP_FLAG(TX_BUFFER_FULL)) {
        hci_user_evt_proc();
        // Radio is busy (buffer full).
        if ((HAL_GetTick() - tickstart) > (10*HCI_DEFAULT_TIMEOUT_MS)) break;
      }
    }
  }
  else
  {
    while(aci_gatt_write_without_resp(connection_handle, rx_handle+1, Nb_bytes, data_buffer)==BLE_STATUS_NOT_ALLOWED)
    {
      hci_user_evt_proc();
      // Radio is busy (buffer full).
      if ((HAL_GetTick() - tickstart) > (10*HCI_DEFAULT_TIMEOUT_MS)) break;
    }
  }
}

/**
 * @brief  This function is used to receive data related to the sample service
 *         (received over the air from the remote board).
 * @param  data_buffer : pointer to store in received data
 * @param  Nb_bytes : number of bytes to be received
 * @retval None
 */
static void receiveData(uint8_t* data_buffer, uint8_t Nb_bytes)
{

  // PRINT_DBG("[RX]:");
  // for(int i = 0; i < Nb_bytes; i++)
  // {
  //   PRINT_DBG("%d", data_buffer[i]);
  // }
  // fflush(stdout);

  if( 1U == data_buffer[0])
  {
    APP__u32RxDataCnt++;
    PRINT_DBG("[RX] Notified\r\n");
  }

}

/*******************************************************************************
* Function Name  : Reset_DiscoveryContext.
* Description    : Reset the discovery context.
* Input          : None.
* Return         : None.
*******************************************************************************/
static void Reset_DiscoveryContext(void)
{
  discovery.check_disc_proc_timer = FALSE;
  discovery.check_disc_mode_timer = FALSE;
  discovery.is_device_found = FALSE;
  discovery.do_connect = FALSE;
  discovery.startTime = 0;
  discovery.device_state = INIT_STATE;
  BLUENRG_memset(&discovery.device_found_address[0], 0, 6);
  device_role = 0xFF;
  mtu_exchanged = 0;
  mtu_exchanged_wait = 0;
  write_char_len = CHAR_VALUE_LENGTH-3;

  for (uint16_t i=0; i<(CHAR_VALUE_LENGTH-3); i++) {
    data[i] = 0x31 + (i%10);
    if ((i+1)%10==0) {
      data[i]='x';
    }
  }
}

/*******************************************************************************
* Function Name  : Setup_DeviceAddress.
* Description    : Setup the device address.
* Input          : None.
* Return         : None.
*******************************************************************************/
static void Setup_DeviceAddress(void)
{
  tBleStatus ret;
  uint8_t bdaddr[] = {0x00, 0x00, 0x00, 0xE1, 0x80, 0x02};
  uint8_t random_number[8];

  /* get a random number from BlueNRG */
  ret = hci_le_rand(random_number);
  if(ret != BLE_STATUS_SUCCESS)
  {
    PRINT_DBG("hci_le_rand() call failed: 0x%02x\r\n", ret);
  }

  discovery_time = 3000; /* at least 3 seconds */
  /* setup discovery time with random number */
  for (uint8_t i=0; i<8; i++)
  {
    discovery_time += (2*random_number[i]);
  }

  /* Setup last 3 bytes of public address with random number */
  bdaddr[0] = (uint8_t) (random_number[0]);
  bdaddr[1] = (uint8_t) (random_number[3]);
  bdaddr[2] = (uint8_t) (random_number[6]);

  ret = aci_hal_write_config_data(CONFIG_DATA_PUBADDR_OFFSET, CONFIG_DATA_PUBADDR_LEN, bdaddr);
  if(ret != BLE_STATUS_SUCCESS)
  {
      PRINT_DBG("Setting BD_ADDR failed 0x%02x\r\n", ret);
  }
  else
  {
    PRINT_DBG("Public address: ");
    for (uint8_t i=5; i>0; i--)
    {
      PRINT_DBG("%02X-", bdaddr[i]);
    }
    PRINT_DBG("%02X\r\n", bdaddr[0]);
  }

}

/*******************************************************************************
* Function Name  : Find_DeviceName.
* Description    : Extracts the device name.
* Input          : Data length.
*                  Data value
* Return         : TRUE if the local name found is the expected one, FALSE otherwise.
*******************************************************************************/
static uint8_t Find_DeviceName(uint8_t data_length, uint8_t *data_value)
{
  uint8_t index = 0;

  while (index < data_length)
  {
    /* Advertising data fields: len, type, values */
    /* Check if field is complete local name and the length is the expected one for BLE CentralApp  */
    if (data_value[index+1] == AD_TYPE_COMPLETE_LOCAL_NAME)
    {
      data_value[index+10] = '\0';
      PRINT_DBG("[Device name] %s\r\n", &data_value[index+1]);
      /* check if found device name is the expected one: local_name */
      if (BLUENRG_memcmp(&data_value[index+1], &local_name[0], 3) == 0)
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
      /* move to next advertising field */
      index += (data_value[index] +1);
    }
  }

  return FALSE;
}

/*******************************************************************************
* Function Name  : Attribute_Modified_CB
* Description    : Attribute modified callback.
* Input          : Attribute handle modified.
*                  Length of the data.
*                  Attribute data.
* Return         : None.
*******************************************************************************/
static void Attribute_Modified_CB(uint16_t handle, uint8_t data_length, uint8_t *att_data)
{
  if(handle == RXCharHandle + 1)
  {
    receiveData(att_data, data_length);
  }
  else if(handle == TXCharHandle + 2)
  {
    if(att_data[0] == 0x01)
    {
      APP_FLAG_SET(NOTIFICATIONS_ENABLED);
    }
  }
}

/*******************************************************************************
* Function Name  : CentralAppInit.
* Description    : Init CentralApp.
* Input          : None.
* Return         : Status.
*******************************************************************************/
static uint8_t CentralAppInit(void)
{
  uint8_t ret;
  uint16_t service_handle, dev_name_char_handle, appearance_char_handle;
  const uint8_t cu8DeviceNameLen = 7U;

  /* Sw reset of the device */
  hci_reset();
  /**
   *  To support both the BlueNRG-2 and the BlueNRG-2N a minimum delay of 2000ms is required at device boot
   */
  HAL_Delay(2000);

  /* Setup the device address */
  Setup_DeviceAddress();

  /* Set the TX power to -2 dBm */
  aci_hal_set_tx_power_level(1, 4);

  /* GATT Init */
  ret = aci_gatt_init();
  if(ret != BLE_STATUS_SUCCESS)
  {
    PRINT_DBG("GATT_Init failed: 0x%02x\r\n", ret);
    return ret;
  }

  /* GAP Init */
  ret = aci_gap_init(GAP_CENTRAL_ROLE,0x0,cu8DeviceNameLen, &service_handle,
                     &dev_name_char_handle, &appearance_char_handle);
  if(ret != BLE_STATUS_SUCCESS)
  {
    PRINT_DBG("GAP_Init failed: 0x%02x\r\n", ret);
    return ret;
  }

  /* Add Device Service & Characteristics */
  ret = Add_Sample_Service();
  if(ret != BLE_STATUS_SUCCESS)
  {
    PRINT_DBG("Error while adding service: 0x%02x\r\n", ret);
    return ret;
  }

  /* Reset the discovery context */
  Reset_DiscoveryContext();

  return BLE_STATUS_SUCCESS;
}

/**
 * @brief  Initialize User process.
 *
 * @param  None
 * @retval None
 */
static void User_Init(void)
{
  BSP_PB_Init(BUTTON_KEY, BUTTON_MODE_GPIO);
  BSP_LED_Init(LED2);

  BSP_COM_Init(COM1);
}

/*******************************************************************************
* Function Name  : Connection_StateMachine.
* Description    : Connection state machine.
* Input          : None.
* Return         : None.
*******************************************************************************/
static void Connection_StateMachine(void)
{
  uint8_t ret;

  switch (discovery.device_state)
  {
  case (INIT_STATE):
    {
      Reset_DiscoveryContext();
      discovery.device_state = START_DISCOVERY_PROC;
    }
    break; /* end case (INIT_STATE) */
  case (START_DISCOVERY_PROC):
    {

      ret = aci_gap_start_general_discovery_proc(SCAN_P, SCAN_L, PUBLIC_ADDR, 0x00);
      if (ret != BLE_STATUS_SUCCESS)
      {
        PRINT_DBG("aci_gap_start_general_discovery_proc() failed: %02X\r\n",ret);
        discovery.device_state = DISCOVERY_ERROR;
      }
      else
      {
        PRINT_DBG("aci_gap_start_general_discovery_proc OK\r\n");
        discovery.startTime = HAL_GetTick();
        discovery.check_disc_proc_timer = TRUE;
        discovery.check_disc_mode_timer = FALSE;
        discovery.device_state = WAIT_TIMER_EXPIRED;
      }
    }
    break;/* end case (START_DISCOVERY_PROC) */
  case (WAIT_TIMER_EXPIRED):
    {
      /* Verify if startTime check has to be done  since discovery procedure is ongoing */
      if (discovery.check_disc_proc_timer == TRUE)
      {
        /* check startTime value */
        if (HAL_GetTick() - discovery.startTime > discovery_time)
        {
          discovery.check_disc_proc_timer = FALSE;
          discovery.startTime = 0;
          discovery.device_state = DO_TERMINATE_GAP_PROC;
        }/* if (HAL_GetTick() - discovery.startTime > discovery_time) */
      }
      /* Verify if startTime check has to be done  since discovery mode is ongoing */
      else if (discovery.check_disc_mode_timer == TRUE)
      {
        /* check startTime value */
        if (HAL_GetTick() - discovery.startTime > discovery_time)
        {
          discovery.check_disc_mode_timer = FALSE;
          discovery.startTime = 0;

          /* Discovery mode is ongoing: set non discoverable mode */
          discovery.device_state = DO_NON_DISCOVERABLE_MODE;

        }/* else if (discovery.check_disc_mode_timer == TRUE) */
      }/* if ((discovery.check_disc_proc_timer == TRUE) */
    }
    break; /* end case (WAIT_TIMER_EXPIRED) */
  case (DO_NON_DISCOVERABLE_MODE):
    {
      ret = aci_gap_set_non_discoverable();
      if (ret != BLE_STATUS_SUCCESS)
      {
        PRINT_DBG("aci_gap_set_non_discoverable() failed: 0x%02x\r\n", ret);
        discovery.device_state = DISCOVERY_ERROR;
      }
      else
      {
        PRINT_DBG("aci_gap_set_non_discoverable() OK\r\n");
        /* Restart Central discovery procedure */
        discovery.device_state = INIT_STATE;
      }
    }
    break; /* end case (DO_NON_DISCOVERABLE_MODE) */
  case (DO_TERMINATE_GAP_PROC):
    {
      /* terminate gap procedure */
      ret = aci_gap_terminate_gap_proc(GAP_GENERAL_DISCOVERY_PROC); // GENERAL_DISCOVERY_PROCEDURE
      if (ret != BLE_STATUS_SUCCESS)
      {
        PRINT_DBG("aci_gap_terminate_gap_procedure() failed: 0x%02x\r\n", ret);
        discovery.device_state = DISCOVERY_ERROR;
        break;
      }
      else
      {
        PRINT_DBG("aci_gap_terminate_gap_procedure() OK\r\n");
        discovery.device_state = WAIT_EVENT; /* wait for GAP procedure complete */
      }
    }
    break; /* end case (DO_TERMINATE_GAP_PROC) */
  case (DO_DIRECT_CONNECTION_PROC):
    {
      PRINT_DBG("Device Found with address: ");
      for (uint8_t i=5; i>0; i--)
      {
        PRINT_DBG("%02X-", discovery.device_found_address[i]);
      }
      PRINT_DBG("%02X\r\n", discovery.device_found_address[0]);
      /* Do connection with first discovered device */
      ret = aci_gap_create_connection(SCAN_P, SCAN_L,
                                      discovery.device_found_address_type, discovery.device_found_address,
                                      PUBLIC_ADDR, 40, 40, 0, 60, 2000 , 2000);
      if (ret != BLE_STATUS_SUCCESS)
      {
        PRINT_DBG("aci_gap_create_connection() failed: 0x%02x\r\n", ret);
        discovery.device_state = DISCOVERY_ERROR;
      }
      else
      {
        PRINT_DBG("aci_gap_create_connection() OK\r\n");
        discovery.device_state = WAIT_EVENT;
      }
    }
    break; /* end case (DO_DIRECT_CONNECTION_PROC) */
  case (WAIT_EVENT):
    {
      discovery.device_state = WAIT_EVENT;
    }
    break; /* end case (WAIT_EVENT) */
  case (ENTER_DISCOVERY_MODE):
    {

      /* Put Peripheral device in discoverable mode */

      /* disable scan response */
      hci_le_set_scan_response_data(0,NULL);

      ret = aci_gap_set_discoverable(ADV_DATA_TYPE, ADV_INTERV_MIN, ADV_INTERV_MAX, PUBLIC_ADDR,
                                     NO_WHITE_LIST_USE, sizeof(local_name), local_name, 0, NULL,
                                     0x0, 0x0);
      if (ret != BLE_STATUS_SUCCESS)
      {
        PRINT_DBG("aci_gap_set_discoverable() failed: 0x%02x\r\n", ret);
        discovery.device_state = DISCOVERY_ERROR;
      }
      else
      {
        PRINT_DBG("aci_gap_set_discoverable() OK\r\n");
        discovery.startTime = HAL_GetTick();
        discovery.check_disc_mode_timer = TRUE;
        discovery.check_disc_proc_timer = FALSE;
        discovery.device_state = WAIT_TIMER_EXPIRED;
      }
    }
    break; /* end case (ENTER_DISCOVERY_MODE) */
  case (DISCOVERY_ERROR):
    {
      Reset_DiscoveryContext();
    }
    break; /* end case (DISCOVERY_ERROR) */
  default:
    break;
  }/* end switch */

}/* end Connection_StateMachine() */


static void APP__vUpdateDetectRange( int8_t i8Rssi )
{
  static APP_tenRange enLastRange = APP_RANGE_NONE;
  
  if ( i8Rssi == (int8_t)127 )
  {
    APP__enDetectRange = APP_RANGE_NONE;
  }
  else if ( i8Rssi  > APP_RSSI_THRESHOLD_NEAR) // 0-0.5M
  {
     APP__enDetectRange = APP_RANGE_NEAR;
  }
  else if ( i8Rssi  > APP_RSSI_THRESHOLD_MID) // 0.5M-1M
  {
    APP__enDetectRange = APP_RANGE_MID;
  }
  else if ( i8Rssi  > APP_RSSI_THRESHOLD_FAR) // 1M-3M
  {
      APP__enDetectRange = APP_RANGE_FAR;
  }
  else  // more than 3M
  {
    APP__enDetectRange = APP_RANGE_CONNECT;
  }

  if ( enLastRange != APP__enDetectRange)
  {
    const char * apccRangeStr[] = {"NEAR", "MID", "FAR", "CONNECTABLE","NONE"};
    PRINT_DBG("[RANGE] %s -> %s \r\n",apccRangeStr[ (uint8_t)enLastRange], apccRangeStr[ (uint8_t)APP__enDetectRange]);
    enLastRange = APP__enDetectRange;
  }


}

static void APP__vLEDHanlder( APP_tenRange enRange )
{
  static uint32_t  u32LastTick = 0U;
  static uint32_t  u32LastRxDataCnt = 0U; 

  switch (enRange)
  {

    case APP_RANGE_CONNECT:
    {
      // if( HAL_GetTick() - u32LastTick > 3*APP_FAR_LED_TOGGLE_TIMEOUT )
      // {
      //   BSP_LED_Toggle(LED2);
      //   u32LastTick = HAL_GetTick();
      // }
      BSP_LED_Off(LED2);
    } break;
    case APP_RANGE_FAR:
    {
      BSP_LED_Off(LED2);
      // if( HAL_GetTick() - u32LastTick > APP_FAR_LED_TOGGLE_TIMEOUT )
      // {
      //   BSP_LED_Toggle(LED2);
      //   u32LastTick = HAL_GetTick();
      // }
    } break;

    case APP_RANGE_MID:
    {
      if( HAL_GetTick() - u32LastTick > APP_MID_LED_TOGGLE_TIMEOUT )
      {
        BSP_LED_Toggle(LED2);
        u32LastTick = HAL_GetTick();
      }
    } break;

    case APP_RANGE_NEAR:
    {
      if ( APP__u32RxDataCnt != u32LastRxDataCnt )   // RX Counter has been changed
      {
        u32LastRxDataCnt = APP__u32RxDataCnt;
        APP__u8LEDTurnOn = 1U;
        BSP_LED_On(LED2);
        u32LastTick = HAL_GetTick();
      }

      if( HAL_GetTick() - u32LastTick > APP_NEAR_LED_ON_TIMEOUT )
      {
        BSP_LED_Off(LED2);
      }

    } break;
    case APP_RANGE_NONE:
    default:
    {
      BSP_LED_Off(LED2);
    } break;
  }
}

/**
 * @brief  Run the state machine.
 *
 * @param  None
 * @retval None
 */
static void User_Process(void)
{
  static uint32_t u32LastRssiReadTick = 0U;

  if(APP_FLAG(SET_CONNECTABLE))
  {
    Connection_StateMachine();
    user_button_init_state = BSP_PB_GetState(BUTTON_KEY);
  }

  if (device_role == MASTER_ROLE)
  {
    /* Start TX handle Characteristic discovery if not yet done */
    if (APP_FLAG(CONNECTED) && !APP_FLAG(END_READ_TX_CHAR_HANDLE))
    {
      if (!APP_FLAG(START_READ_TX_CHAR_HANDLE))
      {
        /* Discovery TX characteristic handle by UUID 128 bits */
        const uint8_t charUuid128_TX[16] = {0x66,0x9a,0x0c,0x20,0x00,0x08,0x96,0x9e,0xe2,0x11,0x9e,0xb1,0xe1,0xf2,0x73,0xd9};

        BLUENRG_memcpy(&UUID_Tx.UUID_16, charUuid128_TX, 16);
        aci_gatt_disc_char_by_uuid(connection_handle, 0x0001, 0xFFFF,UUID_TYPE_128,&UUID_Tx);
        APP_FLAG_SET(START_READ_TX_CHAR_HANDLE);
      }
    }
    /* Start RX handle Characteristic discovery if not yet done */
    else if (APP_FLAG(CONNECTED) && !APP_FLAG(END_READ_RX_CHAR_HANDLE))
    {
      /* Discovery RX characteristic handle by UUID 128 bits */
      if (!APP_FLAG(START_READ_RX_CHAR_HANDLE))
      {
        /* Discovery RX characteristic handle by UUID 128 bits */
        const uint8_t charUuid128_RX[16] = {0x66,0x9a,0x0c,0x20,0x00,0x08,0x96,0x9e,0xe2,0x11,0x9e,0xb1,0xe2,0xf2,0x73,0xd9};

        BLUENRG_memcpy(&UUID_Rx.UUID_16, charUuid128_RX, 16);
        aci_gatt_disc_char_by_uuid(connection_handle, 0x0001, 0xFFFF,UUID_TYPE_128,&UUID_Rx);
        APP_FLAG_SET(START_READ_RX_CHAR_HANDLE);
      }
    }

    if(APP_FLAG(CONNECTED) && APP_FLAG(END_READ_TX_CHAR_HANDLE) && APP_FLAG(END_READ_RX_CHAR_HANDLE) && !APP_FLAG(NOTIFICATIONS_ENABLED))
    {
      //   /* Before enabling notifications perform an ATT MTU exchange procedure */
      //   if ((mtu_exchanged == 0) && (mtu_exchanged_wait == 0))
      //   {
      //     PRINT_DBG("ROLE MASTER (mtu_exchanged %d, mtu_exchanged_wait %d)\r\n",
      //               mtu_exchanged, mtu_exchanged_wait);

      //     uint8_t ret = aci_gatt_exchange_config(connection_handle);
      //     if (ret != BLE_STATUS_SUCCESS) {
      //       PRINT_DBG("aci_gatt_exchange_configuration error 0x%02x\r\n", ret);
      //     }
      //     mtu_exchanged_wait = 1;
      //   }
      //   else if ((mtu_exchanged == 1) && (mtu_exchanged_wait == 2))
      //   {
      //     uint8_t client_char_conf_data[] = {0x01, 0x00}; // Enable notifications
      //     uint32_t tickstart = HAL_GetTick();

      //     while(aci_gatt_write_char_desc(connection_handle, tx_handle+2, 2, client_char_conf_data)==BLE_STATUS_NOT_ALLOWED)
      //     {
      //       // Radio is busy.
      //       if ((HAL_GetTick() - tickstart) > (10*HCI_DEFAULT_TIMEOUT_MS)) break;
      //     }
      //     APP_FLAG_SET(NOTIFICATIONS_ENABLED);
      // }
        uint8_t client_char_conf_data[] = {0x01, 0x00}; // Enable notifications
        uint32_t tickstart = HAL_GetTick();

        while(aci_gatt_write_char_desc(connection_handle, tx_handle+2, 2, client_char_conf_data)==BLE_STATUS_NOT_ALLOWED)
        {
          // Radio is busy.
          if ((HAL_GetTick() - tickstart) > (10*HCI_DEFAULT_TIMEOUT_MS)) break;
        }
        APP_FLAG_SET(NOTIFICATIONS_ENABLED);
    }
  } /* if (device_role == MASTER_ROLE) */


  if( APP_FLAG(CONNECTED) && APP_FLAG(NOTIFICATIONS_ENABLED) )
  {
    if ( HAL_GetTick() -  u32LastRssiReadTick > APP_RSSI_READING_PERIOD )
    {
      int8_t i8tempRssi;
      hci_read_rssi(connection_handle, &i8tempRssi);
      APP__vUpdateDetectRange(i8tempRssi);
      PRINT_DBG("[RSSI] %d dBm\r\n",i8tempRssi);
      u32LastRssiReadTick = HAL_GetTick();
    }
  
  }

  APP__vLEDHanlder( APP__enDetectRange );
}


/* ***************** BlueNRG-1 Stack Callbacks ********************************/

/*******************************************************************************
 * Function Name  : aci_gap_proc_complete_event.
 * Description    : This event indicates the end of a GAP procedure.
 * Input          : See file bluenrg1_events.h
 * Output         : See file bluenrg1_events.h
 * Return         : See file bluenrg1_events.h
 *******************************************************************************/
void aci_gap_proc_complete_event(uint8_t Procedure_Code,
                                 uint8_t Status,
                                 uint8_t Data_Length,
                                 uint8_t Data[])
{
  if (Procedure_Code == GAP_GENERAL_DISCOVERY_PROC)
  {
    /* gap procedure complete has been raised as consequence of a GAP
       terminate procedure done after a device found event during the discovery procedure */
    if (discovery.do_connect == TRUE)
    {
      discovery.do_connect = FALSE;
      discovery.check_disc_proc_timer = FALSE;
      discovery.startTime = 0;
      /* discovery procedure has been completed and no device found:
         go to discovery mode */
      discovery.device_state = DO_DIRECT_CONNECTION_PROC;
    }
    else
    {
      /* discovery procedure has been completed and no device found:
         go to discovery mode */
      discovery.check_disc_proc_timer = FALSE;
      discovery.startTime = 0;
      discovery.device_state = INIT_STATE;
    }
  }
}

/*******************************************************************************
 * Function Name  : hci_le_connection_complete_event.
 * Description    : This event indicates the end of a connection procedure.
 * Input          : See file bluenrg1_events.h
 * Output         : See file bluenrg1_events.h
 * Return         : See file bluenrg1_events.h
 *******************************************************************************/
void hci_le_connection_complete_event(uint8_t Status,
                                      uint16_t Connection_Handle,
                                      uint8_t Role,
                                      uint8_t Peer_Address_Type,
                                      uint8_t Peer_Address[6],
                                      uint16_t Conn_Interval,
                                      uint16_t Conn_Latency,
                                      uint16_t Supervision_Timeout,
                                      uint8_t Master_Clock_Accuracy)

{
  /* Set the exit state for the Connection state machine: APP_FLAG_CLEAR(SET_CONNECTABLE); */
  APP_FLAG_CLEAR(SET_CONNECTABLE);
  discovery.check_disc_proc_timer = FALSE;
  discovery.check_disc_mode_timer = FALSE;
  discovery.startTime = 0;

  connection_handle = Connection_Handle;

  APP_FLAG_SET(CONNECTED);
  discovery.device_state = INIT_STATE;

  /* store device role */
  device_role = Role;

  PRINT_DBG("Connection Complete with peer address: ");
  for (uint8_t i=5; i>0; i--)
  {
    PRINT_DBG("%02X-", Peer_Address[i]);
  }
  PRINT_DBG("%02X\r\n", Peer_Address[0]);

}/* end hci_le_connection_complete_event() */

/*******************************************************************************
 * Function Name  : hci_disconnection_complete_event.
 * Description    : This event indicates the discconnection from a peer device.
 * Input          : See file bluenrg1_events.h
 * Output         : See file bluenrg1_events.h
 * Return         : See file bluenrg1_events.h
 *******************************************************************************/
void hci_disconnection_complete_event(uint8_t Status,
                                      uint16_t Connection_Handle,
                                      uint8_t Reason)
{
  APP_FLAG_CLEAR(CONNECTED);
  /* Make the device connectable again. */
  APP_FLAG_SET(SET_CONNECTABLE);
  APP_FLAG_CLEAR(NOTIFICATIONS_ENABLED);

  APP_FLAG_CLEAR(START_READ_TX_CHAR_HANDLE);
  APP_FLAG_CLEAR(END_READ_TX_CHAR_HANDLE);
  APP_FLAG_CLEAR(START_READ_RX_CHAR_HANDLE);
  APP_FLAG_CLEAR(END_READ_RX_CHAR_HANDLE);
  APP_FLAG_CLEAR(TX_BUFFER_FULL);

  APP__vUpdateDetectRange(127);

  PRINT_DBG("Disconnection with reason: 0x%02X\r\n", Reason);
  Reset_DiscoveryContext();

}/* end hci_disconnection_complete_event() */


/*******************************************************************************
 * Function Name  : hci_le_advertising_report_event.
 * Description    : An advertising report is received.
 * Input          : See file bluenrg1_events.h
 * Output         : See file bluenrg1_events.h
 * Return         : See file bluenrg1_events.h
 *******************************************************************************/
void hci_le_advertising_report_event(uint8_t Num_Reports,
                                     Advertising_Report_t Advertising_Report[])
{
  /* Advertising_Report contains all the expected parameters */
  uint8_t evt_type = Advertising_Report[0].Event_Type ;
  uint8_t data_length = Advertising_Report[0].Length_Data;
  uint8_t bdaddr_type = Advertising_Report[0].Address_Type;
  uint8_t bdaddr[6];

  BLUENRG_memcpy(bdaddr, Advertising_Report[0].Address,6);

  /* BLE CentralApp device not yet found: check current device found */
  if (!(discovery.is_device_found))
  {
    /* BLE CentralApp device not yet found: check current device found */
    if ((evt_type == ADV_IND) && Find_DeviceName(data_length, Advertising_Report[0].Data))
    {
      int8_t i8Rssi = Advertising_Report[0].RSSI;

      APP__vUpdateDetectRange(i8Rssi);

      if( APP__enDetectRange == APP_RANGE_NEAR )
      {
        discovery.is_device_found = TRUE;
        discovery.do_connect = TRUE;
        discovery.check_disc_proc_timer = FALSE;
        discovery.check_disc_mode_timer = FALSE;
        /* store first device found:  address type and address value */
        discovery.device_found_address_type = bdaddr_type;
        BLUENRG_memcpy(discovery.device_found_address, bdaddr, 6);
        /* device is found: terminate discovery procedure */
        discovery.device_state = DO_TERMINATE_GAP_PROC;
        PRINT_DBG("Device found\r\n");
      }

    }
  }
} /* hci_le_advertising_report_event() */

/*******************************************************************************
 * Function Name  : aci_gatt_attribute_modified_event.
 * Description    : Attribute modified from a peer device.
 * Input          : See file bluenrg1_events.h
 * Output         : See file bluenrg1_events.h
 * Return         : See file bluenrg1_events.h
 *******************************************************************************/
void aci_gatt_attribute_modified_event(uint16_t Connection_Handle,
                                       uint16_t Attr_Handle,
                                       uint16_t Offset,
                                       uint16_t Attr_Data_Length,
                                       uint8_t Attr_Data[])
{
  Attribute_Modified_CB(Attr_Handle, Attr_Data_Length, Attr_Data);
} /* end aci_gatt_attribute_modified_event() */

/*******************************************************************************
 * Function Name  : aci_gatt_notification_event.
 * Description    : Notification received from a peer device.
 * Input          : See file bluenrg1_events.h
 * Output         : See file bluenrg1_events.h
 * Return         : See file bluenrg1_events.h
 *******************************************************************************/
void aci_gatt_notification_event(uint16_t Connection_Handle,
                                 uint16_t Attribute_Handle,
                                 uint8_t Attribute_Value_Length,
                                 uint8_t Attribute_Value[])
{
  if(Attribute_Handle == tx_handle+1)
  {
    receiveData(Attribute_Value, Attribute_Value_Length);
  }
} /* end aci_gatt_notification_event() */

/*******************************************************************************
 * Function Name  : aci_gatt_disc_read_char_by_uuid_resp_event.
 * Description    : Read characteristic by UUID from a peer device.
 * Input          : See file bluenrg1_events.h
 * Output         : See file bluenrg1_events.h
 * Return         : See file bluenrg1_events.h
 *******************************************************************************/
void aci_gatt_disc_read_char_by_uuid_resp_event(uint16_t Connection_Handle,
                                                uint16_t Attribute_Handle,
                                                uint8_t Attribute_Value_Length,
                                                uint8_t Attribute_Value[])
{
  PRINT_DBG("aci_gatt_disc_read_char_by_uuid_resp_event, Connection Handle: 0x%04X\r\n", Connection_Handle);
  if (APP_FLAG(START_READ_TX_CHAR_HANDLE) && !APP_FLAG(END_READ_TX_CHAR_HANDLE))
  {
    tx_handle = Attribute_Handle;
    PRINT_DBG("TX Char Handle 0x%04X\r\n", tx_handle);
  }
  else
  {
    if(APP_FLAG(START_READ_RX_CHAR_HANDLE) && !APP_FLAG(END_READ_RX_CHAR_HANDLE))
    {
      rx_handle = Attribute_Handle;
      PRINT_DBG("RX Char Handle 0x%04X\r\n", rx_handle);
      /**
       * LED blinking on the CENTRAL device to indicate the characteristic
       * discovery process has terminated
       */
      // for (uint8_t i=0; i<9; i++) {
      //   BSP_LED_Toggle(LED2);
      //   HAL_Delay(250);
      // }
    }
  }
} /* end aci_gatt_disc_read_char_by_uuid_resp_event() */

/*******************************************************************************
 * Function Name  : aci_gatt_proc_complete_event.
 * Description    : GATT procedure complete event.
 * Input          : See file bluenrg1_events.h
 * Output         : See file bluenrg1_events.h
 * Return         : See file bluenrg1_events.h
 *******************************************************************************/
void aci_gatt_proc_complete_event(uint16_t Connection_Handle,
                                  uint8_t Error_Code)
{
  if (APP_FLAG(START_READ_TX_CHAR_HANDLE) && !APP_FLAG(END_READ_TX_CHAR_HANDLE))
  {
    APP_FLAG_SET(END_READ_TX_CHAR_HANDLE);
  }
  else
  {
    if (APP_FLAG(START_READ_RX_CHAR_HANDLE) && !APP_FLAG(END_READ_RX_CHAR_HANDLE))
    {
      APP_FLAG_SET(END_READ_RX_CHAR_HANDLE);
    }
  }
} /* end aci_gatt_proc_complete_event() */

/*******************************************************************************
 * Function Name  : aci_gatt_tx_pool_available_event.
 * Description    : GATT TX pool available event.
 * Input          : See file bluenrg1_events.h
 * Output         : See file bluenrg1_events.h
 * Return         : See file bluenrg1_events.h
 *******************************************************************************/
void aci_gatt_tx_pool_available_event(uint16_t Connection_Handle,
                                      uint16_t Available_Buffers)
{
  APP_FLAG_CLEAR(TX_BUFFER_FULL);
} /* end aci_gatt_tx_pool_available_event() */

/*******************************************************************************
 * Function Name  : aci_att_exchange_mtu_resp_event.
 * Description    : GATT ATT exchange MTU response event.
 * Input          : See file bluenrg1_events.h
 * Output         : See file bluenrg1_events.h
 * Return         : See file bluenrg1_events.h
 *******************************************************************************/
void aci_att_exchange_mtu_resp_event(uint16_t Connection_Handle,
                                     uint16_t Server_RX_MTU)
{
  PRINT_DBG("aci_att_exchange_mtu_resp_event: Server_RX_MTU=%d\r\n", Server_RX_MTU);

  if (Server_RX_MTU <= CLIENT_MAX_MTU_SIZE) {
    write_char_len = Server_RX_MTU - 3;
  }
  else {
    write_char_len = CLIENT_MAX_MTU_SIZE - 3;
  }

  if ((mtu_exchanged_wait == 0) || ((mtu_exchanged_wait == 1))) {
    /**
     * The aci_att_exchange_mtu_resp_event is received also if the
     * aci_gatt_exchange_config is called by the other peer.
     * Here we manage this case.
     */
    if (mtu_exchanged_wait == 0) {
      mtu_exchanged_wait = 2;
    }
    mtu_exchanged = 1;
  }
}

