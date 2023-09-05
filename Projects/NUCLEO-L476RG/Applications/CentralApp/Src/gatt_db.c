/**
 ******************************************************************************
 * @file    gatt_db.c
 * @author  SRA Application Team
 * @brief   Functions to build GATT DB and handle GATT events.
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

#include "gatt_db.h"
#include "bluenrg1_aci.h"
#include "bluenrg1_hci_le.h"

#define GATT_DB_CUSTOM_UUID_BYTE_NUM ( 16U )

#define COPY_UUID_128( uuid_struct, uuid_15, uuid_14, uuid_13, uuid_12, uuid_11, uuid_10, uuid_9, uuid_8, uuid_7,      \
                       uuid_6, uuid_5, uuid_4, uuid_3, uuid_2, uuid_1, uuid_0 )                                        \
    do                                                                                                                 \
    {                                                                                                                  \
        uuid_struct.uuid128[0]  = uuid_0;                                                                              \
        uuid_struct.uuid128[1]  = uuid_1;                                                                              \
        uuid_struct.uuid128[2]  = uuid_2;                                                                              \
        uuid_struct.uuid128[3]  = uuid_3;                                                                              \
        uuid_struct.uuid128[4]  = uuid_4;                                                                              \
        uuid_struct.uuid128[5]  = uuid_5;                                                                              \
        uuid_struct.uuid128[6]  = uuid_6;                                                                              \
        uuid_struct.uuid128[7]  = uuid_7;                                                                              \
        uuid_struct.uuid128[8]  = uuid_8;                                                                              \
        uuid_struct.uuid128[9]  = uuid_9;                                                                              \
        uuid_struct.uuid128[10] = uuid_10;                                                                             \
        uuid_struct.uuid128[11] = uuid_11;                                                                             \
        uuid_struct.uuid128[12] = uuid_12;                                                                             \
        uuid_struct.uuid128[13] = uuid_13;                                                                             \
        uuid_struct.uuid128[14] = uuid_14;                                                                             \
        uuid_struct.uuid128[15] = uuid_15;                                                                             \
    } while( 0 )

uint16_t u16LocalServHandle, u16LocalNormalWriteCharHandle, u16LocalEncryptedWriteCharHandle,
u16LocalAuthenWriteCharHandle;
/*******************************************************************************
 * Function Name  : Add_Sample_Service
 * Description    : Add the 'Accelerometer' service.
 * Input          : None
 * Return         : Status.
 *******************************************************************************/
