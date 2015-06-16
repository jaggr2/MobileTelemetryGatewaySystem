/***************************************************************************//**
 * @brief   This module contains helper functions to set up the device.
 *
 * @file    setup.h
 * @version 1.0
 * @date    18.01.2015
 * @author  Daniel Meer
 *
 * @remark  Last Modifications:
 *          18.01.2015 meerd1 created
 ******************************************************************************/

#ifndef TXW51_FRAMEWORK_UTILS_SETUP_H_
#define TXW51_FRAMEWORK_UTILS_SETUP_H_

/*----- Header-Files ---------------------------------------------------------*/
#include "nrf/sd_common/softdevice_handler.h"

#include "txw51_framework/config/config.h"

/*----- Macros ---------------------------------------------------------------*/

/*----- Data types -----------------------------------------------------------*/

/*----- Function prototypes --------------------------------------------------*/

/***************************************************************************//**
 * @brief Initializes the S110 softdevice.
 *
 * @return Nothing.
 ******************************************************************************/
extern void TXW51_SETUP_InitSoftdevice(void);

/***************************************************************************//**
 * @brief Initializes the scheduler.
 *
 * @return Nothing.
 ******************************************************************************/
extern void TXW51_SETUP_InitScheduler(void);

/***************************************************************************//**
 * @brief Initializes the clocks.
 *
 * @return Nothing.
 ******************************************************************************/
extern void TXW51_SETUP_InitHfClock(void);

/*----- Data -----------------------------------------------------------------*/

#endif /* TXW51_FRAMEWORK_UTILS_SETUP_H_ */
