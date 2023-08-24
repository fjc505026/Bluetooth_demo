[#ftl]
/**
  ******************************************************************************
  * @file    app_${ModuleName?lower_case}.h
  * @author  SRA Application Team
  * @brief   Header file for app_${ModuleName?lower_case}.c
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
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef APP_${ModuleName?upper_case}_H
#define APP_${ModuleName?upper_case}_H

#ifdef __cplusplus
 extern "C" {
#endif

[#assign useSENSORDEMO_BS = false]
[#assign useSAMPLEAPP = false]
[#assign useBEACON = false]
[#assign useCENTRAL = false]
[#assign useVIRTUALCOMPORT = false]
[#assign useIFRSTACKUPDATER = false]

[#if RTEdatas??]
[#list RTEdatas as define]

[#if define?contains("SENSOR_DEMO_BS")]
[#assign useSENSORDEMO_BS = true]
[/#if]
[#if define?contains("SAMPLE_APP")]
[#assign useSAMPLEAPP = true]
[/#if]
[#if define?contains("BEACON")]
[#assign useBEACON = true]
[/#if]
[#if define?contains("CENTRAL")]
[#assign useCENTRAL = true]
[/#if]
[#if define?contains("VIRTUAL_COM_PORT")]
[#assign useVIRTUALCOMPORT = true]
[/#if]
[#if define?contains("IFRSTACK_UPDATER")]
[#assign useIFRSTACKUPDATER = true]
[/#if]

[/#list]
[/#if]

/* Includes ------------------------------------------------------------------*/
[#if includes??]
[#list includes as include]
[/#list]
[/#if]
[#if useCENTRAL]
#include <stdio.h>
[/#if]
[#if useSENSORDEMO_BS | useSAMPLEAPP | useBEACON | useCENTRAL | useVIRTUALCOMPORT | useIFRSTACKUPDATER]
#include "${BoardName}.h"
[/#if]

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported Defines ----------------------------------------------------------*/
[#if useSENSORDEMO_BS]
/*************** Don't Change the following defines *************/

/* Package Version only numbers 0->9 */
#define PACK_VERSION_MAJOR '3'
#define PACK_VERSION_MINOR '2'
#define PACK_VERSION_PATCH '2'

/* Define the application Name (MUST be 7 char long) */
#define APP_NAME 'S','D','E','M',PACK_VERSION_MAJOR,PACK_VERSION_MINOR,PACK_VERSION_PATCH

/* Package Name */
#define BLUENRG_PACKAGENAME "X-CUBE-BLE2"

[/#if]
[#if useIFRSTACKUPDATER]
#define BLUENRG2_DEVICE

/**
 * Enable this #define if you want to change the BlueNRG IFR configuration.
 * IFR parameters can be edited in file Middlewares\ST\BlueNRG-2\hci\bluenrg1_devConfig.c
 * Disable this define if you want to update the BlueNRG fw stack.
*/
//#define BLUENRG_DEV_CONFIG_UPDATER

#ifndef BLUENRG_DEV_CONFIG_UPDATER
  /**
   * Update the DTM image using the FW_IMAGE provided in the file update_fw_image.c.
   * Disable this define if you want to update the BlueNRG fw stack through YMODEM
   * You can use this mode when using STM32 MCUs with small flash size (i.e.  
   * when you cannot use the BLUENRG_STACK_UPDATER mode).
   */
  //#define BLUENRG_STACK_UPDATER

  #ifndef BLUENRG_STACK_UPDATER
    /**
     * Update the DTM image using a serial communication SW on PC
     * (as HyperTerminal or TeraTerm).
     * The DTM image to transfer, usually named DTM_SPI_NOUPDATER.bin, can be found
     * in the ST BlueNRG GUI installation folder 
     * (usually C:\Users\<user name>\ST\BlueNRG GUI X.Y.Z\)
     * at location Firmware\BlueNRG2\DTM.
     */ 
    #define BLUENRG_STACK_UPDATER_YMODEM
  #endif

#endif 
[/#if]
[#if useCENTRAL]
#define message printf 
[/#if]

/* USER CODE BEGIN ED */

/* USER CODE END ED */

/* Exported Variables --------------------------------------------------------*/

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Exported Functions Prototypes ---------------------------------------------*/
void ${fctName}(void);
void ${fctProcessName}(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif
#endif /* APP_${ModuleName?upper_case}_H */
