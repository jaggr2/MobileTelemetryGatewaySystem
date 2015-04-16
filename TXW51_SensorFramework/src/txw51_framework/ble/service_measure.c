/***************************************************************************//**
 * @brief   Bluetooth Smart "Measurement Service".
 *
 * This service allows the user to configure and start a new measurement with
 * the TXW51 sensor.
 *
 * @file    service_measure.c
 * @version 1.0
 * @date    09.12.2014
 * @author  Daniel Meer
 *
 * @remark  Last Modifications:
 *          09.12.2014 meerd1 created
 ******************************************************************************/

/*----- Header-Files ---------------------------------------------------------*/
#include "service_measure.h"

#include <stdint.h>
#include <string.h>

#include "txw51_framework/config/config_services.h"
#include "txw51_framework/utils/log.h"
#include "txw51_framework/utils/txw51_errors.h"

/*----- Macros ---------------------------------------------------------------*/

/*----- Data types -----------------------------------------------------------*/

/*----- Function prototypes --------------------------------------------------*/
static void SERV_MEASURE_OnConnect(struct TXW51_SERV_MEASURE_Handle *handle,
                                   ble_evt_t *bleEvent);
static void SERV_MEASURE_OnDisconnect(struct TXW51_SERV_MEASURE_Handle *handle,
                                      ble_evt_t *bleEvent);
static void SERV_MEASURE_OnWrite(struct TXW51_SERV_MEASURE_Handle *handle,
                                 ble_evt_t *bleEvent);
static void SERV_MEASURE_OnHvc(struct TXW51_SERV_MEASURE_Handle *handle,
                               ble_evt_t *bleEvent);
static void SERV_MEASURE_OnTxComplete(struct TXW51_SERV_MEASURE_Handle *handle,
                                      ble_evt_t *bleEvent);
static uint32_t SERV_MEASURE_AddAllChars(struct TXW51_SERV_MEASURE_Handle *serviceHandle);
static uint32_t SERV_MEASURE_AddChar_Start(struct TXW51_SERV_MEASURE_Handle *serviceHandle);
static uint32_t SERV_MEASURE_AddChar_Stop(struct TXW51_SERV_MEASURE_Handle *serviceHandle);
static uint32_t SERV_MEASURE_AddChar_Duration(struct TXW51_SERV_MEASURE_Handle *serviceHandle);
static uint32_t SERV_MEASURE_AddChar_DataStream(struct TXW51_SERV_MEASURE_Handle *serviceHandle);

/*----- Data -----------------------------------------------------------------*/

/*----- Implementation -------------------------------------------------------*/

uint32_t TXW51_SERV_MEASURE_Init(struct TXW51_SERV_MEASURE_Handle *handle,
                                 const struct TXW51_SERV_MEASURE_Init *init)
{
    uint32_t err;

    handle->EventHandler = init->EventHandler;

    struct TXW51_SERV_ServiceInit serviceInit = {
        .BaseUuid    = TXW51_SERVICE_UUID_BASE,
        .ServiceUuid = TXW51_SERV_MEASURE_UUID_SERVICE,
        .IsSecondary = false
    };

    err = TXW51_SERV_Create(&serviceInit, &handle->ServiceHandle);
    if (err != ERR_NONE) {
        TXW51_LOG_ERROR("[Measure Service] Service creation failed.");
        return err;
    }

    err = SERV_MEASURE_AddAllChars(handle);
    if (err != ERR_NONE) {
        TXW51_LOG_ERROR("[Measure Service] Could not create all characteristics.");
        return err;
    }

    TXW51_LOG_INFO("[Measure Service] Service creation successful.");
    return ERR_NONE;
}


