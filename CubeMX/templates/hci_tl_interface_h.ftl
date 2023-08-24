[#ftl]
/**
  ******************************************************************************
  * @file    hci_tl_interface.h
  * @author  SRA Application Team
  * @brief   Header file for hci_tl_interface.c
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
#ifndef HCI_TL_INTERFACE_H
#define HCI_TL_INTERFACE_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
[#if includes??]
[#list includes as include]
#include "${include}"
[/#list]
[/#if]
#include "${BoardName}_bus.h"

/* Exported Defines ----------------------------------------------------------*/
[#assign IpInstance = ""]
[#assign UsartInstance = ""]
[#assign IpName = ""]
[#assign IrqNumber = ""]
[#assign BusDriver = ""]
[#assign ExtiLine = ""]

[#assign BNRG_RST_PORT = ""]
[#assign BNRG_RST_PIN = ""]

[#assign BNRG_IRQ_PORT = ""]
[#assign BNRG_IRQ_PIN = ""]

[#assign BNRG_CS_PORT = ""]
[#assign BNRG_CS_PIN = ""]

[#assign BNRG_EXTI_PORT = ""]
[#assign BNRG_EXTI_PIN = ""]
[#assign BNRG_EXTI_IRQn = ""]
[#assign BNRG_EXTI_LINE = ""]

[#list BspIpDatas as SWIP] 
	[#if SWIP.variables??]
		[#list SWIP.variables as variables]
			[#if variables.name?contains("IpInstance")]
				[#assign IpInstance = variables.value]
			[/#if]
			[#if variables.name?contains("IpName")]
				[#assign IpName = variables.value]
			[/#if]
			[#if variables.name?contains("GPIO_INT_NUM")]
				[#assign IrqNumber = variables.value]
			[/#if]
			[#if variables.name?contains("EXTI_LINE_NUMBER")]
				[#assign ExtiLine = variables.value]
			[/#if]
			[#if variables.value?contains("BUS IO driver")]
				[#assign SpiInstance = IpInstance]
			[/#if]
			[#if variables.value?contains("Exti Line")]
				[#assign BNRG_EXTI_PORT = IpName]
				[#assign BNRG_EXTI_PIN = IpInstance]
				[#assign BNRG_EXTI_IRQn = IrqNumber]
				[#assign BNRG_EXTI_LINE = ExtiLine]
			[/#if]						
			[#if variables.value?contains("CS Line")]
				[#assign BNRG_CS_PORT = IpName]
				[#assign BNRG_CS_PIN = IpInstance]				
			[/#if]
			[#if variables.value?contains("Reset Line")]
				[#assign BNRG_RST_PORT = IpName]
				[#assign BNRG_RST_PIN = IpInstance]				
			[/#if]			
		[/#list]
	[/#if]
[/#list]

[#assign useIFRSTACKUPD = false]
[#if RTEdatas??]
[#list RTEdatas as define]

[#if define?contains("IFRSTACK_UPDATER")]
[#assign useIFRSTACKUPD = true]
[/#if]

[/#list]
[/#if]

#define HCI_TL_SPI_EXTI_PORT  ${BNRG_EXTI_PORT}
#define HCI_TL_SPI_EXTI_PIN   ${BNRG_EXTI_PIN}
#define HCI_TL_SPI_EXTI_IRQn  ${BNRG_EXTI_IRQn}

#define HCI_TL_SPI_IRQ_PORT   ${BNRG_EXTI_PORT}
#define HCI_TL_SPI_IRQ_PIN    ${BNRG_EXTI_PIN}

#define HCI_TL_SPI_CS_PORT    ${BNRG_CS_PORT}
#define HCI_TL_SPI_CS_PIN     ${BNRG_CS_PIN}

#define HCI_TL_RST_PORT       ${BNRG_RST_PORT}
#define HCI_TL_RST_PIN        ${BNRG_RST_PIN}

[#if useIFRSTACKUPD]
#define HCI_TL_SPI_MOSI_PORT  BUS_${SpiInstance}_MOSI_GPIO_PORT
#define HCI_TL_SPI_MOSI_PIN   BUS_${SpiInstance}_MOSI_GPIO_PIN
#define HCI_TL_SPI_MISO_PORT  BUS_${SpiInstance}_MISO_GPIO_PORT
#define HCI_TL_SPI_MISO_PIN   BUS_${SpiInstance}_MISO_GPIO_PIN
#ifdef IS_GPIO_AF 
#define HCI_TL_SPI_MOSI_AF    BUS_${SpiInstance}_MOSI_GPIO_AF
#define HCI_TL_SPI_MISO_AF    BUS_${SpiInstance}_MISO_GPIO_AF
#endif
[/#if]

/* Exported variables --------------------------------------------------------*/
extern EXTI_HandleTypeDef     hexti${BNRG_EXTI_LINE};
#define H_EXTI_${BNRG_EXTI_LINE} hexti${BNRG_EXTI_LINE}

/* Exported Functions --------------------------------------------------------*/
int32_t HCI_TL_SPI_Init    (void* pConf);
int32_t HCI_TL_SPI_DeInit  (void);
int32_t HCI_TL_SPI_Receive (uint8_t* buffer, uint16_t size);
int32_t HCI_TL_SPI_Send    (uint8_t* buffer, uint16_t size);
int32_t HCI_TL_SPI_Reset   (void);

/**
 * @brief  Register hci_tl_interface IO bus services
 *
 * @param  None
 * @retval None
 */
void hci_tl_lowlevel_init(void);

/**
 * @brief HCI Transport Layer Low Level Interrupt Service Routine
 *
 * @param  None
 * @retval None
 */
void hci_tl_lowlevel_isr(void);

#ifdef __cplusplus
}
#endif
#endif /* HCI_TL_INTERFACE_H */
