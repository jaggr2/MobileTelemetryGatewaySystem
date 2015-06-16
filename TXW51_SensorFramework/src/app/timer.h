/***************************************************************************//**
 * @brief   Module for the timeout timer.
 *
 * Initializes the timer module and sets up the timeout timer. This timer is
 * used to put the application to sleep after a specified time.
 *
 * @file    timer.h
 * @version 1.0
 * @date    05.12.2014
 * @author  Daniel Meer
 *
 * @remark  Last Modifications:
 *          05.12.2014 meerd1 created
 ******************************************************************************/

#ifndef TXW51_APPLICATION_TIMER_H_
#define TXW51_APPLICATION_TIMER_H_

/*----- Header-Files ---------------------------------------------------------*/
#include <stdint.h>

/*----- Macros ---------------------------------------------------------------*/
#define APPL_TIMER_TIMEOUT_TICKS    ( 30 * RTC_FREQUENCY )      /**< Timer timeout in ((# of seconds)*(RTC_FREQUENCY)). */

/*----- Data types -----------------------------------------------------------*/

/*----- Function prototypes --------------------------------------------------*/

/***************************************************************************//**
 * @brief Initializes the timer module and the timeout timer.
 *
 * @return ERR_NONE if no error occurred.
 *         ERR_TIMER_INIT_FAILED if initialization failed.
 ******************************************************************************/
extern uint32_t APPL_TIMER_Init(void);

/***************************************************************************//**
 * @brief Starts the timeout timer.
 *
 * @return ERR_NONE if no error occurred.
 *         ERR_TIMER_START_FAILED if timer could not be started.
 ******************************************************************************/
extern uint32_t APPL_TIMER_Start(void);

/***************************************************************************//**
 * @brief Stops the timeout timer.
 *
 * @return ERR_NONE if no error occurred.
 *         ERR_TIMER_STOP_FAILED if timer could not be stopped.
 ******************************************************************************/
extern uint32_t APPL_TIMER_Stop(void);

/*----- Data -----------------------------------------------------------------*/

#endif /* TXW51_APPLICATION_TIMER_H_ */
