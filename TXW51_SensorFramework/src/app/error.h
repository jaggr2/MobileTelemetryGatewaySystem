/***************************************************************************//**
 * @brief   Defines the error codes for the application.
 *
 * @file    error.h
 * @version 1.0
 * @date    17.11.2014
 * @author  Daniel Meer
 *
 * @remark  Last Modifications:
 *          17.11.2014 meerd1 created
 ******************************************************************************/

#ifndef TXW51_APPLICATION_ERROR_H_
#define TXW51_APPLICATION_ERROR_H_

/*----- Header-Files ---------------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>

#include "txw51_framework/utils/txw51_errors.h"

/*----- Macros ---------------------------------------------------------------*/
#define APPL_ERROR_BASE_NUM         ( 0x6000U )     /**< Base of the errors the distinguish from other error codes. */

/*----- Data types -----------------------------------------------------------*/
/**
 * @brief List of the available error codes.
 */
enum appl_errors {
    ERR_DEVINFO_OPERATION_PENDING = APPL_ERROR_BASE_NUM,    /**< Could not execute. because flash is busy. */
    ERR_DEVINFO_DATA_NOT_LOADED,                            /**< The data has not been loaded from flash yet. */
    ERR_DEVINFO_FLASH_INIT_FAILED,                          /**< Flash initialization failed. */
    ERR_DEVINFO_FLASH_LOAD_FAILED,                          /**< Reading from flash has failed. */
    ERR_DEVINFO_FLASH_SAVE_FAILED,                          /**< Writing to flash has failed. */

    ERR_BLE_SERVICE_INIT_FAILED,                            /**< The initialization of the BLE service has failed. */

    ERR_ADC_EXMPL_INIT_FAILED,                              /**< The initialization of the ADC example has failed. */
    ERR_ADC_EXMPL_START_FAILED,                             /**< Could not start the ADC. */

    ERR_TIMER_INIT_FAILED,                                  /**< The initialization of the timer has failed. */
    ERR_TIMER_START_FAILED,                                 /**< Starting the timeout timer has failed. */
    ERR_TIMER_STOP_FAILED,                                  /**< Stopping the timeout timer has failed. */

    ERR_FIFO_INIT_FAILED,                                   /**< The initialization of the FIFO failed. */
    ERR_FIFO_PUT_FAILED,                                    /**< Could not put values into the FIFO. */
    ERR_FIFO_GET_FAILED,                                    /**< Could not get values from the FIFO. */
};

/*----- Function prototypes --------------------------------------------------*/

/***************************************************************************//**
 * @brief Function for error handling, which is called when an error has
 *        occurred.
 *
 * This handler is part of the examples from the Nordic SDK. The final product
 * should not use them.
 *
 * @param[in] errorCode  Error code supplied to the handler.
 * @param[in] lineNumber Line number where the handler is called.
 * @param[in] fileName   Pointer to the file name.
 *
 * @return Nothing.
 ******************************************************************************/
extern void app_error_handler(uint32_t errorCode,
                              uint32_t lineNumber,
                              const uint8_t *fileName);

/***************************************************************************//**
 * @brief Callback function for asserts in the SoftDevice.
 *
 * This function will be called in case of an assert in the SoftDevice.
 *
 * This handler is part of the examples from the Nordic SDK. The final product
 * should not use them.
 *
 * On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param[in] lineNumber Line number of the failing ASSERT call.
 * @param[in] fileName   File name of the failing ASSERT call.
 *
 * @return Nothing.
 ******************************************************************************/
extern void assert_nrf_callback(uint16_t lineNumber, const uint8_t *fileName);

/*----- Data -----------------------------------------------------------------*/

#endif /* TXW51_APPLICATION_ERROR_H_ */