void TXW51_SERV_MEASURE_OnBleEvent(struct TXW51_SERV_MEASURE_Handle *handle,
                                   ble_evt_t *bleEvent)
{
    TXW51_SERV_OnBleEvent(&handle->ServiceHandle, bleEvent);

    switch (bleEvent->header.evt_id) {
        case BLE_GAP_EVT_CONNECTED:
            SERV_MEASURE_OnConnect(handle, bleEvent);
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            SERV_MEASURE_OnDisconnect(handle, bleEvent);
            break;

        case BLE_GATTS_EVT_WRITE:
            SERV_MEASURE_OnWrite(handle, bleEvent);
            break;

        case BLE_GATTS_EVT_HVC:
            SERV_MEASURE_OnHvc(handle, bleEvent);
            break;

        case BLE_EVT_TX_COMPLETE:
            SERV_MEASURE_OnTxComplete(handle, bleEvent);
            break;

        default:
            /* No implementation needed. */
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
static void SERV_MEASURE_OnConnect(struct TXW51_SERV_MEASURE_Handle *handle,
                                   ble_evt_t *bleEvent)
{
    TXW51_LOG_DEBUG("[Measure Service] Connected");
}


/***************************************************************************//**
* @brief Handles the disconnection event.
*
* @param[in,out] handle   The handle for the service.
* @param[in]     bleEvent The BLE event that occurred.
* @return Nothing.
******************************************************************************/
static void SERV_MEASURE_OnDisconnect(struct TXW51_SERV_MEASURE_Handle *handle,
                                      ble_evt_t *bleEvent)
{
    TXW51_LOG_DEBUG("[Measure Service] Disconnected");
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
static void SERV_MEASURE_OnWrite(struct TXW51_SERV_MEASURE_Handle *handle,
                                 ble_evt_t *bleEvent)
{
    ble_gatts_evt_write_t *writeEvt = &bleEvent->evt.gatts_evt.params.write;

    if (handle->EventHandler == NULL) {
        return;
    }

    struct TXW51_SERV_MEASURE_Event evt;
    evt.EventType = TXW51_SERV_MEASURE_EVT_UNKNOWN;

    if (writeEvt->handle == handle->CharHandle_Start.value_handle) {
        evt.EventType = TXW51_SERV_MEASURE_EVT_START;

    } else if (writeEvt->handle == handle->CharHandle_Stop.value_handle) {
        evt.EventType = TXW51_SERV_MEASURE_EVT_STOP;

    } else if (writeEvt->handle == handle->CharHandle_Duration.value_handle) {
        evt.EventType = TXW51_SERV_MEASURE_EVT_SET_DURATION;

    } else if (writeEvt->handle == handle->CharHandle_DataStream.cccd_handle) {
        if (ble_srv_is_notification_enabled(writeEvt->data)) {
            evt.EventType = TXW51_SERV_MEASURE_EVT_ENABLE_DATASTREAM;
        } else {
            evt.EventType = TXW51_SERV_MEASURE_EVT_DISABLE_DATASTREAM;
        }
    }

    if (evt.EventType != TXW51_SERV_MEASURE_EVT_UNKNOWN) {
        evt.Value = writeEvt->data;
        evt.Length = writeEvt->len;
        handle->EventHandler(handle, &evt);
    }
}


/***************************************************************************//**
* @brief Handles the HVC event.
*
* Detects on which characteristic the CCCD has been enabled and notifies the
* application. The CCCD enables us to send indications or notifications.
*
* @param[in,out] handle   The handle for the service.
* @param[in]     bleEvent The BLE event that occurred.
* @return Nothing.
******************************************************************************/
static void SERV_MEASURE_OnHvc(struct TXW51_SERV_MEASURE_Handle *handle,
                               ble_evt_t *bleEvent)
{
    ble_gatts_evt_hvc_t *hvcEvt = &bleEvent->evt.gatts_evt.params.hvc;

    if (handle->EventHandler == NULL) {
        return;
    }

    struct TXW51_SERV_MEASURE_Event evt;
    evt.EventType = TXW51_SERV_MEASURE_EVT_UNKNOWN;

    if (hvcEvt->handle == handle->CharHandle_DataStream.value_handle) {
        evt.EventType = TXW51_SERV_MEASURE_EVT_INDICATION_RECEIVED;
    }

    if (evt.EventType != TXW51_SERV_MEASURE_EVT_UNKNOWN) {
        evt.Value = NULL;
        evt.Length = 0;
        handle->EventHandler(handle, &evt);
    }
}


/***************************************************************************//**
* @brief Handles the TX Complete event.
*
* Notifies the application that a notification has been successfully sent.
*
* @param[in,out] handle   The handle for the service.
* @param[in]     bleEvent The BLE event that occurred.
* @return Nothing.
******************************************************************************/
static void SERV_MEASURE_OnTxComplete(struct TXW51_SERV_MEASURE_Handle *handle,
                                      ble_evt_t *bleEvent)
{
    ble_evt_tx_complete_t *txCompleteEvt = &bleEvent->evt.common_evt.params.tx_complete;

    if (handle->EventHandler == NULL) {
        return;
    }

    struct TXW51_SERV_MEASURE_Event evt;
    evt.EventType = TXW51_SERV_MEASURE_EVT_NOTIFICATIONS_SENT;
    evt.Value = &txCompleteEvt->count;
    evt.Length = 1;
    handle->EventHandler(handle, &evt);
}


/***************************************************************************//**
* @brief Adds all the different characteristics to the service.
*
* @param[in,out] serviceHandle The handle for the service.
* @return ERR_NONE if no error occurred.
*         ERR_BLE_SERVICE_ADD_CHARACTERISTIC if characteristic could not be
*                                            added.
******************************************************************************/
static uint32_t SERV_MEASURE_AddAllChars(struct TXW51_SERV_MEASURE_Handle *serviceHandle)
{
    uint32_t err;

    err = SERV_MEASURE_AddChar_Start(serviceHandle);
    if (err != ERR_NONE) {
        return err;
    }

    err = SERV_MEASURE_AddChar_Stop(serviceHandle);
    if (err != ERR_NONE) {
        return err;
    }

    err = SERV_MEASURE_AddChar_Duration(serviceHandle);
    if (err != ERR_NONE) {
        return err;
    }

    err = SERV_MEASURE_AddChar_DataStream(serviceHandle);
    if (err != ERR_NONE) {
        return err;
    }

    return ERR_NONE;
}


/***************************************************************************//**
* @brief Adds the "Start Measurement" characteristic to the service.
*
* @param[in,out] serviceHandle The handle for the service.
* @return ERR_NONE if no error occurred.
*         ERR_BLE_SERVICE_ADD_CHARACTERISTIC if characteristic could not be
*                                            added.
******************************************************************************/
static uint32_t SERV_MEASURE_AddChar_Start(struct TXW51_SERV_MEASURE_Handle *serviceHandle)
{
    struct TXW51_SERV_CharInit charInit;

    /* Initialize characteristic. */
    TXW51_SERV_InitChar(&serviceHandle->ServiceHandle,
                        TXW51_SERV_MEASURE_UUID_CHAR_START,
                        &charInit);

    /* Set up characteristic. */
    charInit.Metadata.char_props.read  = 0;
    charInit.Metadata.char_props.write = 1;
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&charInit.AttrMetadata.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&charInit.AttrMetadata.write_perm);

    /*add_desc_user_description(&charInit, (uint8_t *)TXW51_SERV_MEASURE_STRING_CHAR_START);*/

    charInit.Attribute.max_len = 1;

    /* Add characteristic. */
    return TXW51_SERV_AddChar(&serviceHandle->ServiceHandle,
                              &charInit,
                              &serviceHandle->CharHandle_Start);
}


/***************************************************************************//**
* @brief Adds the "Stop Measurement" characteristic to the service.
*
* @param[in,out] serviceHandle The handle for the service.
* @return ERR_NONE if no error occurred.
*         ERR_BLE_SERVICE_ADD_CHARACTERISTIC if characteristic could not be
*                                            added.
******************************************************************************/
static uint32_t SERV_MEASURE_AddChar_Stop(struct TXW51_SERV_MEASURE_Handle *serviceHandle)
{
    struct TXW51_SERV_CharInit charInit;

    /* Initialize characteristic. */
    TXW51_SERV_InitChar(&serviceHandle->ServiceHandle,
                        TXW51_SERV_MEASURE_UUID_CHAR_STOP,
                        &charInit);

    /* Set up characteristic. */
    charInit.Metadata.char_props.read  = 0;
    charInit.Metadata.char_props.write = 1;
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&charInit.AttrMetadata.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&charInit.AttrMetadata.write_perm);

    /*add_desc_user_description(&charInit, (uint8_t *)TXW51_SERV_MEASURE_STRING_CHAR_STOP);*/

    charInit.Attribute.max_len = 1;

    /* Add characteristic. */
    return TXW51_SERV_AddChar(&serviceHandle->ServiceHandle,
                              &charInit,
                              &serviceHandle->CharHandle_Stop);
}


/***************************************************************************//**
* @brief Adds the "Measurement Duration" characteristic to the service.
*
* @param[in,out] serviceHandle The handle for the service.
* @return ERR_NONE if no error occurred.
*         ERR_BLE_SERVICE_ADD_CHARACTERISTIC if characteristic could not be
*                                            added.
******************************************************************************/
static uint32_t SERV_MEASURE_AddChar_Duration(struct TXW51_SERV_MEASURE_Handle *serviceHandle)
{
    struct TXW51_SERV_CharInit charInit;

    /* Initialize characteristic. */
    TXW51_SERV_InitChar(&serviceHandle->ServiceHandle,
                        TXW51_SERV_MEASURE_UUID_CHAR_DURATION,
                        &charInit);

    /* Set up characteristic. */
    charInit.Metadata.char_props.read  = 1;
    charInit.Metadata.char_props.write = 1;
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&charInit.AttrMetadata.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&charInit.AttrMetadata.write_perm);

    /*add_desc_user_description(&charInit, (uint8_t *)TXW51_SERV_MEASURE_STRING_CHAR_DURATION);*/

    charInit.Attribute.max_len = 2;

    /* Add characteristic. */
    return TXW51_SERV_AddChar(&serviceHandle->ServiceHandle,
                              &charInit,
                              &serviceHandle->CharHandle_Duration);
}


