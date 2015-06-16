/***************************************************************************//**
 * @brief   Setup of Bluetooth Stack.
 *
 * The BLE module handles the initialization and setup of the Bluetooth Smart
 * Stack "S110" from Nordic Semiconductors.
 *
 * @file    btle.h
 * @version 1.0
 * @date    10.11.2014
 * @author  Daniel Meer
 *
 * @remark  Last Modifications:
 *          10.11.2014 meerd1 created
 ******************************************************************************/

#ifndef TXW51_FRAMEWORK_BLE_BTLE_H_
#define TXW51_FRAMEWORK_BLE_BTLE_H_

/*----- Header-Files ---------------------------------------------------------*/
#include "nrf/s110/ble_gap.h"

/*----- Macros ---------------------------------------------------------------*/

/*----- Data types -----------------------------------------------------------*/

/*----- Function prototypes --------------------------------------------------*/

/***************************************************************************//**
* @brief Function for initializing the BLE stack.
*
* Initializes the SoftDevice and the BLE event interrupt.
*
* @return Nothing.
******************************************************************************/
extern void TXW51_BLE_Init(void);

/***************************************************************************//**
* @brief Function for initializing the Advertising functionality.
*
* Encodes the required advertising data and passes it to the  stack. Also builds
* a structure to be passed to the stack when starting advertising.
*
* @return Nothing.
******************************************************************************/
extern void TXW51_BLE_InitAdvertising(void);

/***************************************************************************//**
* @brief Function for starting advertising.
*
* @return Nothing.
******************************************************************************/
extern void TXW51_BLE_StartAdvertising(void);

/***************************************************************************//**
* @brief Get security parameters of this application.
*
* @return Structure with the security parameters.
******************************************************************************/
extern const ble_gap_sec_params_t* TXW51_BLE_GetSecurityParams(void);

/*----- Data -----------------------------------------------------------------*/

#endif /* TXW51_FRAMEWORK_BLE_BTLE_H_ */
