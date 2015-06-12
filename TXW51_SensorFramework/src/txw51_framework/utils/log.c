/***************************************************************************//**
 * @brief   This module can be used to log messages.
 *
 * It uses the UART to send static logging messages used for debugging. The
 * amount of logging messages can be configured.
 *
 * @file    log.c
 * @version 1.0
 * @date    17.11.2014
 * @author  Daniel Meer
 *
 * @remark  Last Modifications:
 *          17.11.2014 meerd1 created
 ******************************************************************************/

/*----- Header-Files ---------------------------------------------------------*/
#include "log.h"

#include "txw51_framework/hw/uart.h"

/*----- Macros ---------------------------------------------------------------*/

/*----- Data types -----------------------------------------------------------*/

/*----- Function prototypes --------------------------------------------------*/

/*----- Data -----------------------------------------------------------------*/

/*----- Implementation -------------------------------------------------------*/

#if (CONFIG_LOG_LEVEL > CONFIG_LOG_LEVEL_NONE)

/**
 * @brief Prefix that gets added to the output of the different log levels.
 */
static const char const *logLevels[] = {
    "",
    "ERROR:   ",
    "WARNING: ",
    "INFO:    ",
    "DEBUG:   "
};


void TXW51_LOG_Init(void)
{
    struct TXW51_UART_Init uart_init = {
        .RxPin  = CONFIG_LOG_UART_RX,
        .TxPin  = CONFIG_LOG_UART_TX,
        .RtsPin = CONFIG_LOG_UART_CTS,
        .CtsPin = CONFIG_LOG_UART_RTS,
        .EnableHwFlowControl = CONFIG_LOG_UART_HWFC,
        .Baudrate            = CONFIG_LOG_UART_BAUDRATE
    };

    TXW51_UART_Init(&uart_init);
}


void TXW51_LOG_Print(const char *msg, enum TXW51_LOG_Level level)
{
    TXW51_UART_WriteString((const uint8_t *)logLevels[level]);
    TXW51_UART_WriteString((const uint8_t *)msg);
    TXW51_UART_WriteString((const uint8_t *)"\r\n");
}

#else

void TXW51_LOG_Init(void) { }
void TXW51_LOG_Print(const char *msg, enum TXW51_LOG_Level level) { }

#endif /* CONFIG_LOG_LEVEL > CONFIG_LOG_LEVEL_NONE */