/***************************************************************************//**
* @brief Adds the "Data Stream" characteristic to the service.
*
* This characteristic receives the data during measurement. For this the CCCD
* has to be set by the peer device.
*
* @param[in,out] serviceHandle The handle for the service.
* @return ERR_NONE if no error occurred.
*         ERR_BLE_SERVICE_ADD_CHARACTERISTIC if characteristic could not be
*                                            added.
******************************************************************************/
static uint32_t SERV_MEASURE_AddChar_DataStream(struct TXW51_SERV_MEASURE_Handle *serviceHandle)
{
    struct TXW51_SERV_CharInit charInit;

    /* Initialize characteristic. */
    TXW51_SERV_InitChar(&serviceHandle->ServiceHandle,
                        TXW51_SERV_MEASURE_UUID_CHAR_DATASTRAM,
                        &charInit);

    ble_gatts_attr_md_t cccd_md;
    memset(&cccd_md, 0, sizeof(cccd_md));
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);
    cccd_md.vloc = BLE_GATTS_VLOC_STACK;

    /* Set up characteristic. */
    charInit.Metadata.char_props.read  = 1;
    charInit.Metadata.char_props.write = 0;
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&charInit.AttrMetadata.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&charInit.AttrMetadata.write_perm);

    charInit.Metadata.char_props.notify   = 1;
    charInit.Metadata.char_props.indicate = 1;
    charInit.Metadata.p_cccd_md           = &cccd_md;

    /*add_desc_user_description(&charInit, (uint8_t *)TXW51_SERV_MEASURE_STRING_CHAR_DATASTREAM);*/

    charInit.Attribute.max_len = 20;

    /* Add characteristic. */
    return TXW51_SERV_AddChar(&serviceHandle->ServiceHandle,
                              &charInit,
                              &serviceHandle->CharHandle_DataStream);
}


uint32_t TXW51_SERV_MEASURE_SendData(enum TXW51_SERV_MEASURE_TxType txType,
                                     struct TXW51_SERV_MEASURE_Handle *handle,
                                     struct TXW51_SERV_MEASURE_DataPacket *data)
{
    uint32_t err;
    uint16_t length = sizeof(*data);

    if (handle->ServiceHandle.ConnHandle == BLE_CONN_HANDLE_INVALID) {
        return ERR_BLE_SERVICE_NO_CONNECTION;
    }

    ble_gatts_hvx_params_t hvxParams;
    memset(&hvxParams, 0, sizeof(hvxParams));

    hvxParams.handle = handle->CharHandle_DataStream.value_handle;
    hvxParams.type   = txType;
    hvxParams.offset = 0;
    hvxParams.p_len  = &length;
    hvxParams.p_data = (uint8_t *)data;

    err = sd_ble_gatts_hvx(handle->ServiceHandle.ConnHandle, &hvxParams);
    if (err == NRF_ERROR_INVALID_STATE) {
        TXW51_LOG_WARNING("[Measure Service] Could not send notification. CCCD is not enabled.");
        return ERR_SERVICE_MEASURE_CCCD_NOT_ENABLED;
    } else if (err != NRF_SUCCESS) {
        TXW51_LOG_WARNING("[Measure Service] Could not send notification.");
        return ERR_SERVICE_MEASURE_HVC_COULD_NOT_SEND;
    }

    return ERR_NONE;
}

