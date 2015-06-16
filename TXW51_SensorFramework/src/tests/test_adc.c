/***************************************************************************//**
 * @brief   This module can be used to test the ADC according to the Test
 *          Specification entry T125.
 *
 * You have to include this file to the build and exclude the file with the
 * real main function.
 *
 * It prints periodically the ACD values to the log.
 *
 * @file    test_adc.c
 * @version 1.0
 * @date    15.01.2015
 * @author  Daniel Meer
 *
 * @remark  Last Modifications:
 *          15.01.2015 meerd1 created
 ******************************************************************************/

/*----- Header-Files ---------------------------------------------------------*/
#include <stdbool.h>

#include "nrf/sd_common/softdevice_handler.h"

#include "txw51_framework/hw/uart.h"
#include "txw51_framework/utils/log.h"

#include "app/adc_example.h"

/*----- Macros ---------------------------------------------------------------*/

/*----- Data types -----------------------------------------------------------*/

/*----- Function prototypes --------------------------------------------------*/

/*----- Data -----------------------------------------------------------------*/

/*----- Implementation -------------------------------------------------------*/

int main(void)
{
    TXW51_LOG_Init();
    TXW51_UART_WriteString((uint8_t *)"\r\n\r\n");
    TXW51_UART_WriteString((uint8_t *)"ADC-Test\r\n");

    SOFTDEVICE_HANDLER_INIT(CONFIG_CLOCK_LFCLK_SOURCE, false);

    APP_TIMER_INIT(CONFIG_TIMERS_PRESCALER,
                       CONFIG_TIMERS_MAX_TIMERS,
                       CONFIG_TIMERS_OP_QUEUE_SIZE,
                       false);

    APPL_ADC_EXMPL_Init();
    APPL_ADC_EXMPL_Start();

    while (true) {
        /* Do nothing. */
    }

    return 0;
}

