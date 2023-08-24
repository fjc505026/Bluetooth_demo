[#ftl]
#include <stdlib.h>
#include <stdio.h>
#include "bluenrg1_aci.h"
#include "bluenrg1_hci_le.h"
#include "bluenrg1_events.h"
#include "hci_tl.h"
#include "hci.h"
#include "bluenrg_utils.h"
#include "${BoardName}.h"

#ifdef BLUENRG_STACK_UPDATER_YMODEM
  #include "ymodem.h"
#endif /* BLUENRG_STACK_UPDATER_YMODEM */

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern const devConfig_t deviceConfig;
#ifdef BLUENRG_STACK_UPDATER
  extern const unsigned char FW_IMAGE[];
  extern unsigned int FW_IMAGE_SIZE;
#endif

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void User_Process(void);
static void User_Init(void);
void APP_UserEvtRx(void *pData);
#ifdef BLUENRG_STACK_UPDATER_YMODEM
  extern int32_t program_device_UART(void);
  extern void processInputData(uint8_t* data_buffer, uint16_t Nb_bytes);
#endif /* BLUENRG_STACK_UPDATER_YMODEM */

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */
