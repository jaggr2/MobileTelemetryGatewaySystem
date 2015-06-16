/***************************************************************************//**
 * @brief   Callback handling of the S110 Softdevice.
 *
 * Handles callbacks from the Softdevice for BLE and system events. The
 * application has the possibility to register a callback as well.
 *
 * @file    cb.c
 * @version 1.0
 * @date    10.11.2014
 * @author  Daniel Meer
 *
 * @remark  Last Modifications:
 *          10.11.2014 meerd1 created
 ******************************************************************************/

/*----- Header-Files ---------------------------------------------------------*/
#include "cb.h"

#include <stdint.h>
#include <string.h>

#include "nrf/app_common/pstorage.h"
#include "nrf/ble/ble_advdata.h"
#include "nrf/s110/ble_hci.h"
#include "nrf/sd_common/softdevice_handler.h"

#include "txw51_framework/config/config.h"
#include "txw51_framework/ble/btle.h"

/*----- Macros ---------------------------------------------------------------*/

/*----- Data types -----------------------------------------------------------*/

/*----- Function prototypes --------------------------------------------------*/
static void CB_HandleBleEvent(ble_evt_t *bleEvent);

/*----- Data -----------------------------------------------------------------*/
static uint16_t  connectionHandle = BLE_CONN_HANDLE_INVALID;    /**< Handle of the current connection. */
static TXW51_CB_BleHandler_t bleEventCallback = NULL;           /**< BLE callback handle from the application. */
static TXW51_CB_SysHandler_t sysEventCallback = NULL;           /**< System callback handle from the application. */

/*----- Implementation -------------------------------------------------------*/

void TXW51_CB_DispatchBleEvent(ble_evt_t *bleEvent)
{
	CB_HandleBleEvent(bleEvent);
    ble_conn_params_on_ble_evt(bleEvent);

    if (bleEventCallback != NULL) {
        bleEventCallback(bleEvent);
    }
}


void TXW51_CB_DispatchSysEvent(uint32_t sysEvent)
{
    /* if clause needed because of bug in pstorage.c. */
    if ((sysEvent == NRF_EVT_FLASH_OPERATION_SUCCESS) ||
        (sysEvent == NRF_EVT_FLASH_OPERATION_ERROR))  {
        pstorage_sys_event_handler(sysEvent);
    }

    if (sysEventCallback != NULL) {
        sysEventCallback(sysEvent);
    }
}


void TXW51_CB_HandleConnParamsEvent(ble_conn_params_evt_t *event)
{
    uint32_t err;

    if (event->evt_type == BLE_CONN_PARAMS_EVT_FAILED) {
        err = sd_ble_gap_disconnect(connectionHandle,
                                    BLE_HCI_CONN_INTERVAL_UNACCEPTABLE);
        APP_ERROR_CHECK(err);
    }
}


void TXW51_CB_HandleConnParamsError(uint32_t error)
{
    APP_ERROR_HANDLER(error);
}


void TXW51_CB_HandleServiceError(uint32_t error)
{
    APP_ERROR_HANDLER(error);
}


/***************************************************************************//**
* @brief Function for handling the Application's BLE Stack events.
*
* Detailed description.
*
* @param[in] bleEvent Bluetooth stack event.
* @return Nothing.
******************************************************************************/
static void CB_HandleBleEvent(ble_evt_t *bleEvent)
{
    static ble_gap_evt_auth_status_t authStatus;

    uint32_t err;
    ble_gap_enc_info_t *encryptInfo;

    switch (bleEvent->header.evt_id) {
        case BLE_GAP_EVT_CONNECTED:
            connectionHandle = bleEvent->evt.gap_evt.conn_handle;
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            connectionHandle = BLE_CONN_HANDLE_INVALID;
            TXW51_BLE_StartAdvertising();
            break;

        case BLE_GAP_EVT_SEC_PARAMS_REQUEST:
            err = sd_ble_gap_sec_params_reply(connectionHandle,
                                              BLE_GAP_SEC_STATUS_SUCCESS,
                                              TXW51_BLE_GetSecurityParams());
            APP_ERROR_CHECK(err);
            break;

        case BLE_GATTS_EVT_SYS_ATTR_MISSING:
            err = sd_ble_gatts_sys_attr_set(connectionHandle, NULL, 0);
            APP_ERROR_CHECK(err);
            break;

        case BLE_GAP_EVT_AUTH_STATUS:
            authStatus = bleEvent->evt.gap_evt.params.auth_status;
            break;

        case BLE_GAP_EVT_SEC_INFO_REQUEST:
            encryptInfo = &authStatus.periph_keys.enc_info;
            if (encryptInfo->div == bleEvent->evt.gap_evt.params.sec_info_request.div) {
                err = sd_ble_gap_sec_info_reply(connectionHandle, encryptInfo, NULL);
                APP_ERROR_CHECK(err);
            } else {
                /* No keys found for this device. */
                err = sd_ble_gap_sec_info_reply(connectionHandle, NULL, NULL);
                APP_ERROR_CHECK(err);
            }
            break;

        case BLE_GAP_EVT_TIMEOUT:
            if (bleEvent->evt.gap_evt.params.timeout.src == BLE_GAP_TIMEOUT_SRC_ADVERTISEMENT) {
                /* Handle timeout in application. */
            }
            break;

        default:
            /* No implementation needed. */
            break;
    }
}


void TXW51_CB_RegisterBleCallback(TXW51_CB_BleHandler_t handler)
{
    bleEventCallback = handler;
}


void TXW51_CB_RegisterSysCallback(TXW51_CB_SysHandler_t handler)
{
    sysEventCallback = handler;
}