uint8_t GATT_DB_u8AddService( void )
{
    uint8_t u8Ret;

    // UUIDs:
    // D973F2E0-B19E-11E2-9E96-0800200C9A66
    // D973F2E1-B19E-11E2-9E96-0800200C9A66
    // D973F2E2-B19E-11E2-9E96-0800200C9A66
    // D973F2E3-B19E-11E2-9E96-0800200C9A66

    // clang-format off
    const uint8_t cu8ServiceUUID[GATT_DB_CUSTOM_UUID_BYTE_NUM]                  = {0x66,0x9a,0x0c,0x20,0x00,0x08,0x96,0x9e,0xe2,0x11,0x9e,0xb1,0xe0,0xf2,0x73,0xd9};
    const uint8_t cu8CharUUIDNormalControl[GATT_DB_CUSTOM_UUID_BYTE_NUM]        = {0x66,0x9a,0x0c,0x20,0x00,0x08,0x96,0x9e,0xe2,0x11,0x9e,0xb1,0xe1,0xf2,0x73,0xd9};
    const uint8_t cu8CharUUIDEncryptedControl[GATT_DB_CUSTOM_UUID_BYTE_NUM]     = {0x66,0x9a,0x0c,0x20,0x00,0x08,0x96,0x9e,0xe2,0x11,0x9e,0xb1,0xe2,0xf2,0x73,0xd9};
    const uint8_t cu8CharUUIDAuthenticatedControl[GATT_DB_CUSTOM_UUID_BYTE_NUM] = {0x66,0x9a,0x0c,0x20,0x00,0x08,0x96,0x9e,0xe2,0x11,0x9e,0xb1,0xe3,0xf2,0x73,0xd9};
    // clang-format on

    /**
     * Number of attribute records that can be added to this service
     * For this service it is given by:
     * 1 (fixed value) + 3 (for characteristic with CHAR_PROP_NOTIFY) + 2 (for characteristic with CHAR_PROP_WRITE)
     */
    uint8_t max_attribute_records = 1 + 3 * 3;

    const uint16_t u16CharLen = 0x0001U;

    Service_UUID_t unTempServiceUUID;
    Char_UUID_t    unTempCharUUID;

    BLUENRG_memcpy( &unTempServiceUUID.Service_UUID_128, cu8ServiceUUID, GATT_DB_CUSTOM_UUID_BYTE_NUM );

    u8Ret = aci_gatt_add_service( UUID_TYPE_128, &unTempServiceUUID, PRIMARY_SERVICE, max_attribute_records,
                                  &u16LocalServHandle );
    if( u8Ret != BLE_STATUS_SUCCESS )
        goto fail;

    // add normal write char
    BLUENRG_memcpy( &unTempCharUUID.Char_UUID_128, cu8CharUUIDNormalControl, GATT_DB_CUSTOM_UUID_BYTE_NUM );
    u8Ret = aci_gatt_add_char( u16LocalServHandle, UUID_TYPE_128, &unTempCharUUID, u16CharLen,
                               CHAR_PROP_READ | CHAR_PROP_WRITE | CHAR_PROP_NOTIFY, ATTR_PERMISSION_NONE,
                               GATT_NOTIFY_ATTRIBUTE_WRITE, 16, 0, &u16LocalNormalWriteCharHandle );
    if( u8Ret != BLE_STATUS_SUCCESS )
        goto fail;

    // add encrypted write char
    BLUENRG_memcpy( &unTempCharUUID.Char_UUID_128, cu8CharUUIDEncryptedControl, GATT_DB_CUSTOM_UUID_BYTE_NUM );
    u8Ret = aci_gatt_add_char( u16LocalServHandle, UUID_TYPE_128, &unTempCharUUID, u16CharLen,
                               CHAR_PROP_READ | CHAR_PROP_WRITE | CHAR_PROP_NOTIFY, ATTR_PERMISSION_ENCRY_WRITE,
                               GATT_NOTIFY_ATTRIBUTE_WRITE, 16, 0, &u16LocalEncryptedWriteCharHandle );
    if( u8Ret != BLE_STATUS_SUCCESS )
        goto fail;

    // add encrypted write char
    BLUENRG_memcpy( &unTempCharUUID.Char_UUID_128, cu8CharUUIDAuthenticatedControl, GATT_DB_CUSTOM_UUID_BYTE_NUM );
    u8Ret = aci_gatt_add_char( u16LocalServHandle, UUID_TYPE_128, &unTempCharUUID, u16CharLen,
                               CHAR_PROP_READ | CHAR_PROP_WRITE | CHAR_PROP_NOTIFY, ATTR_PERMISSION_AUTHEN_WRITE,
                               GATT_NOTIFY_ATTRIBUTE_WRITE, 16, 0, &u16LocalAuthenWriteCharHandle );
    if( u8Ret != BLE_STATUS_SUCCESS )
        goto fail;

    PRINT_DBG( "Local Control Service added\r\n" );

    PRINT_DBG( "Normal        Write Char Handle 0x%04X\r\n", u16LocalNormalWriteCharHandle );
    PRINT_DBG( "Encrypted     Write Char Handle 0x%04X\r\n", u16LocalEncryptedWriteCharHandle );
    PRINT_DBG( "Authenticated Write Char Handle 0x%04X\r\n", u16LocalAuthenWriteCharHandle );

    return BLE_STATUS_SUCCESS;

fail:
    PRINT_DBG( "Error while adding Sample service.\r\n" );
    return BLE_STATUS_ERROR;
}

// clang-format off
void APP_UserEvtRx(void *pData)
{
  uint32_t i;

  hci_spi_pckt *hci_pckt = (hci_spi_pckt *)pData;

  if(hci_pckt->type == HCI_EVENT_PKT)
  {
    hci_event_pckt *event_pckt = (hci_event_pckt*)hci_pckt->data;

    if(event_pckt->evt == EVT_LE_META_EVENT)
    {
      evt_le_meta_event *evt = (void *)event_pckt->data;

      for (i = 0; i < (sizeof(hci_le_meta_events_table)/sizeof(hci_le_meta_events_table_type)); i++)
      {
        if (evt->subevent == hci_le_meta_events_table[i].evt_code)
        {
          hci_le_meta_events_table[i].process((void *)evt->data);
        }
      }
    }
    else if(event_pckt->evt == EVT_VENDOR)
    {
      evt_blue_aci *blue_evt = (void*)event_pckt->data;

      for (i = 0; i < (sizeof(hci_vendor_specific_events_table)/sizeof(hci_vendor_specific_events_table_type)); i++)
      {
        if (blue_evt->ecode == hci_vendor_specific_events_table[i].evt_code)
        {
          hci_vendor_specific_events_table[i].process((void *)blue_evt->data);
        }
      }
    }
    else
    {
      for (i = 0; i < (sizeof(hci_events_table)/sizeof(hci_events_table_type)); i++)
      {
        if (event_pckt->evt == hci_events_table[i].evt_code)
        {
          hci_events_table[i].process((void *)event_pckt->data);
        }
      }
    }
  }
}
// clang-format on