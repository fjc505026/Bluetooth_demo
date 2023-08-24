[#ftl]
#include "bluenrg_conf.h"

#include "hci.h"
#include "bluenrg1_types.h"
#include "bluenrg1_gap.h"
#include "bluenrg1_aci.h"
#include "bluenrg1_hci_le.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/
/* Set to 1 for enabling Flags AD Type position at the beginning 
   of the advertising packet */
#define ENABLE_FLAGS_AD_TYPE_AT_BEGINNING 1

#if (ADV_DATA_TYPE != ADV_NONCONN_IND)
   #warning "The advertising data type should be of 'non-connectable and no scan response' type."
#endif
#if (ADV_INTERV_MIN > ADV_INTERV_MAX)
   #error "The min advertising interval cannot be greater than the max advertising interval."
#endif
#if ((ADV_INTERV_MIN <= 0) || (ADV_INTERV_MIN > 40959))
  #error "Invalid min advertising interval! Please select a value between 0 and 40959 ms."
#endif
#if ((ADV_INTERV_MAX <= 0) || (ADV_INTERV_MAX > 40959))
  #error "Invalid max advertising interval! Please select a value between 0 and 40959 ms."
#endif

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void User_Init(void);
static void User_Process(void);
static void Device_Init(void);
static void Start_Beaconing(void);

void EnterStopMode(void);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */
