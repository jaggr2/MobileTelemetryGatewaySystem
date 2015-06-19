/***************************************************************************//**
 * @brief   Bluetooth Smart "Device Information Service".
 *
 * This service is slightly different to the standardized DIS service in that
 * it is tailored to our application and has additional strings.
 *
 * @file    service_dis.c
 * @version 1.0
 * @date    11.11.2014
 * @author  Daniel Meer
 *
 * @remark  Last Modifications:
 *          11.11.2014 meerd1 created
 ******************************************************************************/

/*----- Header-Files ---------------------------------------------------------*/
#include "service_dis.h"

#include <stdint.h>
#include <string.h>

#include "nrf/nordic_common.h"

#include "txw51_framework/config/config_services.h"
#include "txw51_framework/utils/log.h"
#include "txw51_framework/utils/txw51_errors.h"

/*----- Macros ---------------------------------------------------------------*/

/*----- Data types -----------------------------------------------------------*/

/*----- Function prototypes --------------------------------------------------*/
static void SERV_DIS_OnConnect(struct TXW51_SERV_DIS_Handle *handle,
                               ble_evt_t *bleEvent);
static void SERV_DIS_OnDisconnect(struct TXW51_SERV_DIS_Handle *handle,
                                  ble_evt_t *bleEvent);
static void SERV_DIS_OnWrite(struct TXW51_SERV_DIS_Handle *handle,
                             ble_evt_t *bleEvent);
static uint32_t SERV_DIS_AddAllChars(struct TXW51_SERV_DIS_Handle *serviceHandle,
                                     const struct TXW51_SERV_DIS_Init *init);
static uint32_t SERV_DIS_AddChar(struct TXW51_SERV_DIS_Handle *serviceHandle,
                                 uint16_t uuid,
                                 uint8_t *charValue,
                                 char *description,
                                 ble_gatts_char_handles_t *charHandle);
static uint32_t SERV_DIS_AddChar_Timer(struct TXW51_SERV_DIS_Handle *serviceHandle);
static uint32_t SERV_DIS_AddChar_SaveValues(struct TXW51_SERV_DIS_Handle *serviceHandle);

/*----- Data -----------------------------------------------------------------*/

/*----- Implementation -------------------------------------------------------*/

uint32_t TXW51_SERV_DIS_Init(struct TXW51_SERV_DIS_Handle *handle,
                             const struct TXW51_SERV_DIS_Init *init)
{
    uint32_t err;

    handle->EventHandler = init->EventHandler;

    struct TXW51_SERV_ServiceInit serviceInit = {
        .BaseUuid    = TXW51_SERVICE_UUID_BASE,
        .ServiceUuid = TXW51_SERV_DIS_UUID_SERVICE,
        .IsSecondary = false
    };

    err = TXW51_SERV_Create(&serviceInit, &handle->ServiceHandle);
    if (err != ERR_NONE) {
        TXW51_LOG_ERROR("[DIS Service] Service creation failed.");
        return err;
    }

    err = SERV_DIS_AddAllChars(handle, init);
    if (err != ERR_NONE) {
        TXW51_LOG_ERROR("[DIS Service] Could not create all characteristics.");
        return err;
    }

    TXW51_LOG_INFO("[DIS Service] Service creation successful.");
    return ERR_NONE;
}


