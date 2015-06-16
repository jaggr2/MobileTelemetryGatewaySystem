/***************************************************************************//**
 * @brief   Functions to create Bluetooth Smart Services.
 *
 * This module allows the creation and handling of Bluetooth Smart Services.
 * Furthermore it includes helper functions for often used operations.
 *
 * @file    service.c
 * @version 1.0
 * @date    18.11.2014
 * @author  Daniel Meer
 *
 * @remark  Last Modifications:
 *          18.11.2014 meerd1 created
 ******************************************************************************/

/*----- Header-Files ---------------------------------------------------------*/
#include "service.h"

#include <string.h>

#include "txw51_framework/utils/txw51_errors.h"

/*----- Macros ---------------------------------------------------------------*/

/*----- Data types -----------------------------------------------------------*/

/*----- Function prototypes --------------------------------------------------*/
static void SERV_OnConnect(struct TXW51_SERV_Handle *handle, ble_evt_t *bleEvent);
static void SERV_OnDisconnect(struct TXW51_SERV_Handle *handle, ble_evt_t *bleEvent);

/*----- Data -----------------------------------------------------------------*/

/*----- Implementation -------------------------------------------------------*/

uint32_t TXW51_SERV_Create(struct TXW51_SERV_ServiceInit *init,
                           struct TXW51_SERV_Handle *handle)
{
    uint32_t err;

    // Initialize service structure
    handle->ConnHandle = BLE_CONN_HANDLE_INVALID;

    // Set UUID
    err = sd_ble_uuid_vs_add(&init->BaseUuid, &handle->UuidType);
    if (err != NRF_SUCCESS) {
        return ERR_BLE_SERVICE_ADD_UUID;
    }

    // Add service
    ble_uuid_t bleUuid;
    bleUuid.type = handle->UuidType;
    bleUuid.uuid = init->ServiceUuid;

    uint8_t primary_or_secondary = (init->IsSecondary) ?
            BLE_GATTS_SRVC_TYPE_SECONDARY : BLE_GATTS_SRVC_TYPE_PRIMARY;

    err = sd_ble_gatts_service_add(primary_or_secondary,
                                   &bleUuid,
                                   &handle->ServiceHandle);
    if (err != NRF_SUCCESS) {
        return ERR_BLE_SERVICE_ADD_SERVICE;
    }

    return ERR_NONE;
}


void TXW51_SERV_InitChar(struct TXW51_SERV_Handle *serviceHandle,
                         uint16_t charUuid,
                         struct TXW51_SERV_CharInit *charac)
{
    uint8_t initialValue = 0;

    /* TODO: memset is the nicer way to initialize, but we can't see what
             fields get initialized. */
    /*memset(&charac->metadata, 0, sizeof(charac->metadata));
    memset(&charac->attr_metadata, 0, sizeof(charac->attr_metadata));
    memset(&charac->attribute, 0, sizeof(charac->attribute));*/

    charac->Metadata.char_props.broadcast      = 0;
    charac->Metadata.char_props.read           = 0;
    charac->Metadata.char_props.write_wo_resp  = 0;
    charac->Metadata.char_props.write          = 0;
    charac->Metadata.char_props.notify         = 0;
    charac->Metadata.char_props.indicate       = 0;
    charac->Metadata.char_props.auth_signed_wr = 0;

    charac->Metadata.char_ext_props.reliable_wr = 0;
    charac->Metadata.char_ext_props.wr_aux      = 0;

    charac->Metadata.p_char_user_desc        = NULL;
    charac->Metadata.char_user_desc_max_size = 0;
    charac->Metadata.char_user_desc_size     = 0;

    charac->Metadata.p_char_pf      = NULL;
    charac->Metadata.p_user_desc_md = NULL;
    charac->Metadata.p_cccd_md      = NULL;
    charac->Metadata.p_sccd_md      = NULL;

    ble_srv_security_mode_t attrSecurityMode;
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&attrSecurityMode.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&attrSecurityMode.write_perm);

    charac->AttrMetadata.read_perm  = attrSecurityMode.read_perm;
    charac->AttrMetadata.write_perm = attrSecurityMode.write_perm;
    charac->AttrMetadata.vlen       = 0;
    charac->AttrMetadata.vloc       = BLE_GATTS_VLOC_STACK;
    charac->AttrMetadata.rd_auth    = 0;
    charac->AttrMetadata.wr_auth    = 0;

    charac->BleUuid.type = serviceHandle->UuidType;
    charac->BleUuid.uuid = charUuid;

    charac->Attribute.p_uuid    = &charac->BleUuid;
    charac->Attribute.p_attr_md = &charac->AttrMetadata;
    charac->Attribute.init_len  = sizeof(initialValue);
    charac->Attribute.init_offs = 0;
    charac->Attribute.max_len   = sizeof(initialValue);
    charac->Attribute.p_value   = &initialValue;
}


uint32_t TXW51_SERV_AddChar(struct TXW51_SERV_Handle *serviceHandle,
                            struct TXW51_SERV_CharInit *charInit,
                            ble_gatts_char_handles_t *charHandle)
{
    uint32_t err;
    err = sd_ble_gatts_characteristic_add(serviceHandle->ServiceHandle,
                                          &charInit->Metadata,
                                          &charInit->Attribute,
                                          charHandle);
    if (err != NRF_SUCCESS) {
        return ERR_BLE_SERVICE_ADD_CHARACTERISTIC;
    }
    return ERR_NONE;
}


void TXW51_SERV_OnBleEvent(struct TXW51_SERV_Handle *handle,
                         ble_evt_t *bleEvent)
{
    switch (bleEvent->header.evt_id) {
        case BLE_GAP_EVT_CONNECTED:
            SERV_OnConnect(handle, bleEvent);
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            SERV_OnDisconnect(handle, bleEvent);
            break;

        default:
            break;
    }
}


/***************************************************************************//**
* @brief Handles the connection event.
*
* @param[in,out] handle   The handle for the service.
* @param[in]     bleEvent The BLE event that occurred.
* @return Nothing.
******************************************************************************/
static void SERV_OnConnect(struct TXW51_SERV_Handle *handle,
                           ble_evt_t *bleEvent)
{
    handle->ConnHandle = bleEvent->evt.gap_evt.conn_handle;
}


/***************************************************************************//**
* @brief Handles the disconnection event.
*
* @param[in,out] handle   The handle for the service.
* @param[in]     bleEvent The BLE event that occurred.
* @return Nothing.
******************************************************************************/
static void SERV_OnDisconnect(struct TXW51_SERV_Handle *handle,
                              ble_evt_t *bleEvent)
{
    handle->ConnHandle = BLE_CONN_HANDLE_INVALID;
}


void TXW51_SERV_AddUserDescription(struct TXW51_SERV_CharInit *charHandle,
                                   uint8_t *description)
{
    charHandle->Metadata.p_char_user_desc        = description;
    charHandle->Metadata.char_user_desc_max_size = strlen((char *)description);
    charHandle->Metadata.char_user_desc_size     = strlen((char *)description);
}

