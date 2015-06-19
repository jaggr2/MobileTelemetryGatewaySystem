/***************************************************************************//**
 * @brief   Module that handles the Bluetooth Smart Measurement Service.
 *
 * It initializes and communicates with the Measurement service. Furthermore,
 * it starts the measurement and sends the data over the Bluetooth link.
 *
 * @file    measurement.h
 * @version 1.0
 * @date    09.12.2014
 * @author  Daniel Meer
 *
 * @remark  Last Modifications:
 *          09.12.2014 meerd1 created
 ******************************************************************************/

#ifndef TXW51_APPLICATION_MEASUREMENT_H_
#define TXW51_APPLICATION_MEASUREMENT_H_

/*----- Header-Files ---------------------------------------------------------*/
#include <stdint.h>

#include "txw51_framework/ble/service_measure.h"

/*----- Macros ---------------------------------------------------------------*/

/*----- Data types -----------------------------------------------------------*/

/*----- Function prototypes --------------------------------------------------*/

/***************************************************************************//**
 * @brief Initializes this module and the Measurement service.
 *
 * @param[out] serviceHandle Handle of the Measurement service.
 *
 * @return ERR_NONE if no error occurred.
 *         ERR_BLE_SERVICE_INIT_FAILED if service initialization failed.
 ******************************************************************************/
extern uint32_t APPL_MEASUREMENT_InitService(struct TXW51_SERV_MEASURE_Handle *serviceHandle);

/***************************************************************************//**
 * @brief Sends as much data as possible over the Bluetooth link.
 *
 * Data can be sent via indications or notification.
 *
 * @param[in] txType Set to send the data with indications or notifications.
 *
 * @return Nothing.
 ******************************************************************************/
extern void APPL_MEASUREMENT_SendAllData(enum TXW51_SERV_MEASURE_TxType txType);

/*----- Data -----------------------------------------------------------------*/
#endif /* TXW51_APPLICATION_MEASUREMENT_H_ */
