/***************************************************************************//**
 * @brief   Bluetooth Smart "Contact Less Temperature Measurment".
 *
 * This service allows the user to configure and start a new contact less temperature measurement with
 * the TXW51 sensor.
 *
 * @file    service_tempContactLess.h
 * @version 1.0
 * @date    10.04.2015
 * @author  Pascal Bohni
 *
 * @remark  Last Modifications:
 *          10.04.2015 bohnp1 created
 ******************************************************************************/

#ifndef TXW51_FRAMEWORK_BLE_SERVICE_TEMP_CONTACTLESS_H_
#define TXW51_FRAMEWORK_BLE_SERVICE_TEMP_CONTACTLESS_H_

/*----- Header-Files ---------------------------------------------------------*/
#include "txw51_framework/ble/service.h"

/*----- Macros ---------------------------------------------------------------*/

/*----- Data types -----------------------------------------------------------*/
/**
 * @brief The different event types that the service signals to the application.
 */
enum TXW51_SERV_TEMP_CONTACTLESS_EventType {
    TXW51_SERV_TEMP_CONTACTLESS_EVT_UNKNOWN,      	/**< The event isn't known. Maybe from a damaged package. */
    TXW51_SERV_TEMP_CONTACTLESS_EVT_TEMP_SAMPLE,   /**< Read the temperature from the sensor */
};

/**
 * @brief An event structure that the service uses to signal the application
 * what happened.
 */
struct TXW51_SERV_TEMP_CONTACTLESS_Event {
	enum TXW51_SERV_TEMP_CONTACTLESS_EventType EventType; /**< Type of the event. */
	uint8_t  *Value;                            /**< Pointer to the value. */
	uint16_t Length;                            /**< Length of the value in byte. */
};

/**
 * @brief Forward declaration of the service handle for the callback.
 */
struct TXW51_SERV_TEMP_CONTACTLESS_Handle;

/**
 * @brief Callback that gets called when the service wants to signal something
 * to the application.
 */
typedef void (*TXW51_SERV_TEMP_CONTACTLESS_EventHandler_t) (struct TXW51_SERV_TEMP_CONTACTLESS_Handle *handle,
		                                          struct TXW51_SERV_TEMP_CONTACTLESS_Event  *event);

/**
 * @brief Structure with the initialization values.
 */
struct TXW51_SERV_TEMP_CONTACTLESS_Init {
    TXW51_SERV_TEMP_CONTACTLESS_EventHandler_t EventHandler;  /**< Callback to register. */
};

/**
 * @brief Handle that defines a specific instance of the service.
 */
struct TXW51_SERV_TEMP_CONTACTLESS_Handle {
    struct TXW51_SERV_Handle    ServiceHandle;              /**< Handle of the service. */
    ble_gatts_char_handles_t    CharHandle_TempSample;      /**< Handle of the Temp Sample characteristic. */
    TXW51_SERV_TEMP_CONTACTLESS_EventHandler_t EventHandler;          /**< Callback to the application. */
};

/*----- Function prototypes --------------------------------------------------*/

/***************************************************************************//**
* @brief Initializes the contactless temperature service.
*
* @param[out] handle The handle for the service.
* @param[in]  init   Structure with the initialization values.
* @return ERR_NONE if no error occurred.
*         ERR_BLE_SERVICE_ADD_UUID if UUID could not be added.
*         ERR_BLE_SERVICE_ADD_SERVICE if service could not be added.
*         ERR_BLE_SERVICE_ADD_CHARACTERISTIC if characteristic could not be
*                                            added.
******************************************************************************/
extern uint32_t TXW51_SERV_TEMP_CONTACTLESS_Init(struct TXW51_SERV_TEMP_CONTACTLESS_Handle *handle,
                                       const struct TXW51_SERV_TEMP_CONTACTLESS_Init *init);

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
extern void TXW51_SERV_TEMP_CONTACTLESS_OnBleEvent(struct TXW51_SERV_TEMP_CONTACTLESS_Handle *handle,
                                         ble_evt_t *bleEvent);

/*----- Data -----------------------------------------------------------------*/

#endif // TXW51_FRAMEWORK_BLE_SERVICE_TEMP_CONTACTLESS_H_
