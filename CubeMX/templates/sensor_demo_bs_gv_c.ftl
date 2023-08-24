[#ftl]

#include <stdlib.h>

#include "sensor.h"
#include "bluenrg1_aci.h"
#include "bluenrg1_hci_le.h"
#include "bluenrg1_events.h"
#include "hci_tl.h"
#include "gatt_db.h"
#include "bluenrg_utils.h"
#include "${BoardName}.h"
#ifdef STM32L476xx [#-- #if McuName=="STM32L476RGTx" --]
#include "OTA.h"
#endif /* STM32L476xx */ [#-- /#if --]

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private defines -----------------------------------------------------------*/
/**
 * This define enables a security scenario with PassKey Entry method.
 *
 * It is the BLE Central connected to this BLE Peripheral 
 * that starts security process by sending a Slave Security Request.
 * Once connected with the BLE Central device, this BLE Peripheral starts 
 * ACI_GAP_SLAVE_SECURITY_REQ procedure. The BLE Peripheral sets the key through 
 * ACI_GAP_PASS_KEY_RESP command after the ACI_GAP_PASS_KEY_REQ_EVENT event is 
 * generated. 
 * At this stage, an input window is displayed on the Central device for allowing 
 * user to insert the key set by the Peripheral device (123456 in this example). 
 * After these operations the Pairing is completed and the bonded device is 
 * displayed.
 */
#define SECURE_PAIRING (0)
#define PERIPHERAL_PASS_KEY (123456)

/**
 * 1 to send environmental and motion data when pushing the user button
 * 0 to send environmental and motion data automatically (period = 1 sec)
 */
#define USE_BUTTON (0)
 
/* Private macros ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
extern AxesRaw_t x_axes;
extern AxesRaw_t g_axes;
extern AxesRaw_t m_axes;
extern AxesRaw_t q_axes;

extern __IO uint16_t connection_handle;
extern volatile uint8_t set_connectable;
extern volatile uint8_t connected;
extern volatile uint8_t pairing;
extern volatile uint8_t paired;
uint8_t bdaddr[BDADDR_SIZE];
static volatile uint8_t user_button_init_state = 1;
static volatile uint8_t user_button_pressed = 0;
extern __IO uint8_t send_env;
extern __IO uint8_t send_mot;
extern __IO uint8_t send_quat;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void User_Process(void);
static void User_Init(void);
static uint8_t Sensor_DeviceInit(void);
static void Set_Random_Environmental_Values(float *data_t, float *data_p);
static void Set_Random_Motion_Values(uint32_t cnt);
static void Reset_Motion_Values(void);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */
