/***************************************************************************//**
 * @brief   Bluetooth Smart "I2C BLE Bridge".
 *
 * This service allows the user to configure and to transmit i2c messages with
 * the TXW51 sensor.
 *
 * @file    service_i2c.c
 * @version 1.0
 * @date    08.05.2015
 * @author  Pascal Bohni
 *
 * @remark  Last Modifications:
 *          10.04.2015 bohnp1 created
 ******************************************************************************/

/*----- Header-Files ---------------------------------------------------------*/
#include "service_i2c.h"

#include <string.h>

#include "nrf/nordic_common.h"

#include "txw51_framework/config/config_services.h"
#include "txw51_framework/utils/log.h"
#include "txw51_framework/utils/txw51_errors.h"

/*----- Macros ---------------------------------------------------------------*/

/*----- Data types -----------------------------------------------------------*/

/*----- Function prototypes --------------------------------------------------*/
static void SERV_I2C_OnConnect(struct TXW51_SERV_I2C_Handle *handle,
                                  ble_evt_t *bleEvent);
static void SERV_I2C_OnDisconnect(struct TXW51_SERV_I2C_Handle *handle,
                                     ble_evt_t *bleEvent);
static void SERV_I2C_OnWrite(struct TXW51_SERV_I2C_Handle *handle,
                                ble_evt_t *bleEvent);
static void SERV_I2C_OnRwAuthRequest(struct TXW51_SERV_I2C_Handle *handle,
                                        ble_evt_t *bleEvent);
static uint32_t SERV_I2C_AddAllChars(struct TXW51_SERV_I2C_Handle *serviceHandle);
static uint32_t SERV_I2C_AddChar(struct TXW51_SERV_I2C_Handle *serviceHandle,
                                    uint16_t uuid,
                                    uint8_t charValue,
                                    char *description,
                                    ble_gatts_char_handles_t *charHandle);
//static uint32_t SERV_I2C_AddChar_TriggerValue(struct TXW51_SERV_I2C_Handle *serviceHanI2C Data -----------------------------------------------------------------*/

/*----- Implementation -------------------------------------------------------*/

uint32_t TXW51_SERV_I2C_Init(struct TXW51_SERV_I2C_Handle *handle,
		                        const struct TXW51_SERV_I2C_Init *init)
{
    uint32_t err;

    handle->EventHandler = init->EventHandler;

    struct TXW51_SERV_ServiceInit serviceInit = {
        .BaseUuid    = TXW51_SERVICE_UUID_BASE,
        .ServiceUuid = SERVICE_I2C_UUID_SERVICE,
        .IsSecondary = false
    };

    err = TXW51_SERV_Create(&serviceInit, &handle->ServiceHandle);
    if (err != ERR_NONE) {
        return err;
    }

    err = SERV_I2C_AddAllChars(handle);
    if (err != ERR_NONE) {
        TXW51_LOG_ERROR("[I2C Service] Could not create all characteristics.");
        return err;
    }

    return ERR_NONE;
}


void TXW51_SERV_I2C_OnBleEvent(struct TXW51_SERV_I2C_Handle *handle,
                                  ble_evt_t *bleEvent)
{
    TXW51_SERV_OnBleEvent(&handle->ServiceHandle, bleEvent);


    switch (bleEvent->header.evt_id) {
        case BLE_GAP_EVT_CONNECTED:
            SERV_I2C_OnConnect(handle, bleEvent);
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            SERV_I2C_OnDisconnect(handle, bleEvent);
            break;

        case BLE_GATTS_EVT_WRITE:
            SERV_I2C_OnWrite(handle, bleEvent);
            break;

        case BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST:
            SERV_I2C_OnRwAuthRequest(handle, bleEvent);
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
static void SERV_I2C_OnConnect(struct TXW51_SERV_I2C_Handle *handle,
                                  ble_evt_t *bleEvent)
{
    TXW51_LOG_DEBUG("[I2C Service] Connected");
}


/***************************************************************************//**
* @brief Handles the disconnection event.
*
* @param[in,out] handle   The handle for the service.
* @param[in]     bleEvent The BLE event that occurred.
* @return Nothing.
******************************************************************************/
static void SERV_I2C_OnDisconnect(struct TXW51_SERV_I2C_Handle *handle,
                                     ble_evt_t *bleEvent)
{
    TXW51_LOG_DEBUG("[I2C Service] Disconnected");
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
static void SERV_I2C_OnWrite(struct TXW51_SERV_I2C_Handle *handle,
                                ble_evt_t *bleEvent)
{
	ble_gatts_evt_write_t *evtWrite = &bleEvent->evt.gatts_evt.params.write;

	if (handle->EventHandler != NULL) {
	    struct TXW51_SERV_I2C_Event evt;
	    evt.EventType = TXW51_SERV_I2C_EVT_UNKNOWN;

	    if (evtWrite->handle == handle->CharHandle_I2CValue.value_handle) {
	        evt.EventType = TXW51_SERV_I2C_EVT_VALUE_WRITE;

	    } else if (evtWrite->handle == handle->CharHandle_I2CAddress.value_handle) {
            evt.EventType = TXW51_SERV_I2C_EVT_ADRESS;

	    } else if (evtWrite->handle == handle->CharHandle_I2CRegister.value_handle) {
            evt.EventType = TXW51_SERV_I2C_EVT_REGISTER;

	    }

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
void SERV_I2C_OnRwAuthRequest(struct TXW51_SERV_I2C_Handle *handle,
                                 ble_evt_t *bleEvent)
{
    ble_gatts_evt_rw_authorize_request_t *authRequest = &bleEvent->evt.gatts_evt.params.authorize_request;

    TXW51_LOG_DEBUG("SERV_I2C_OnRwAuthRequest");

    if (handle->EventHandler != NULL) {
        struct TXW51_SERV_I2C_Event evt;

        if ((authRequest->type = BLE_GATTS_AUTHORIZE_TYPE_READ) &&
            (authRequest->request.read.handle == handle->CharHandle_I2CValue.value_handle)) {
            uint32_t err;

            /* Handle event. */
            uint8_t tempValue = 0;
            evt.EventType = TXW51_SERV_I2C_EVT_VALUE_READ;
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
                TXW51_LOG_WARNING("[I2C Service] Error I2C Value Read!");
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
static uint32_t SERV_TEMP_CONTACTLESS_AddAllChars(struct TXW51_SERV_I2C_Handle *serviceHandle)
{
    uint32_t err;

    err = SERV_I2C_AddChar(serviceHandle,
                              SERVICE_I2C_UUID_CHAR_REGISTER_VALUE,
                              0,
                              SERVICE_I2C_STRING_CHAR_REGISTER_VALUE,
                              &serviceHandle->CharHandle_I2CRegister);
    if (err != ERR_NONE) {
        return err;
    }

    err = SERV_I2C_AddChar(serviceHandle,
                              SERVICE_I2C_UUID_CHAR_ADDRESS,
                              0,
                              SERVICE_I2C_STRING_CHAR_ADDRESS,
                              &serviceHandle->CharHandle_I2CAddress);
    if (err != ERR_NONE) {
        return err;
    }

	err = SERV_I2C_AddChar(serviceHandle,
							  SERVICE_I2C_UUID_CHAR_REGISTER,
							  0,
							  SERVICE_I2C_STRING_CHAR_REGISTER,
							  &serviceHandle->CharHandle_I2CRegister);
	if (err != ERR_NONE) {
		return err;
	}

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
static uint32_t SERV_I2C_AddChar(struct TXW51_SERV_I2C_Handle *serviceHandle,
                                    uint16_t uuid,
                                    uint8_t charValue,
                                    char *description,
                                    ble_gatts_char_handles_t *charHandle)
{
    struct TXW51_SERV_CharInit charInit;

    /* Initialize characteristic. */
    TXW51_SERV_InitChar(&serviceHandle->ServiceHandle,
                        uuid,
                        &charInit);

    /* Set up characteristic. */
    charInit.Metadata.char_props.read  = 1;
    charInit.Metadata.char_props.write = 1;
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&charInit.AttrMetadata.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&charInit.AttrMetadata.write_perm);

    /*add_desc_user_description(&charInit, (uint8_t *)description);*/

    charInit.Attribute.init_len = 1;
    charInit.Attribute.max_len  = 1;
    charInit.Attribute.p_value  = &charValue;

    /* Add characteristic. */
    return TXW51_SERV_AddChar(&serviceHandle->ServiceHandle,
                              &charInit,
                              charHandle);
}
