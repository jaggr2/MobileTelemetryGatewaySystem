/***************************************************************************//**
 * @brief   Defines the error codes for the TXW51 sensor framework.
 *
 * @file    txw51_errors.h
 * @version 1.0
 * @date    03.01.2015
 * @author  Daniel Meer
 *
 * @remark  Last Modifications:
 *          03.01.2015 meerd1 created
 ******************************************************************************/

#ifndef TXW51_FRAMEWORK_UTILS_TXW51_ERRORS_H_
#define TXW51_FRAMEWORK_UTILS_TXW51_ERRORS_H_

/*----- Header-Files ---------------------------------------------------------*/

/*----- Macros ---------------------------------------------------------------*/
#define TXW51_ERROR_BASE_NUM        ( 0x5000U )     /**< Base of the errors the distinguish from other error codes. */

/*----- Data types -----------------------------------------------------------*/
/**
 * @brief List of the available error codes.
 */
enum txw51_errors {
    ERR_NONE = TXW51_ERROR_BASE_NUM,        /**< No error. Function terminated successfully. */
    ERR_UNKNOWN,                            /**< Error is not known or function has a bug in the error handling. */

    ERR_SPI_INIT_FAILED,                    /**< Could not initialize the SPI interface. */
    ERR_SPI_READ_FAILED,                    /**< Could not read from the SPI interface. */
    ERR_SPI_WRITE_FAILED,                   /**< Could not write to the SPI interface. */

    ERR_LSM330_READ_FAILED,                 /**< Could not read from the LSM330 sensor. */
    ERR_LSM330_WRITE_FAILED,                /**< Could not write to the LSM330 sensor. */

    ERR_BLE_SERVICE_NO_CONNECTION,          /**< Bluetooth Smart has no connection. */
    ERR_BLE_SERVICE_ADD_UUID,               /**< Could not add the UUID to a BLE service. */
    ERR_BLE_SERVICE_ADD_SERVICE,            /**< Could not add a service to the BLE stack. */
    ERR_BLE_SERVICE_ADD_CHARACTERISTIC,     /**< Could not add a characteristic to the BLE stack. */

    ERR_SERVICE_MEASURE_HVC_COULD_NOT_SEND, /**< Could not send a HVC event (indication or notification). */
    ERR_SERVICE_MEASURE_CCCD_NOT_ENABLED,   /**< Could not send a HVC event because CCCD was not set by the peer device. */

    ERR_UART_READ_FAILED,                   /**< Could not read from the UART interface. */
    ERR_UART_WRITE_FAILED,                  /**< Could not write to the UART interface. */
};

/*----- Function prototypes --------------------------------------------------*/

/*----- Data -----------------------------------------------------------------*/

#endif /* TXW51_FRAMEWORK_UTILS_TXW51_ERRORS_H_ */
