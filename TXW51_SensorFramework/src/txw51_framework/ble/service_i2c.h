/***************************************************************************//**
 * @brief   Bluetooth Smart I2C BLE Bridge.
 *
 * This service allows the user to configure and to transmit i2c messages with
 * the TXW51 sensor.
 *
 * @file    service_i2c.h
 * @version 1.0
 * @date    08.05.2015
 * @author  Pascal Bohni
 *
 * @remark  Last Modifications:
 *          08.05.2015 bohnp1 created
 ******************************************************************************/

#ifndef TXW51_FRAMEWORK_BLE_SERVICE_I2C_H_
#define TXW51_FRAMEWORK_BLE_SERVICE_I2C_H_

/*----- Header-Files ---------------------------------------------------------*/
#include "txw51_framework/ble/service.h"

/*----- Macros ---------------------------------------------------------------*/
#define TXW51_SERV_I2C_VALUE_MAX_LENGTH     ( 10 )      /**< Maximum length of a device I2C Value */
/*----- Data types -----------------------------------------------------------*/
/**
 * @brief The different event types that the service signals to the application.
 */
enum TXW51_SERV_I2C_EventType {
    TXW51_SERV_I2C_EVT_UNKNOWN,      			/**< The event isn't known. Maybe from a damaged package. */
    TXW51_SERV_I2C_EVT_ADRESS,   				/**< Set the address of the I2C device */
    TXW51_SERV_I2C_EVT_REGISTER,				/**< Set the register of the I2C device to write/read */
    TXW51_SERV_I2C_EVT_VALUE_READ,				/**< Read the value from specified I2C device */
    TXW51_SERV_I2C_EVT_VALUE_WRITE,				/**< Write the value to specified I2C device */
    TXW51_SERV_I2C_EVT_VALUE_LENGTH				/**< Number of Bytes for the value to read */

};

/**
 * @brief An event structure that the service uses to signal the application
 * what happened.
 */
struct TXW51_SERV_I2C_Event {
	enum TXW51_SERV_I2C_EventType EventType; 	/**< Type of the event. */
	uint8_t  *Value;                            /**< Pointer to the value. */
	uint16_t Length;                            /**< Length of the value in byte. */
};

/**
 * @brief Forward declaration of the service handle for the callback.
 */
struct TXW51_SERV_I2C_Handle;

/**
 * @brief Callback that gets called when the service wants to signal something
 * to the application.
 */
typedef void (*TXW51_SERV_I2C_EventHandler_t) (struct TXW51_SERV_I2C_Handle *handle,
		                                          struct TXW51_SERV_I2C_Event  *event);

/**
 * @brief Structure with the initialization values.
 */
struct TXW51_SERV_I2C_Init {
    TXW51_SERV_I2C_EventHandler_t EventHandler;  /**< Callback to register. */
};

/**
 * @brief Handle that defines a specific instance of the service.
 */
struct TXW51_SERV_I2C_Handle {
    struct TXW51_SERV_Handle    ServiceHandle;              /**< Handle of the service. */
    ble_gatts_char_handles_t    CharHandle_I2CValue;	      	/**< Handle of the I2C characteristic. */
    ble_gatts_char_handles_t    CharHandle_I2CAddress;	      	/**< Handle of the I2C characteristic. */
    ble_gatts_char_handles_t    CharHandle_I2CRegister;	      	/**< Handle of the I2C characteristic. */
    ble_gatts_char_handles_t    CharHandle_I2CLength;	      	/**< Handle of the I2C characteristic. */
    TXW51_SERV_I2C_EventHandler_t EventHandler;    				/**< Callback to the application. */
};

/*----- Function prototypes --------------------------------------------------*/

/***************************************************************************//**
* @brief Initializes the i2c service.
*
* @param[out] handle The handle for the service.
* @param[in]  init   Structure with the initialization values.
* @return ERR_NONE if no error occurred.
*         ERR_BLE_SERVICE_ADD_UUID if UUID could not be added.
*         ERR_BLE_SERVICE_ADD_SERVICE if service could not be added.
*         ERR_BLE_SERVICE_ADD_CHARACTERISTIC if characteristic could not be
*                                            added.
******************************************************************************/
extern uint32_t TXW51_SERV_I2C_Init(struct TXW51_SERV_I2C_Handle *handle,
                                       const struct TXW51_SERV_I2C_Init *init);

/***************************************************************************//**
* @brief BLE event callback for this service.
*
* The application has to call this function every time a BLE event occurs.
* So that the service can operate accordingly.
*
* @param[in,out] handle   The handle for the service.
* @param[in]     bleEvent The BLE event that occurred.
* @return Nothing.
******************************************************************************/
extern void TXW51_SERV_I2C_OnBleEvent(struct TXW51_SERV_I2C_Handle *handle,
                                         ble_evt_t *bleEvent);

/*----- Data -----------------------------------------------------------------*/

#endif // TXW51_FRAMEWORK_BLE_SERVICE_I2C_H_
