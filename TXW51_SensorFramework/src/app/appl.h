/***************************************************************************//**
 * @brief   Main module of the application.
 *
 * First, the hardware and the Bluetooth Smart stack get set up. Afterwards,
 * the main loop gets started.
 *
 * @file    appl.h
 * @version 1.0
 * @date    10.11.2014
 * @author  Daniel Meer
 *
 * @remark  Last Modifications:
 *          10.11.2014 meerd1 created
 ******************************************************************************/

#ifndef TXW51_APPLICATION_APPL_H_
#define TXW51_APPLICATION_APPL_H_

/*----- Header-Files ---------------------------------------------------------*/
#include "stdbool.h"

/*----- Macros ---------------------------------------------------------------*/

/*----- Data types -----------------------------------------------------------*/

/*----- Function prototypes --------------------------------------------------*/

/***************************************************************************//**
 * @brief Main function of the application. Initializes everything and starts
 * the main loop.
 *
 * @return The function does not return.
 ******************************************************************************/
extern void APPL_Start(void);

/*----- Data -----------------------------------------------------------------*/
extern bool gIsNewAccDataAvailable;     /**< Global flag that indicates if new accelerometer data is available. */
extern bool gIsNewGyroDataAvailable;    /**< Global flag that indicates if new gyroscope data is available. */
extern bool gIsTimeout;                 /**< Global flag that indicates if the device should go into standby mode. */

#endif /* TXW51_APPLICATION_APPL_H_ */
