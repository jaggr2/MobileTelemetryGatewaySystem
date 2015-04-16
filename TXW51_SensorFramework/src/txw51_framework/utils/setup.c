/***************************************************************************//**
 * @brief   This module contains helper functions to set up the device.
 *
 * @file    setup.c
 * @version 1.0
 * @date    18.01.2015
 * @author  Daniel Meer
 *
 * @remark  Last Modifications:
 *          18.01.2015 meerd1 created
 ******************************************************************************/

/*----- Header-Files ---------------------------------------------------------*/
#include "setup.h"

/*----- Macros ---------------------------------------------------------------*/

/*----- Data types -----------------------------------------------------------*/

/*----- Function prototypes --------------------------------------------------*/

/*----- Data -----------------------------------------------------------------*/

/*----- Implementation -------------------------------------------------------*/

void TXW51_SETUP_InitSoftdevice(void)
{
    SOFTDEVICE_HANDLER_INIT(CONFIG_CLOCK_LFCLK_SOURCE,
                            CONFIG_S110_USE_SCHEDULER);
}


void TXW51_SETUP_InitScheduler(void)
{
    APP_SCHED_INIT(CONFIG_SCHED_MAX_EVENT_DATA_SIZE, CONFIG_SCHED_QUEUE_SIZE);
}


void TXW51_SETUP_InitHfClock(void)
{
    uint32_t isClockRunning = 0;

    sd_clock_hfclk_request();
    while (!isClockRunning) {
        sd_clock_hfclk_is_running(&isClockRunning);
    }
}

