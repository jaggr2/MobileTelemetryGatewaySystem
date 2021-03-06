/***************************************************************************//**
 * @brief   Bluetooth Smart "Contact Less Temperature Measurment".
 *
 * This service allows the user to configure and start a new contact less temperature measurement with
 * the TXW51 sensor.
 *
 * @file    service_tempContactLess.h
 * @version 1.0
 * @date    10.04.2015
 * @author  Pascal Bohni
 *
 * @remark  Last Modifications:
 *          10.04.2015 bohnp1 created
 ******************************************************************************/

/*----- Header-Files ---------------------------------------------------------*/
#include "service_tempContactless.h"

#include <string.h>

#include "nrf/nordic_common.h"

#include "txw51_framework/config/config_services.h"
#include "txw51_framework/utils/log.h"
#include "txw51_framework/utils/txw51_errors.h"

/*----- Macros ---------------------------------------------------------------*/

/*----- Data types -----------------------------------------------------------*/

/*----- Function prototypes --------------------------------------------------*/
static void SERV_TEMP_CONTACTLESS_OnConnect(struct TXW51_SERV_TEMP_CONTACTLESS_Handle *handle,
                                  ble_evt_t *bleEvent);
static void SERV_TEMP_CONTACTLESS_OnDisconnect(struct TXW51_SERV_TEMP_CONTACTLESS_Handle *handle,
                                     ble_evt_t *bleEvent);
static void SERV_TEMP_CONTACTLESS_OnWrite(struct TXW51_SERV_TEMP_CONTACTLESS_Handle *handle,
                                ble_evt_t *bleEvent);
static void SERV_TEMP_CONTACTLESS_OnRwAuthRequest(struct TXW51_SERV_TEMP_CONTACTLESS_Handle *handle,
                                        ble_evt_t *bleEvent);
static uint32_t SERV_TEMP_CONTACTLESS_AddAllChars(struct TXW51_SERV_TEMP_CONTACTLESS_Handle *serviceHandle);
static uint32_t SERV_TEMP_CONTACTLESS_AddChar(struct TXW51_SERV_TEMP_CONTACTLESS_Handle *serviceHandle,
                                    uint16_t uuid,
                                    uint8_t charValue,
                                    char *description,
                                    ble_gatts_char_handles_t *charHandle);
static uint32_t SERV_TEMP_CONTACTLESS_AddChar_TempSample(struct TXW51_SERV_TEMP_CONTACTLESS_Handle *serviceHandle);
//static uint32_t SERV_TEMP_CONTACTLESS_AddChar_TriggerValue(struct TXW51_SERV_TEMP_CONTACTLESS_Handle *serviceHandle);

/*----- Data -----------------------------------------------------------------*/

/*----- Implementation -------------------------------------------------------*/

uint32_t TXW51_SERV_TEMP_CONTACTLESS_Init(struct TXW51_SERV_TEMP_CONTACTLESS_Handle *handle,
		                        const struct TXW51_SERV_TEMP_CONTACTLESS_Init *init)
{
    uint32_t err;

    handle->EventHandler = init->EventHandler;

    struct TXW51_SERV_ServiceInit serviceInit = {
        .BaseUuid    = TXW51_SERVICE_UUID_BASE,
        .ServiceUuid = SERVICE_TEMP_CONTACTLESS_UUID_SERVICE,
        .IsSecondary = false
    };

    err = TXW51_SERV_Create(&serviceInit, &handle->ServiceHandle);
    if (err != ERR_NONE) {
        return err;
    }

    err = SERV_TEMP_CONTACTLESS_AddAllChars(handle);
    if (err != ERR_NONE) {
        TXW51_LOG_ERROR("[TEMP_CONTACTLESS Service] Could not create all characteristics.");
        return err;
    }

    TXW51_LOG_INFO("[TEMP_CONTACTLESS Service] Service creation successful");
    return ERR_NONE;
}


