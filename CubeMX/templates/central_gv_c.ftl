[#ftl]
#include "bluenrg_conf.h"
#include "central.h"
#include "console.h"

#include "hci.h"
#include "hci_tl.h"
#include "bluenrg1_events.h"
#include "bluenrg1_gap_aci.h"

#include "${BoardName}.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN Defines */

/* USER CODE END Defines */

/* Private variables ---------------------------------------------------------*/
/**
 * @brief Properties mask
 */
static uint8_t props_mask[] = {
  0x01,
  0x02,
  0x04,
  0x08,
  0x10,
  0x20,
  0x40,
  0x80
};

static volatile uint8_t allow_console = FALSE;

extern savedDevices_t   saved_devices;
extern nonConnDevices_t non_conn_devices;
extern serviceInfo_t    serv_info[MAX_NUM_OF_SERVICES];
extern centralStatus_t  central_status;

/* Primary Service UUID expected from peripherals */
uint8_t GENERIC_ACCESS_PROFILE_UUID[]    = {0x00, 0x18};
uint8_t GENERIC_ATTRIBUTE_PROFILE_UUID[] = {0x01, 0x18};

/* ST Custom Services UUID */
uint8_t ST_HARDWARE_SERVICE_UUID[] = {0x1b,0xc5,0xd5,0xa5,0x02,0x00,0xb4,0x9a,0xe1,0x11,0x01,0x00,0x00,0x00,0x00,0x00};
uint8_t ST_CONFIG_SERVICE_UUID[]   = {0x1b,0xc5,0xd5,0xa5,0x02,0x00,0xb4,0x9a,0xe1,0x11,0x0F,0x00,0x00,0x00,0x00,0x00};
uint8_t ST_SW_SENS_SERVICE_UUID[]  = {0x1b,0xc5,0xd5,0xa5,0x02,0x00,0xb4,0x9a,0xe1,0x11,0x02,0x00,0x00,0x00,0x00,0x00};
uint8_t ST_CONSOLE_SERVICE_UUID[]  = {0x1b,0xc5,0xd5,0xa5,0x02,0x00,0xb4,0x9a,0xe1,0x11,0x0E,0x00,0x00,0x00,0x00,0x00};

/* ST Custom Characteristics UUID */
/* ENVIRONMENTAL */
uint8_t ST_ENVIRONMENTAL_CHAR_UUID[]      = {0x1b,0xc5,0xd5,0xa5,0x02,0x00,0x36,0xac,0xe1,0x11,0x01,0x00,0x00,0x00,0x1d,0x00};
/* ENVIRONMENTAL for Sensor Tile */
uint8_t ST_ENVIRONMENTAL_ST_CHAR_UUID[]   = {0x1b,0xc5,0xd5,0xa5,0x02,0x00,0x36,0xac,0xe1,0x11,0x01,0x00,0x00,0x00,0x14,0x00};
/* PRESSURE */
uint8_t ST_PRESSURE_CHAR_UUID[]           = {0x1b,0xc5,0xd5,0xa5,0x02,0x00,0x36,0xac,0xe1,0x11,0x01,0x00,0x00,0x00,0x10,0x00};
/* HUMIDITY */
uint8_t ST_HUMIDITY_CHAR_UUID[]           = {0x1b,0xc5,0xd5,0xa5,0x02,0x00,0x36,0xac,0xe1,0x11,0x01,0x00,0x00,0x00,0x08,0x00};
/* TEMPERATURE */
uint8_t ST_TEMPERATURE_CHAR_UUID[]        = {0x1b,0xc5,0xd5,0xa5,0x02,0x00,0x36,0xac,0xe1,0x11,0x01,0x00,0x00,0x00,0x04,0x00};
/* LED */
uint8_t ST_LED_CHAR_UUID[]                = {0x1b,0xc5,0xd5,0xa5,0x02,0x00,0x36,0xac,0xe1,0x11,0x01,0x00,0x00,0x00,0x00,0x20};
/* ACCELERATION EVENT */
uint8_t ST_ACC_EVENT_CHAR_UUID[]          = {0x1b,0xc5,0xd5,0xa5,0x02,0x00,0x36,0xac,0xe1,0x11,0x01,0x00,0x00,0x04,0x00,0x00};
/* MICROPHONE */
uint8_t ST_MIC_EVENT_CHAR_UUID[]          = {0x1b,0xc5,0xd5,0xa5,0x02,0x00,0x36,0xac,0xe1,0x11,0x01,0x00,0x00,0x00,0x00,0x04};
/* PROXIMITY */
uint8_t ST_PROXIMITY_CHAR_UUID[]          = {0x1b,0xc5,0xd5,0xa5,0x02,0x00,0x36,0xac,0xe1,0x11,0x01,0x00,0x00,0x00,0x00,0x02};
/* LUX */
uint8_t ST_LUX_CHAR_UUID[]                = {0x1b,0xc5,0xd5,0xa5,0x02,0x00,0x36,0xac,0xe1,0x11,0x01,0x00,0x00,0x00,0x00,0x01};
/* CONFIGURATION */
uint8_t ST_CONFIG_CHAR_UUID[]             = {0x1b,0xc5,0xd5,0xa5,0x02,0x00,0x36,0xac,0xe1,0x11,0x0F,0x00,0x02,0x00,0x00,0x00};
/* ACCELEROMETER, GYROSCOPE, MAGNETOMETER */
uint8_t ST_ACC_GYRO_MAG_CHAR_UUID[]       = {0x1b,0xc5,0xd5,0xa5,0x02,0x00,0x36,0xac,0xe1,0x11,0x01,0x00,0x00,0x00,0xE0,0x00};
/* QUATERNIONS */
uint8_t ST_QUATERNIONS_CHAR_UUID[]        = {0x1b,0xc5,0xd5,0xa5,0x02,0x00,0x36,0xac,0xe1,0x11,0x01,0x00,0x00,0x01,0x00,0x00};
/* E-COMPASS */
uint8_t ST_ECOMPASS_CHAR_UUID[]           = {0x1b,0xc5,0xd5,0xa5,0x02,0x00,0x36,0xac,0xe1,0x11,0x01,0x00,0x40,0x00,0x00,0x00};
/* Activity Recognition */
uint8_t ST_ACTIVITY_REC_CHAR_UUID[]       = {0x1b,0xc5,0xd5,0xa5,0x02,0x00,0x36,0xac,0xe1,0x11,0x01,0x00,0x10,0x00,0x00,0x00};
/* Carry Position recognition */
uint8_t ST_CARRY_POSITION_REC_CHAR_UUID[] = {0x1b,0xc5,0xd5,0xa5,0x02,0x00,0x36,0xac,0xe1,0x11,0x01,0x00,0x08,0x00,0x00,0x00};
/* Gesture Recognition */
uint8_t ST_GESTURE_REC_CHAR_UUID[]        = {0x1b,0xc5,0xd5,0xa5,0x02,0x00,0x36,0xac,0xe1,0x11,0x01,0x00,0x02,0x00,0x00,0x00};
/* Pedometer */
uint8_t ST_ACC_PEDO_CHAR_UUID[]           = {0x1b,0xc5,0xd5,0xa5,0x02,0x00,0x36,0xac,0xe1,0x11,0x01,0x00,0x01,0x00,0x00,0x00};
/* Intensity Detection */
uint8_t ST_INTENSITY_DET_CHAR_UUID[]      = {0x1b,0xc5,0xd5,0xa5,0x02,0x00,0x36,0xac,0xe1,0x11,0x01,0x00,0x20,0x00,0x00,0x00};
/* Terminal */
uint8_t ST_TERM_CHAR_UUID[]               = {0x1b,0xc5,0xd5,0xa5,0x02,0x00,0x36,0xac,0xe1,0x11,0x0E,0x00,0x01,0x00,0x00,0x00};
/* Standard Error */
uint8_t ST_STDERR_CHAR_UUID[]             = {0x1b,0xc5,0xd5,0xa5,0x02,0x00,0x36,0xac,0xe1,0x11,0x0E,0x00,0x02,0x00,0x00,0x00};

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void User_Init(void);
static void User_Process(void);
static void Get_Action(void);
static void Central_Process(void);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */
