/***************************************************************************//**
 * @brief   Utility functions for the error handling.
 *
 * @file    error.c
 * @version 1.0
 * @date    10.11.2014
 * @author  Daniel Meer
 *
 * @remark  Last Modifications:
 *          10.11.2014 meerd1 created
 ******************************************************************************/

/*----- Header-Files ---------------------------------------------------------*/
#include "error.h"

#include <stdint.h>
#include <string.h>

#include "nrf/nrf.h"

/*----- Macros ---------------------------------------------------------------*/
#define DEAD_BEEF    ( 0xDEADBEEF )  /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */

/*----- Data types -----------------------------------------------------------*/

/*----- Function prototypes --------------------------------------------------*/

/*----- Data -----------------------------------------------------------------*/

/*----- Implementation -------------------------------------------------------*/

void app_error_handler(uint32_t errorCode,
                       uint32_t lineNumber,
                       const uint8_t *fileName)
{
	while(true) {
	    /* Do nothing. */
	}

    /* On assert, the system can only recover with a reset. */
    NVIC_SystemReset();
}


void assert_nrf_callback(uint16_t lineNumber, const uint8_t *fileName)
{
    app_error_handler(DEAD_BEEF, lineNumber, fileName);
}

