/***************************************************************************//**
 * @brief   Setup of Bluetooth Stack.
 *
 * The BLE module handles the initialization and setup of the Bluetooth Smart
 * Stack "S110" from Nordic Semiconductors.
 *
 * @file    btle.c
 * @version 1.0
 * @date    10.11.2014
 * @author  Daniel Meer
 *
 * @remark  Last Modifications:
 *          10.11.2014 meerd1 created
 ******************************************************************************/

/*----- Header-Files ---------------------------------------------------------*/
#include "btle.h"

#include <stdint.h>
#include <string.h>

#include "nrf/ble/ble_advdata.h"
#include "nrf/ble/ble_conn_params.h"
#include "nrf/s110/ble_hci.h"
#include "nrf/sd_common/softdevice_handler.h"

#include "txw51_framework/ble/cb.h"
#include "txw51_framework/config/config.h"
#include "txw51_framework/config/config_services.h"
#include "txw51_framework/config/pstorage_platform.h"
#include "txw51_framework/utils/log.h"

/*----- Macros ---------------------------------------------------------------*/

/*----- Data types -----------------------------------------------------------*/

/*----- Function prototypes --------------------------------------------------*/
static void BLE_InitStack(void);
static void BLE_InitSecurityParams(void);
static void BLE_InitConnectionParams(void);
static void BLE_InitGapParams(void);

/*----- Data -----------------------------------------------------------------*/
static ble_gap_sec_params_t  securityParams;   /**< Security requirements for this application. */

/*----- Implementation -------------------------------------------------------*/

void TXW51_BLE_Init(void)
{
	BLE_InitStack();
	BLE_InitGapParams();
	BLE_InitConnectionParams();
	BLE_InitSecurityParams();
}


void TXW51_BLE_InitAdvertising(void)
{
    uint32_t err;
    ble_advdata_t advData;
    ble_advdata_t scanResponseData;
    uint8_t flags = CONFIG_GAP_ADV_FLAGS;

    ble_uuid_t adv_uuids[] =
    {
        {TXW51_SERVICE_UUID_BLANK_SERVICE, BLE_UUID_TYPE_VENDOR_BEGIN}
    };

    /* Build and set advertising data. */
    memset(&advData, 0, sizeof(advData));
    advData.name_type          = BLE_ADVDATA_FULL_NAME;
    advData.include_appearance = true;
    advData.flags.size         = sizeof(flags);
    advData.flags.p_data       = &flags;

    memset(&scanResponseData, 0, sizeof(scanResponseData));
    scanResponseData.uuids_complete.uuid_cnt = sizeof(adv_uuids) / sizeof(adv_uuids[0]);
    scanResponseData.uuids_complete.p_uuids  = adv_uuids;

    err = ble_advdata_set(&advData, &scanResponseData);
    APP_ERROR_CHECK(err);
}


void TXW51_BLE_StartAdvertising(void)
{
    uint32_t err;
    ble_gap_adv_params_t advParams;

    /* Initialize advertising parameters */
    memset(&advParams, 0, sizeof(advParams));

    advParams.type        = BLE_GAP_ADV_TYPE_ADV_IND;
    advParams.p_peer_addr = NULL;
    advParams.fp          = BLE_GAP_ADV_FP_ANY;
    advParams.interval    = CONFIG_GAP_ADV_INTERVAL;
    advParams.timeout     = CONFIG_GAP_ADV_TIMEOUT_IN_SECONDS;

    err = sd_ble_gap_adv_start(&advParams);
    APP_ERROR_CHECK(err);
}


const ble_gap_sec_params_t* TXW51_BLE_GetSecurityParams(void)
{
	return &securityParams;
}


