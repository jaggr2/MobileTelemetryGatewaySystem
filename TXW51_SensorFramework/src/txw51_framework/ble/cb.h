/***************************************************************************//**
 * @brief   Callback handling of the S110 Softdevice.
 *
 * Handles callbacks from the Softdevice for BLE and system events. The
 * application has the possibility to register a callback as well.
 *
 * @file    cb.h
 * @version 1.0
 * @date    10.11.2014
 * @author  Daniel Meer
 *
 * @remark  Last Modifications:
 *          10.11.2014 meerd1 created
 ******************************************************************************/

#ifndef TXW51_FRAMEWORK_BLE_CB_H_
#define TXW51_FRAMEWORK_BLE_CB_H_

/*----- Header-Files ---------------------------------------------------------*/
#include "nrf/ble/ble_conn_params.h"
#include "nrf/s110/ble.h"

/*----- Macros ---------------------------------------------------------------*/

/*----- Data types -----------------------------------------------------------*/
typedef void (*TXW51_CB_BleHandler_t) (ble_evt_t *bleEvent);    /**< Callback for BLE events. */
typedef void (*TXW51_CB_SysHandler_t) (uint32_t sysEvent);      /**< Callback for system events. */

/*----- Function prototypes --------------------------------------------------*/

/***************************************************************************//**
* @brief Function for dispatching a BLE stack event to all modules with a BLE
*        stack event handler.
*
* This function is called from the scheduler in the main loop after a BLE stack
* event has been received.
*
* @param[in] bleEvent Bluetooth stack event.
* @return Nothing.
******************************************************************************/
extern void TXW51_CB_DispatchBleEvent(ble_evt_t *bleEvent);

/***************************************************************************//**
* @brief Function for dispatching a system event to interested modules.
*
* This function is called from the System event interrupt handler after a system
* event has been received.
*
* @param[in] sysEvent The name of the task.
* @return Nothing.
******************************************************************************/
extern void TXW51_CB_DispatchSysEvent(uint32_t sysEvent);

/***************************************************************************//**
* @brief Function for handling the connection parameters.
*
* This function will be called for all events in the Connection Parameters
* Module which are passed to the application. All this function does is to
* disconnect. This could have been done by simply setting the disconnect_on_fail
* config parameter, but instead we use the event handler mechanism to
* demonstrate its use.
*
* @param[in] event Event received from the Connection Parameters Module.
* @return Nothing.
******************************************************************************/
extern void TXW51_CB_HandleConnParamsEvent(ble_conn_params_evt_t * event);

/***************************************************************************//**
* @brief  Function for handling a Connection Parameters error.
*
* @param[in] error Error code containing information about what went wrong.
* @return Nothing.
******************************************************************************/
extern void TXW51_CB_HandleConnParamsError(uint32_t error);

/***************************************************************************//**
* @brief Function for handling Service errors.
*
* A pointer to this function will be passed to each service which may need to
* inform the application about an error.
*
* @param[in] error Error code containing information about what went wrong.
* @return Nothing.
******************************************************************************/
extern void TXW51_CB_HandleServiceError(uint32_t error);

/***************************************************************************//**
* @brief Registers a BLE callback for the application.
*
* This allows the application to handle BLE events as well.
*
* @param[in] handler The function to call back.
* @return Nothing.
******************************************************************************/
extern void TXW51_CB_RegisterBleCallback(TXW51_CB_BleHandler_t handler);

/***************************************************************************//**
* @brief Registers a system callback for the application.
*
* This allows the application to handle system events as well.
*
* @param[in] handler The function to call back.
* @return Nothing.
******************************************************************************/
extern void TXW51_CB_RegisterSysCallback(TXW51_CB_SysHandler_t handler);

/*----- Data -----------------------------------------------------------------*/

#endif /* TXW51_FRAMEWORK_BLE_CB_H_ */
