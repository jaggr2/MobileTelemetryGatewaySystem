/***************************************************************************//**
 * @brief   Module for the timeout timer.
 *
 * Initializes the timer module and sets up the timeout timer. This timer is
 * used to put the application to sleep after a specified time.
 *
 * @file    timer.c
 * @version 1.0
 * @date    05.12.2014
 * @author  Daniel Meer
 *
 * @remark  Last Modifications:
 *          05.12.2014 meerd1 created
 ******************************************************************************/

/*----- Header-Files ---------------------------------------------------------*/
#include "timer.h"

#include "nrf/app_common/app_timer.h"

#include "txw51_framework/config/config.h"
#include "txw51_framework/utils/log.h"

#include "app/appl.h"
#include "app/error.h"
#include "app/device_info.h"

/*----- Macros ---------------------------------------------------------------*/
#define APPL_TIMER_INTERRUPT_MODE       ( APP_TIMER_MODE_SINGLE_SHOT )  /**< Sets the timer to single shot or repeated mode. */

/*----- Data types -----------------------------------------------------------*/

/*----- Function prototypes --------------------------------------------------*/
static void TIMER_CallbackHandler(void *context);

/*----- Data -----------------------------------------------------------------*/
static app_timer_id_t timerHandle;      /**< Handle for the timeout timer. */

/*----- Implementation -------------------------------------------------------*/

uint32_t APPL_TIMER_Init(void)
{
    uint32_t err;

    /* Initialize timer module. */
    APP_TIMER_INIT(CONFIG_TIMERS_PRESCALER,
                   CONFIG_TIMERS_MAX_TIMERS,
                   CONFIG_TIMERS_OP_QUEUE_SIZE,
                   CONFIG_S110_USE_SCHEDULER);

    /* Creates the timeout timer. */
    err = app_timer_create(&timerHandle,
                           APPL_TIMER_INTERRUPT_MODE,
                           TIMER_CallbackHandler);
    if (err != NRF_SUCCESS) {
        TXW51_LOG_ERROR("[Timer] Initialization failed.");
        return ERR_TIMER_INIT_FAILED;
    }

    TXW51_LOG_DEBUG("[Timer] Initialization successful.");
    return ERR_NONE;
}


/***************************************************************************//**
 * @brief Callback handler of the timeout timer.
 *
 * @param[in] context Not used.
 *
 * @return Nothing.
 ******************************************************************************/
static void TIMER_CallbackHandler(void *context)
{
    gIsTimeout = true;
    TXW51_LOG_DEBUG("Timeout expired!");
}


uint32_t APPL_TIMER_Start(void)
{
    uint32_t err;

    if (APPL_DEVINFO_IsPowerSaveDisabled())
    {
    	return ERR_NONE;
    }

    err = app_timer_start(timerHandle, APPL_TIMER_TIMEOUT_TICKS, NULL);
    if (err != NRF_SUCCESS) {
        TXW51_LOG_ERROR("[Timer] Could not start timeout timer.");
        return ERR_TIMER_START_FAILED;
    }

    TXW51_LOG_DEBUG("[Timer] Timeout timer started.");
    return ERR_NONE;
}


uint32_t APPL_TIMER_Stop(void)
{
    uint32_t err;


    if (APPL_DEVINFO_IsPowerSaveDisabled())
    {
    	return ERR_NONE;
    }

    err = app_timer_stop(timerHandle);
    if (err != NRF_SUCCESS) {
        TXW51_LOG_ERROR("[Timer] Could not stop timeout timer.");
        return ERR_TIMER_STOP_FAILED;
    }

    TXW51_LOG_DEBUG("[Timer] Timeout timer stopped.");
    return ERR_NONE;
}