void TXW51_SERV_DIS_OnBleEvent(struct TXW51_SERV_DIS_Handle *handle,
                               ble_evt_t *bleEvent)
{
    TXW51_SERV_OnBleEvent(&handle->ServiceHandle, bleEvent);

    switch (bleEvent->header.evt_id) {
        case BLE_GAP_EVT_CONNECTED:
            SERV_DIS_OnConnect(handle, bleEvent);
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            SERV_DIS_OnDisconnect(handle, bleEvent);
            break;

        case BLE_GATTS_EVT_WRITE:
            SERV_DIS_OnWrite(handle, bleEvent);
            break;

        default:
            // No implementation needed.
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
static void SERV_DIS_OnConnect(struct TXW51_SERV_DIS_Handle *handle,
                               ble_evt_t *bleEvent)
{
    TXW51_LOG_DEBUG("[DIS Service] Connected");
}


/***************************************************************************//**
* @brief Handles the disconnection event.
*
* @param[in,out] handle   The handle for the service.
* @param[in]     bleEvent The BLE event that occurred.
* @return Nothing.
******************************************************************************/
static void SERV_DIS_OnDisconnect(struct TXW51_SERV_DIS_Handle *handle,
                                  ble_evt_t *bleEvent)
{
    TXW51_LOG_DEBUG("[DIS Service] Disconnected");
}


/***************************************************************************//**
* @brief Handles the write event.
*
* Detects which characteristic has been written to and notifies the application.
*
* @param[in,out] handle   The handle for the service.
* @param[in]     bleEvent The BLE event that occurred.
* @return Nothing.
******************************************************************************/
static void SERV_DIS_OnWrite(struct TXW51_SERV_DIS_Handle *handle,
                             ble_evt_t *bleEvent)
{
    ble_gatts_evt_write_t *writeEvt = &bleEvent->evt.gatts_evt.params.write;

    if (handle->EventHandler == NULL) {
        return;
    }

    struct TXW51_SERV_DIS_Event evt;
    evt.EventType = TXW51_SERV_DIS_EVT_UNKNOWN;

    if (writeEvt->handle == handle->CharHandle_Manufacturer.value_handle) {
        evt.EventType = TXW51_SERV_DIS_EVT_UPDATE_MANUFACTURER;

    } else if (writeEvt->handle == handle->CharHandle_Model.value_handle) {
        evt.EventType = TXW51_SERV_DIS_EVT_UPDATE_MODEL;

    } else if (writeEvt->handle == handle->CharHandle_Serial.value_handle) {
        evt.EventType = TXW51_SERV_DIS_EVT_UPDATE_SERIAL;

    } else if (writeEvt->handle == handle->CharHandle_HwRev.value_handle) {
        evt.EventType = TXW51_SERV_DIS_EVT_UPDATE_HW_REV;

    } else if (writeEvt->handle == handle->CharHandle_FwRev.value_handle) {
        evt.EventType = TXW51_SERV_DIS_EVT_UPDATE_FW_REV;

    } else if (writeEvt->handle == handle->CharHandle_DeviceName.value_handle) {
        evt.EventType = TXW51_SERV_DIS_EVT_UPDATE_DEVICE_NAME;

    } else if (writeEvt->handle == handle->CharHandle_SaveValues.value_handle) {
        evt.EventType = TXW51_SERV_DIS_EVT_SAVE_VALUES;

    }else if (writeEvt->handle == handle->CharHandle_DisableTimer.value_handle) {
        evt.EventType = TXW51_SERV_DIS_EVT_DISABLE_TIMER;

    }

    if (evt.EventType != TXW51_SERV_DIS_EVT_UNKNOWN) {
        evt.Length = writeEvt->len + 1;
        uint8_t dataBuffer[evt.Length];
        strlcpy((char *)dataBuffer, (char *)writeEvt->data, evt.Length);
        evt.Value = dataBuffer;

        sd_ble_gatts_value_set(writeEvt->handle, 0, &evt.Length, evt.Value);
        handle->EventHandler(handle, &evt);
    }
}


/***************************************************************************//**
* @brief Adds all the different characteristics to the service.
*
* @param[in,out] serviceHandle The handle for the service.
* @param[in]     init          Structure with the initialization values.
* @return ERR_NONE if no error occurred.
*         ERR_BLE_SERVICE_ADD_CHARACTERISTIC if characteristic could not be
*                                            added.
******************************************************************************/
static uint32_t SERV_DIS_AddAllChars(struct TXW51_SERV_DIS_Handle *serviceHandle,
                                     const struct TXW51_SERV_DIS_Init *init)
{
    uint32_t err;

    err = SERV_DIS_AddChar(serviceHandle,
                           TXW51_SERV_DIS_UUID_CHAR_MANUFACTURER,
                           init->String_Manufacturer,
                           TXW51_SERV_DIS_STRING_CHAR_MANUFACTURER,
                           &serviceHandle->CharHandle_Manufacturer);
    if (err != ERR_NONE) {
        return err;
    }

    err = SERV_DIS_AddChar(serviceHandle,
                           TXW51_SERV_DIS_UUID_CHAR_MODEL,
                           init->String_Model,
                           TXW51_SERV_DIS_STRING_CHAR_MODEL,
                           &serviceHandle->CharHandle_Model);
    if (err != ERR_NONE) {
        return err;
    }

    err = SERV_DIS_AddChar(serviceHandle,
                           TXW51_SERV_DIS_UUID_CHAR_SERIAL,
                           init->String_Serial,
                           TXW51_SERV_DIS_STRING_CHAR_SERIAL,
                           &serviceHandle->CharHandle_Serial);
    if (err != ERR_NONE) {
        return err;
    }

    err = SERV_DIS_AddChar(serviceHandle,
                           TXW51_SERV_DIS_UUID_CHAR_HW_REV,
                           init->String_HwRev,
                           TXW51_SERV_DIS_STRING_CHAR_HW_REV,
                           &serviceHandle->CharHandle_HwRev);
    if (err != ERR_NONE) {
        return err;
    }

    err = SERV_DIS_AddChar(serviceHandle,
                           TXW51_SERV_DIS_UUID_CHAR_FW_REV,
                           init->String_FwRev,
                           TXW51_SERV_DIS_STRING_CHAR_FW_REV,
                           &serviceHandle->CharHandle_FwRev);
    if (err != ERR_NONE) {
        return err;
    }

    err = SERV_DIS_AddChar(serviceHandle,
                           TXW51_SERV_DIS_UUID_CHAR_DEVICE_NAME,
                           init->String_DeviceName,
                           TXW51_SERV_DIS_STRING_CHAR_DEVICE_NAME,
                           &serviceHandle->CharHandle_DeviceName);
    if (err != ERR_NONE) {
        return err;
    }

    err = SERV_DIS_AddChar_SaveValues(serviceHandle);
    if (err != ERR_NONE) {
        return err;
    }


    err = SERV_DIS_AddChar_Timer(serviceHandle);
    if (err != ERR_NONE) {
        return err;
    }

    return ERR_NONE;
}


/***************************************************************************//**
* @brief Adds a characteristic that contains a string to the service.
*
* @param[in,out] serviceHandle The handle for the service.
* @param[in]     uuid          The two UUID bytes for the characteristic.
* @param[in]     charValue     The initial string value.
* @param[in]     description   The user description of the characteristic.
* @param[out]    charHandle    The handle for the characteristic.
* @return ERR_NONE if no error occurred.
*         ERR_BLE_SERVICE_ADD_CHARACTERISTIC if characteristic could not be
*                                            added.
******************************************************************************/
static uint32_t SERV_DIS_AddChar(struct TXW51_SERV_DIS_Handle *serviceHandle,
                                 uint16_t uuid,
                                 uint8_t *charValue,
                                 char *description,
                                 ble_gatts_char_handles_t *charHandle)
{
    struct TXW51_SERV_CharInit charInit;

    /* Initialize characteristic. */
    TXW51_SERV_InitChar(&serviceHandle->ServiceHandle, uuid, &charInit);

    /* Set up characteristic. */
    charInit.Metadata.char_props.read  = 1;
    charInit.Metadata.char_props.write = 1;
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&charInit.AttrMetadata.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&charInit.AttrMetadata.write_perm);

    /*add_desc_user_description(&charInit, (uint8_t *)description);*/

    uint16_t length = MIN(strlen((char *)charValue), TXW51_SERV_DIS_VALUE_MAX_LENGTH);
    charInit.Attribute.init_len = length;
    charInit.Attribute.max_len  = TXW51_SERV_DIS_VALUE_MAX_LENGTH;
    charInit.Attribute.p_value  = charValue;

    /* Add characteristic. */
    return TXW51_SERV_AddChar(&serviceHandle->ServiceHandle,
                              &charInit,
                              charHandle);
}


/***************************************************************************//**
* @brief Adds the "Save Values" characteristic to the service.
*
* @param[in,out] serviceHandle The handle for the service.
* @return ERR_NONE if no error occurred.
*         ERR_BLE_SERVICE_ADD_CHARACTERISTIC if characteristic could not be
*                                            added.
******************************************************************************/
static uint32_t SERV_DIS_AddChar_SaveValues(struct TXW51_SERV_DIS_Handle *serviceHandle)
{
    struct TXW51_SERV_CharInit charInit;

    /* Initialize characteristic. */
    TXW51_SERV_InitChar(&serviceHandle->ServiceHandle,
                      TXW51_SERV_DIS_UUID_CHAR_SAVE_VALUES, &charInit);

    /* Set up characteristic. */
    charInit.Metadata.char_props.read  = 0;
    charInit.Metadata.char_props.write = 1;
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&charInit.AttrMetadata.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&charInit.AttrMetadata.write_perm);

    /*add_desc_user_description(&charInit, (uint8_t *)TXW51_SERV_DIS_STRING_CHAR_SAVE_VALUES);*/

    charInit.Attribute.init_len = 1;
    charInit.Attribute.max_len  = 1;

    /* Add characteristic. */
    return TXW51_SERV_AddChar(&serviceHandle->ServiceHandle,
                              &charInit,
                              &serviceHandle->CharHandle_SaveValues);
}

