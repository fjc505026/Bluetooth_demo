[#ftl]
#include "app_state.h"
#include "bluenrg1_aci.h"
#include "bluenrg1_hci_le.h"
#include "bluenrg1_events.h"
#include "hci_tl.h"
#include "gatt_db.h"

#include <stdio.h>

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private defines -----------------------------------------------------------*/

#define BLE_SAMPLE_APP_COMPLETE_LOCAL_NAME_SIZE 18

/* Private macros ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* discovery procedure mode context */
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
uint8_t  local_name[] = {AD_TYPE_COMPLETE_LOCAL_NAME,'B','l','u','e','N','R','G','_','S','a','m','p','l','e','A','p','p'};

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void User_Process(void);
static void User_Init(void);
static uint8_t SampleAppInit(void);
static void Reset_DiscoveryContext(void);
static void Setup_DeviceAddress(void);
static void Connection_StateMachine(void);
static uint8_t Find_DeviceName(uint8_t data_length, uint8_t *data_value);
static void Attribute_Modified_CB(uint16_t handle, uint8_t data_length, uint8_t *att_data);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */
