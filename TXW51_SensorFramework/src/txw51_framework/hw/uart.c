/***************************************************************************//**
 * @brief   Module to initialize and use the UART of the TXW51.
 *
 * @file    uart.c
 * @version 1.0
 * @date    17.11.2014
 * @author  Daniel Meer
 *
 * @remark  Last Modifications:
 *          17.11.2014 meerd1 created
 ******************************************************************************/

/*----- Header-Files ---------------------------------------------------------*/
#include "uart.h"

#include "nrf/nrf.h"

#include "txw51_framework/utils/nrf_delay.h"
#include "txw51_framework/utils/txw51_errors.h"

/*----- Macros ---------------------------------------------------------------*/

/*----- Data types -----------------------------------------------------------*/

/*----- Function prototypes --------------------------------------------------*/

/*----- Data -----------------------------------------------------------------*/
static struct TXW51_UART_Init initValues;   /**< Keep copy of init values for deinitialization. */

/*----- Implementation -------------------------------------------------------*/

void TXW51_UART_Init(struct TXW51_UART_Init *init)
{
    TXW51_GPIO_ConfigGpioAsOutput(init->TxPin);
    TXW51_GPIO_ConfigGpioAsInput(init->RxPin, NRF_GPIO_PIN_NOPULL);
    NRF_UART0->PSELTXD = init->TxPin;
    NRF_UART0->PSELRXD = init->RxPin;

    if (init->EnableHwFlowControl) {
        TXW51_GPIO_ConfigGpioAsOutput(init->RtsPin);
        TXW51_GPIO_ConfigGpioAsInput(init->CtsPin, NRF_GPIO_PIN_NOPULL);
        NRF_UART0->PSELCTS = init->CtsPin;
        NRF_UART0->PSELRTS = init->RtsPin;
        NRF_UART0->CONFIG  = (UART_CONFIG_HWFC_Enabled << UART_CONFIG_HWFC_Pos);
    }

    NRF_UART0->BAUDRATE      = (init->Baudrate << UART_BAUDRATE_BAUDRATE_Pos);
    NRF_UART0->ENABLE        = (UART_ENABLE_ENABLE_Enabled << UART_ENABLE_ENABLE_Pos);
    NRF_UART0->TASKS_STARTTX = 1;
    NRF_UART0->TASKS_STARTRX = 1;
    NRF_UART0->EVENTS_RXDRDY = 0;

    initValues = *init;
}


void TXW51_UART_Deinit(void)
{
    NRF_UART0->TASKS_STOPTX = 1;
    NRF_UART0->TASKS_STOPRX = 1;
    NRF_UART0->ENABLE       = (UART_ENABLE_ENABLE_Disabled << UART_ENABLE_ENABLE_Pos);

    TXW51_GPIO_ConfigGpioAsDisconnected(initValues.TxPin);
    TXW51_GPIO_ConfigGpioAsDisconnected(initValues.RxPin);

    if (initValues.EnableHwFlowControl) {
        TXW51_GPIO_ConfigGpioAsDisconnected(initValues.RtsPin);
        TXW51_GPIO_ConfigGpioAsDisconnected(initValues.CtsPin);
    }
}


uint32_t TXW51_UART_Read(uint8_t *value)
{
    /* Wait for RXD data to be received. */
    for (int32_t i = 0; NRF_UART0->EVENTS_RXDRDY != 1; i++) {
        if (i >= TXW51_UART_WAIT_TIMEOUT) {
            return ERR_UART_READ_FAILED;
        }
    }

    /* Clear the event and set rx_data with received byte. */
    NRF_UART0->EVENTS_RXDRDY = 0;
    *value = (uint8_t)NRF_UART0->RXD;
    return ERR_NONE;
}


uint32_t TXW51_UART_Write(uint8_t value)
{
    NRF_UART0->TXD = value;

    /* Wait for TXD data to be sent. */
    for (int32_t i = 0; NRF_UART0->EVENTS_TXDRDY != 1; i++) {
        if (i >= TXW51_UART_WAIT_TIMEOUT) {
            return ERR_UART_WRITE_FAILED;
        }
    }

    NRF_UART0->EVENTS_TXDRDY = 0;
    return ERR_NONE;
}


uint32_t TXW51_UART_WriteString(const uint8_t *message)
{
    uint32_t err = ERR_NONE;
    uint_fast8_t i = 0;
    uint8_t character = message[i++];

    while (character != '\0') {
        err = TXW51_UART_Write(character);
        if (err != ERR_NONE) {
            return err;
        }
        character = message[i++];
    }
    return ERR_NONE;
}