/***************************************************************************//**
* @brief Adds the "Disable Timer" characteristic to the service.
*
* @param[in,out] serviceHandle The handle for the service.
* @return ERR_NONE if no error occurred.
*         ERR_BLE_SERVICE_ADD_CHARACTERISTIC if characteristic could not be
*                                            added.
******************************************************************************/
static uint32_t SERV_DIS_AddChar_Timer(struct TXW51_SERV_DIS_Handle *serviceHandle)
{
    struct TXW51_SERV_CharInit charInit;

    /* Initialize characteristic. */
    TXW51_SERV_InitChar(&serviceHandle->ServiceHandle,
                      TXW51_SERV_DIS_UUID_CHAR_DISABLE_TIMER, &charInit);

    /* Set up characteristic. */
    charInit.Metadata.char_props.read  = 1;
    charInit.Metadata.char_props.write = 1;
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&charInit.AttrMetadata.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&charInit.AttrMetadata.write_perm);

    /*add_desc_user_description(&charInit, (uint8_t *)TXW51_SERV_DIS_STRING_CHAR_DISABLE_TIMER);*/

    charInit.Attribute.init_len = 1;
    charInit.Attribute.max_len  = 1;
    charInit.Attribute.p_value = 0;

    /* Add characteristic. */
    return TXW51_SERV_AddChar(&serviceHandle->ServiceHandle,
                              &charInit,
                              &serviceHandle->CharHandle_DisableTimer);
}
