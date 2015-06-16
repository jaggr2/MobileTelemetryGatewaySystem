/***************************************************************************//**
 * @brief   Module to initialize and use the UART of the TXW51.
 *
 * @file    uart.h
 * @version 1.0
 * @date    17.11.2014
 * @author  Daniel Meer
 *
 * @remark  Last Modifications:
 *          17.11.2014 meerd1 created
 ******************************************************************************/

#ifndef TXW51_FRAMEWORK_HW_UART_H_
#define TXW51_FRAMEWORK_HW_UART_H_

/*----- Header-Files ---------------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>

#include "nrf/nrf51.h"
#include "nrf/nrf51_bitfields.h"

#include "txw51_framework/hw/gpio.h"

/*----- Macros ---------------------------------------------------------------*/
#define TXW51_UART_WAIT_TIMEOUT      ( 100000 ) /**< Timeout for the module to wait during UART communication (for-loop). */

/*----- Data types -----------------------------------------------------------*/
/**
 * List of the available baudrates.
 */
enum TXW51_UART_Baudrate {
    TXW51_UART_BAUDRATE_1200   = UART_BAUDRATE_BAUDRATE_Baud1200,       /**< 1200 baud. */
    TXW51_UART_BAUDRATE_2400   = UART_BAUDRATE_BAUDRATE_Baud2400,       /**< 2400 baud. */
    TXW51_UART_BAUDRATE_4800   = UART_BAUDRATE_BAUDRATE_Baud4800,       /**< 4800 baud. */
    TXW51_UART_BAUDRATE_9600   = UART_BAUDRATE_BAUDRATE_Baud9600,       /**< 9600 baud. */
    TXW51_UART_BAUDRATE_14400  = UART_BAUDRATE_BAUDRATE_Baud14400,      /**< 14400 baud. */
    TXW51_UART_BAUDRATE_19200  = UART_BAUDRATE_BAUDRATE_Baud19200,      /**< 19200 baud. */
    TXW51_UART_BAUDRATE_28800  = UART_BAUDRATE_BAUDRATE_Baud28800,      /**< 28800 baud. */
    TXW51_UART_BAUDRATE_38400  = UART_BAUDRATE_BAUDRATE_Baud38400,      /**< 38400 baud. */
    TXW51_UART_BAUDRATE_57600  = UART_BAUDRATE_BAUDRATE_Baud57600,      /**< 57600 baud. */
    TXW51_UART_BAUDRATE_76800  = UART_BAUDRATE_BAUDRATE_Baud76800,      /**< 76800 baud. */
    TXW51_UART_BAUDRATE_115200 = UART_BAUDRATE_BAUDRATE_Baud115200,     /**< 115.2k baud. */
    TXW51_UART_BAUDRATE_230400 = UART_BAUDRATE_BAUDRATE_Baud230400,     /**< 230.4k baud. */
    TXW51_UART_BAUDRATE_250000 = UART_BAUDRATE_BAUDRATE_Baud250000,     /**< 250k baud. */
    TXW51_UART_BAUDRATE_460800 = UART_BAUDRATE_BAUDRATE_Baud460800,     /**< 460.8k baud. */
    TXW51_UART_BAUDRATE_921600 = UART_BAUDRATE_BAUDRATE_Baud921600,     /**< 921.6k baud. */
    TXW51_UART_BAUDRATE_1M     = UART_BAUDRATE_BAUDRATE_Baud1M          /**< 1M baud. */
};

/**
 * Structure with the initialization values.
 */
struct TXW51_UART_Init {
    uint8_t RxPin;                      /**< GPIO pin for UART RX. */
    uint8_t TxPin;                      /**< GPIO pin for UART TX. */
    uint8_t RtsPin;                     /**< GPIO pin for UART RTS. */
    uint8_t CtsPin;                     /**< GPIO pin for UART CTS. */
    bool    EnableHwFlowControl;        /**< Enable UART hardware flow control. */
    enum TXW51_UART_Baudrate Baudrate;  /**< Baudrate to use. */
};

/*----- Function prototypes --------------------------------------------------*/

/***************************************************************************//**
 * @brief Initializes the UART interface.
 *
 * @param[in] init Structure with the initialization values.
 *
 * @return Nothing.
 ******************************************************************************/
extern void TXW51_UART_Init(struct TXW51_UART_Init *init);

/***************************************************************************//**
 * @brief Deinitializes the UART interface.
 *
 * This can be used to save energy on the GPIO pins.
 *
 * @return Nothing.
 ******************************************************************************/
extern void TXW51_UART_Deinit(void);

/***************************************************************************//**
 * @brief Reads a character from the UART interface.
 *
 * @param[out] value Buffer to save the character.
 *
 * @return ERR_NONE if no error occurred.
 *         ERR_UART_READ_FAILED if the read failed.
 ******************************************************************************/
extern uint32_t TXW51_UART_Read(uint8_t *value);

/***************************************************************************//**
 * @brief Writes a character to the UART interface.
 *
 * @param[in] value Character to write.
 *
 * @return ERR_NONE if no error occurred.
 *         ERR_UART_WRITE_FAILED if the write failed.
 ******************************************************************************/
extern uint32_t TXW51_UART_Write(uint8_t value);

/***************************************************************************//**
 * @brief Writes a string to the UART interface.
 *
 * @param[in] message String to write.
 *
 * @return ERR_NONE if no error occurred.
 *         ERR_UART_WRITE_FAILED if the write failed.
 ******************************************************************************/
extern uint32_t TXW51_UART_WriteString(const uint8_t *message);

/*----- Data -----------------------------------------------------------------*/


#endif /* TXW51_FRAMEWORK_HW_UART_H_ */