/***************************************************************************//**
* @brief Function for initializing the BLE stack.
*
* Initializes the SoftDevice and the BLE event interrupt.
*
* @return Nothing.
******************************************************************************/
static void BLE_InitStack(void)
{
    uint32_t err;

    /* Enable BLE stack. */
    ble_enable_params_t bleEnableParams;
    memset(&bleEnableParams, 0, sizeof(bleEnableParams));
    bleEnableParams.gatts_enable_params.service_changed =
    		CONFIG_GAP_IS_SRVC_CHANGED_CHARACT_PRESENT;

    err = sd_ble_enable(&bleEnableParams);
    APP_ERROR_CHECK(err);

    /* Register with the SoftDevice handler module for BLE events. */
    err = softdevice_ble_evt_handler_set(TXW51_CB_DispatchBleEvent);
    APP_ERROR_CHECK(err);

    /* Register with the SoftDevice handler module for system events. */
    err = softdevice_sys_evt_handler_set(TXW51_CB_DispatchSysEvent);
    APP_ERROR_CHECK(err);
}


/***************************************************************************//**
* @brief Function for initializing the security parameters.
*
* @return Nothing.
******************************************************************************/
static void BLE_InitSecurityParams(void)
{
    securityParams.timeout      = CONFIG_GAP_SEC_PARAM_TIMEOUT;
    securityParams.bond         = CONFIG_GAP_SEC_PARAM_BOND;
    securityParams.mitm         = CONFIG_GAP_SEC_PARAM_MITM;
    securityParams.io_caps      = CONFIG_GAP_SEC_PARAM_IO_CAPABILITIES;
    securityParams.oob          = CONFIG_GAP_SEC_PARAM_OOB;
    securityParams.min_key_size = CONFIG_GAP_SEC_PARAM_MIN_KEY_SIZE;
    securityParams.max_key_size = CONFIG_GAP_SEC_PARAM_MAX_KEY_SIZE;
}


/***************************************************************************//**
* @brief Initializes the connection parameters with the right values.
*
* @return Nothing.
******************************************************************************/
static void BLE_InitConnectionParams(void)
{
    uint32_t err;
    ble_conn_params_init_t connParamsInit;

    memset(&connParamsInit, 0, sizeof(connParamsInit));
    connParamsInit.p_conn_params                  = NULL;
    connParamsInit.first_conn_params_update_delay = CONFIG_GAP_FIRST_CONN_PARAMS_UPDATE_DELAY;
    connParamsInit.next_conn_params_update_delay  = CONFIG_GAP_NEXT_CONN_PARAMS_UPDATE_DELAY;
    connParamsInit.max_conn_params_update_count   = CONFIG_GAP_MAX_CONN_PARAMS_UPDATE_COUNT;
    connParamsInit.start_on_notify_cccd_handle    = BLE_GATT_HANDLE_INVALID;
    connParamsInit.disconnect_on_fail             = false;
    connParamsInit.evt_handler                    = TXW51_CB_HandleConnParamsEvent;
    connParamsInit.error_handler                  = TXW51_CB_HandleConnParamsError;

    err = ble_conn_params_init(&connParamsInit);
    APP_ERROR_CHECK(err);
}


/***************************************************************************//**
* @brief Function for the GAP initialization.
*
* This function sets up all the necessary GAP (Generic Access Profile)
* parameters of the device including the device name, appearance, and the
* preferred connection parameters.
*
* @return Nothing.
******************************************************************************/
static void BLE_InitGapParams(void)
{
    uint32_t err;
    ble_gap_conn_params_t gapConnParams;
    ble_gap_conn_sec_mode_t securityMode;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&securityMode);
    err = sd_ble_gap_device_name_set(&securityMode,
                                     (const uint8_t *)CONFIG_GAP_DEVICE_NAME,
                                     strlen(CONFIG_GAP_DEVICE_NAME));
    APP_ERROR_CHECK(err);

    err = sd_ble_gap_appearance_set(CONFIG_GAP_BLE_APPEARANCE);
    APP_ERROR_CHECK(err);

    memset(&gapConnParams, 0, sizeof(gapConnParams));
    gapConnParams.min_conn_interval = CONFIG_GAP_MIN_CONN_INTERVAL;
    gapConnParams.max_conn_interval = CONFIG_GAP_MAX_CONN_INTERVAL;
    gapConnParams.slave_latency     = CONFIG_GAP_SLAVE_LATENCY;
    gapConnParams.conn_sup_timeout  = CONFIG_GAP_CONN_SUP_TIMEOUT;

    err = sd_ble_gap_ppcp_set(&gapConnParams);
    APP_ERROR_CHECK(err);
}