void TXW51_SERV_TEMP_CONTACTLESS_OnBleEvent(struct TXW51_SERV_TEMP_CONTACTLESS_Handle *handle,
                                  ble_evt_t *bleEvent)
{
    TXW51_SERV_OnBleEvent(&handle->ServiceHandle, bleEvent);


    switch (bleEvent->header.evt_id) {
        case BLE_GAP_EVT_CONNECTED:
            SERV_TEMP_CONTACTLESS_OnConnect(handle, bleEvent);
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            SERV_TEMP_CONTACTLESS_OnDisconnect(handle, bleEvent);
            break;

        case BLE_GATTS_EVT_WRITE:
            SERV_TEMP_CONTACTLESS_OnWrite(handle, bleEvent);
            break;

        case BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST:
            SERV_TEMP_CONTACTLESS_OnRwAuthRequest(handle, bleEvent);
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
static void SERV_TEMP_CONTACTLESS_OnConnect(struct TXW51_SERV_TEMP_CONTACTLESS_Handle *handle,
                                  ble_evt_t *bleEvent)
{
    TXW51_LOG_DEBUG("[TEMP_CONTACTLESS Service] Connected");
}


/***************************************************************************//**
* @brief Handles the disconnection event.
*
* @param[in,out] handle   The handle for the service.
* @param[in]     bleEvent The BLE event that occurred.
* @return Nothing.
******************************************************************************/
static void SERV_TEMP_CONTACTLESS_OnDisconnect(struct TXW51_SERV_TEMP_CONTACTLESS_Handle *handle,
                                     ble_evt_t *bleEvent)
{
    TXW51_LOG_DEBUG("[TEMP_CONTACTLESS Service] Disconnected");
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
static void SERV_TEMP_CONTACTLESS_OnWrite(struct TXW51_SERV_TEMP_CONTACTLESS_Handle *handle,
                                ble_evt_t *bleEvent)
{
	ble_gatts_evt_write_t *evtWrite = &bleEvent->evt.gatts_evt.params.write;

	if (handle->EventHandler != NULL) {
	    struct TXW51_SERV_TEMP_CONTACTLESS_Event evt;
	    evt.EventType = TXW51_SERV_TEMP_CONTACTLESS_EVT_UNKNOWN;


	    if (evt.EventType != TXW51_SERV_TEMP_CONTACTLESS_EVT_UNKNOWN) {
	        evt.Value = evtWrite->data;
            evt.Length = evtWrite->len;
            handle->EventHandler(handle, &evt);
	    }
	}
}


/***************************************************************************//**
* @brief Handles the read/write authorization request.
*
* Detects which characteristic has been read, notifies the application and
* allows the request. This can be used to act on characteristics reads.
*
* @param[in,out] handle   The handle for the service.
* @param[in]     bleEvent The BLE event that occurred.
* @return Nothing.
******************************************************************************/
void SERV_TEMP_CONTACTLESS_OnRwAuthRequest(struct TXW51_SERV_TEMP_CONTACTLESS_Handle *handle,
                                 ble_evt_t *bleEvent)
{
    ble_gatts_evt_rw_authorize_request_t *authRequest = &bleEvent->evt.gatts_evt.params.authorize_request;

    TXW51_LOG_DEBUG("SERV_TEMP_CONTACTLESS_OnRwAuthRequest");

    if (handle->EventHandler != NULL) {
        struct TXW51_SERV_TEMP_CONTACTLESS_Event evt;

        if ((authRequest->type = BLE_GATTS_AUTHORIZE_TYPE_READ) &&
            (authRequest->request.read.handle == handle->CharHandle_TempSample.value_handle)) {
            uint32_t err;

            /* Handle event. */
            uint8_t tempValue = 0;
            evt.EventType = TXW51_SERV_TEMP_CONTACTLESS_EVT_TEMP_SAMPLE;
            evt.Value = &tempValue;
            handle->EventHandler(handle, &evt);

            /* Reply to peer. */
            ble_gatts_rw_authorize_reply_params_t reply;
            reply.type = BLE_GATTS_AUTHORIZE_TYPE_READ;
            reply.params.read.gatt_status = BLE_GATT_STATUS_SUCCESS;
            reply.params.read.p_data = &tempValue;
            reply.params.read.len = 1;
            reply.params.read.update = 1;
            reply.params.read.offset = 0;

            err = sd_ble_gatts_rw_authorize_reply(bleEvent->evt.gatts_evt.conn_handle, &reply);
            if (err != NRF_SUCCESS) {
                TXW51_LOG_WARNING("[TEMP_CONTACTLESS Service] Temperature read failed!");
            }
        }
    }
}


/***************************************************************************//**
* @brief Adds all the different characteristics to the service.
*
* @param[in,out] serviceHandle The handle for the service.
* @return ERR_NONE if no error occurred.
*         ERR_BLE_SERVICE_ADD_CHARACTERISTIC if characteristic could not be
*                                            added.
******************************************************************************/
static uint32_t SERV_TEMP_CONTACTLESS_AddAllChars(struct TXW51_SERV_TEMP_CONTACTLESS_Handle *serviceHandle)
{
    uint32_t err;

//    err = SERV_TEMP_CONTACTLESS_AddChar(serviceHandle,
//                              SERVICE_TEMP_CONTACTLESS_UUID_CHAR_ACC_EN,
//                              0,
//                              SERVICE_TEMP_CONTACTLESS_STRING_CHAR_ACC_EN,
//                              &serviceHandle->CharHandle_AccEnable);
//    if (err != ERR_NONE) {
//        return err;
//    }

//    err = SERV_TEMP_CONTACTLESS_AddChar(serviceHandle,
//                              SERVICE_TEMP_CONTACTLESS_UUID_CHAR_GYRO_EN,
//                              0,
//                              SERVICE_TEMP_CONTACTLESS_STRING_CHAR_GYRO_EN,
//                              &serviceHandle->CharHandle_GyroEnable);
//    if (err != ERR_NONE) {
//        return err;
//    }

    err = SERV_TEMP_CONTACTLESS_AddChar_TempSample(serviceHandle);
    if (err != ERR_NONE) {
        return err;
    }

//    err = SERV_TEMP_CONTACTLESS_AddChar(serviceHandle,
//                              SERVICE_TEMP_CONTACTLESS_UUID_CHAR_ACC_FSCALE,
//                              0,
//                              SERVICE_TEMP_CONTACTLESS_STRING_CHAR_ACC_FSCALE,
//                              &serviceHandle->CharHandle_AccFscale);
//    if (err != ERR_NONE) {
//        return err;
//    }
//
//    err = SERV_TEMP_CONTACTLESS_AddChar(serviceHandle,
//                              SERVICE_TEMP_CONTACTLESS_UUID_CHAR_GYRO_FSCALE,
//                              0,
//                              SERVICE_TEMP_CONTACTLESS_STRING_CHAR_GYRO_FSCALE,
//                              &serviceHandle->CharHandle_GyroFscale);
//    if (err != ERR_NONE) {
//        return err;
//    }
//
//    err = SERV_TEMP_CONTACTLESS_AddChar(serviceHandle,
//                              SERVICE_TEMP_CONTACTLESS_UUID_CHAR_ACC_ODR,
//                              0,
//                              SERVICE_TEMP_CONTACTLESS_STRING_CHAR_ACC_ODR,
//                              &serviceHandle->CharHandle_AccOdr);
//    if (err != ERR_NONE) {
//        return err;
//    }
//
//    err = SERV_TEMP_CONTACTLESS_AddChar(serviceHandle,
//                              SERVICE_TEMP_CONTACTLESS_UUID_CHAR_GYRO_ODR,
//                              0,
//                              SERVICE_TEMP_CONTACTLESS_STRING_CHAR_GYRO_ODR,
//                              &serviceHandle->CharHandle_GyroOdr);
//    if (err != ERR_NONE) {
//        return err;
//    }
//
//    err = SERV_TEMP_CONTACTLESS_AddChar_TriggerValue(serviceHandle);
//    if (err != ERR_NONE) {
//        return err;
//    }
//
//    err = SERV_TEMP_CONTACTLESS_AddChar(serviceHandle,
//                              SERVICE_TEMP_CONTACTLESS_UUID_CHAR_TRIGGER_AXIS,
//                              0,
//                              SERVICE_TEMP_CONTACTLESS_STRING_CHAR_TRIGGER_AXIS,
//                              &serviceHandle->CharHandle_TriggerAxis);
//    if (err != ERR_NONE) {
//        return err;
//    }

    return ERR_NONE;
}


/***************************************************************************//**
* @brief Adds a characteristic that contains a 1 byte value to the service.
*
* @param[in,out] serviceHandle The handle for the service.
* @param[in]     uuid          The two UUID bytes for the characteristic.
* @param[in]     charValue     The initial value.
* @param[in]     description   The user description of the characteristic.
* @param[out]    charHandle    The handle for the characteristic.
* @return ERR_NONE if no error occurred.
*         ERR_BLE_SERVICE_ADD_CHARACTERISTIC if characteristic could not be
*                                            added.
******************************************************************************/
//static uint32_t SERV_TEMP_CONTACTLESS_AddChar(struct TXW51_SERV_TEMP_CONTACTLESS_Handle *serviceHandle,
//                                    uint16_t uuid,
//                                    uint8_t charValue,
//                                    char *description,
//                                    ble_gatts_char_handles_t *charHandle)
//{
//    struct TXW51_SERV_CharInit charInit;
//
//    /* Initialize characteristic. */
//    TXW51_SERV_InitChar(&serviceHandle->ServiceHandle,
//                        uuid,
//                        &charInit);
//
//    /* Set up characteristic. */
//    charInit.Metadata.char_props.read  = 1;
//    charInit.Metadata.char_props.write = 1;
//    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&charInit.AttrMetadata.read_perm);
//    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&charInit.AttrMetadata.write_perm);
//
//    /*add_desc_user_description(&charInit, (uint8_t *)description);*/
//
//    charInit.Attribute.init_len = 1;
//    charInit.Attribute.max_len  = 1;
//    charInit.Attribute.p_value  = &charValue;
//
//    /* Add characteristic. */
//    return TXW51_SERV_AddChar(&serviceHandle->ServiceHandle,
//                              &charInit,
//                              charHandle);
//}


/***************************************************************************//**
* @brief Adds the "Temperature Sample" characteristic to the service.
*
* @param[in,out] serviceHandle The handle for the service.
* @return ERR_NONE if no error occurred.
*         ERR_BLE_SERVICE_ADD_CHARACTERISTIC if characteristic could not be
*                                            added.
******************************************************************************/
static uint32_t SERV_TEMP_CONTACTLESS_AddChar_TempSample(struct TXW51_SERV_TEMP_CONTACTLESS_Handle *serviceHandle)
{
    struct TXW51_SERV_CharInit charInit;

    /* Initialize characteristic. */
    TXW51_SERV_InitChar(&serviceHandle->ServiceHandle,
                        SERVICE_TEMP_CONTACTLESS_UUID_CHAR_TEMP_SAMPLE,
                        &charInit);

    /* Set up characteristic. */
    charInit.Metadata.char_props.read = 1;
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&charInit.AttrMetadata.read_perm);
    charInit.AttrMetadata.rd_auth = 1;

    /*add_desc_user_description(&charInit, (uint8_t *)SERVICE_TEMP_CONTACTLESS_STRING_CHAR_TEMP_SAMPLE);*/

    charInit.Attribute.max_len = 1;

    /* Add characteristic. */
    return TXW51_SERV_AddChar(&serviceHandle->ServiceHandle,
                              &charInit,
                              &serviceHandle->CharHandle_TempSample);
}


/***************************************************************************//**
* @brief Adds the "Trigger Value" characteristic to the service.
*
* @param[in,out] serviceHandle The handle for the service.
* @return ERR_NONE if no error occurred.
*         ERR_BLE_SERVICE_ADD_CHARACTERISTIC if characteristic could not be
*                                            added.
******************************************************************************/
//static uint32_t SERV_TEMP_CONTACTLESS_AddChar_TriggerValue(struct TXW51_SERV_TEMP_CONTACTLESS_Handle *serviceHandle)
//{
//    struct TXW51_SERV_CharInit charInit;
//
//    /* Initialize characteristic. */
//    TXW51_SERV_InitChar(&serviceHandle->ServiceHandle,
//                        SERVICE_TEMP_CONTACTLESS_UUID_CHAR_TRIGGER_VAL,
//                        &charInit);
//
//    /* Set up characteristic. */
//    charInit.Metadata.char_props.read  = 1;
//    charInit.Metadata.char_props.write = 1;
//    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&charInit.AttrMetadata.read_perm);
//    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&charInit.AttrMetadata.write_perm);
//
//    /*add_desc_user_description(&charInit, (uint8_t *)SERVICE_TEMP_CONTACTLESS_STRING_CHAR_TRIGGER_VAL);*/
//
//    charInit.Attribute.max_len = 2;
//
//    /* Add characteristic. */
//    return TXW51_SERV_AddChar(&serviceHandle->ServiceHandle,
//                              &charInit,
//                              &serviceHandle->CharHandle_TriggerValue);
//}

