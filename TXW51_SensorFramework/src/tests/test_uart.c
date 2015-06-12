/***************************************************************************//**
 * @brief   This module can be used to test the UART according to the Test
 *          Specification entry T123.
 *
 * You have to include this file to the build and exclude the file with the
 * real main function.
 *
 * It echoes everything that is sent over the UART back.
 *
 * @file    test_uart.c
 * @version 1.0
 * @date    15.01.2015
 * @author  Daniel Meer
 *
 * @remark  Last Modifications:
 *          15.01.2015 meerd1 created
 ******************************************************************************/

/*----- Header-Files ---------------------------------------------------------*/
#include <stdbool.h>

#include "txw51_framework/hw/uart.h"
#include "txw51_framework/utils/txw51_errors.h"

/*----- Macros ---------------------------------------------------------------*/

/*----- Data types -----------------------------------------------------------*/

/*----- Function prototypes --------------------------------------------------*/

/*----- Data -----------------------------------------------------------------*/

/*----- Implementation -------------------------------------------------------*/
int main(void)
{
    struct TXW51_UART_Init uart_init = {
        .RxPin               = TXW51_GPIO_PIN_UART0_RX,
        .TxPin               = TXW51_GPIO_PIN_UART0_TX,
        .RtsPin              = 0,
        .CtsPin              = 0,
        .EnableHwFlowControl = false,
        .Baudrate            = TXW51_UART_BAUDRATE_38400
    };
    TXW51_UART_Init(&uart_init);

    TXW51_UART_WriteString((uint8_t *)"\r\n\r\n");
    TXW51_UART_WriteString((uint8_t *)"UART Echo-Test\r\n");

    uint32_t err = ERR_NONE;
    uint8_t character = 0;

    while (true) {
        err = TXW51_UART_Read(&character);

        if (err == ERR_NONE) {
            TXW51_UART_Write(character);
        }
    }

    return 0;
}

