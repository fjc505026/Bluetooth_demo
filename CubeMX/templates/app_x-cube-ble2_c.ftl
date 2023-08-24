[#ftl]
/**
  ******************************************************************************
  * @file    app_${ModuleName?lower_case}.c
  * @author  SRA Application Team
  * @brief   BlueNRG-2 initialization and applicative code
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
  
/* Includes ------------------------------------------------------------------*/
#include "app_${ModuleName?lower_case}.h"
[#if includes??]
[#list includes as include]
#include "${include}"
[/#list]
[/#if]

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

[#if useSENSORDEMO_BS]
[@common.optinclude name=mxTmpFolder + "/sensor_demo_bs_gv.tmp"/]
[/#if]
[#if useSAMPLEAPP]
[@common.optinclude name=mxTmpFolder + "/sample_app_gv.tmp"/]
[/#if]
[#if useBEACON]
[@common.optinclude name=mxTmpFolder + "/beacon_gv.tmp"/]
[/#if]
[#if useCENTRAL]
[@common.optinclude name=mxTmpFolder + "/central_gv.tmp"/]
[/#if]
[#if useVIRTUALCOMPORT]
[@common.optinclude name=mxTmpFolder + "/virtual_com_port_gv.tmp"/]
[/#if]
[#if useIFRSTACKUPDATER]
[@common.optinclude name=mxTmpFolder + "/ifrstack_updater_gv.tmp"/]
[/#if]

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

void ${fctName}(void)
{
  /* USER CODE BEGIN SV */ 

  /* USER CODE END SV */
  
  /* USER CODE BEGIN ${fctName?replace("MX_","")}_PreTreatment */
  
  /* USER CODE END ${fctName?replace("MX_","")}_PreTreatment */

[#if useSENSORDEMO_BS | useSAMPLEAPP | useBEACON | useCENTRAL | useIFRSTACKUPDATER]
  /* Initialize the peripherals and the BLE Stack */
[/#if]
[#if useSENSORDEMO_BS]
[@common.optinclude name=mxTmpFolder + "/sensor_demo_bs_init.tmp"/]
[/#if]
[#if useSAMPLEAPP]
[@common.optinclude name=mxTmpFolder + "/sample_app_init.tmp"/]
[/#if]
[#if useBEACON]
[@common.optinclude name=mxTmpFolder + "/beacon_init.tmp"/]
[/#if]
[#if useCENTRAL]
[@common.optinclude name=mxTmpFolder + "/central_init.tmp"/]
[/#if]
[#if useVIRTUALCOMPORT]
[@common.optinclude name=mxTmpFolder + "/virtual_com_port_init.tmp"/]
[/#if]
[#if useIFRSTACKUPDATER]
[@common.optinclude name=mxTmpFolder + "/ifrstack_updater_init.tmp"/]
[/#if]
  
  /* USER CODE BEGIN ${fctName?replace("MX_","")}_PostTreatment */
  
  /* USER CODE END ${fctName?replace("MX_","")}_PostTreatment */
}

/*
 * BlueNRG-2 background task
 */
void ${fctProcessName}(void)
{
  /* USER CODE BEGIN ${fctProcessName?replace("MX_","")}_PreTreatment */
  
  /* USER CODE END ${fctProcessName?replace("MX_","")}_PreTreatment */
  
[#if useSENSORDEMO_BS | useSAMPLEAPP | useBEACON | useCENTRAL]
  hci_user_evt_proc();
[/#if]
[#if useSENSORDEMO_BS | useSAMPLEAPP | useBEACON | useCENTRAL | useVIRTUALCOMPORT | useIFRSTACKUPDATER]
  User_Process();
[/#if]

  /* USER CODE BEGIN ${fctProcessName?replace("MX_","")}_PostTreatment */
  
  /* USER CODE END ${fctProcessName?replace("MX_","")}_PostTreatment */
}

[#if useSENSORDEMO_BS]
[#-- SensorDemoBLESensor Code Begin--]
[@common.optinclude name=mxTmpFolder + "/sensor_demo_bs.tmp"/]
[#-- SensorDemoBLESensor Code End--]
[/#if]
[#if useSAMPLEAPP]
[#-- SampleApp Code Begin--]
[@common.optinclude name=mxTmpFolder + "/sample_app.tmp"/]
[#-- SampleApp Code End--]
[/#if]
[#if useBEACON]
[#-- Beacon Code Begin--]
[@common.optinclude name=mxTmpFolder + "/beacon.tmp"/]
[#-- Beacon Code End--]
[/#if]
[#if useCENTRAL]
[#-- Central Code Begin--]
[@common.optinclude name=mxTmpFolder + "/central.tmp"/]
[#-- Central Code End--]
[/#if]
[#if useVIRTUALCOMPORT]
[#-- VirtualCOMPort Code Begin--]
[@common.optinclude name=mxTmpFolder + "/virtual_com_port.tmp"/]
[#-- VirtualCOMPort Code End--]
[/#if]
[#if useIFRSTACKUPDATER]
[#-- IFRStackUpdater Code Begin--]
[@common.optinclude name=mxTmpFolder + "/ifrstack_updater.tmp"/]
[#-- IFRStackUpdater Code End--]
[/#if]
