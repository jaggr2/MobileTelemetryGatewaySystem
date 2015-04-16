/***************************************************************************//**
 * @brief   Bluetooth Smart "LSM330 Service".
 *
 * This service allows the user to configure the LSM330 sensor on the TXW51
 * hardware. This sensor consists of an accelerometer, an gyroscope and a
 * temperature sensor.
 *
 * @file    service_lsm330.h
 * @version 1.0
 * @date    13.11.2014
 * @author  Daniel Meer
 *
 * @remark  Last Modifications:
 *          13.11.2014 meerd1 created
 ******************************************************************************/

#ifndef TXW51_FRAMEWORK_BLE_SERVICE_LSM330_H_
#define TXW51_FRAMEWORK_BLE_SERVICE_LSM330_H_

/*----- Header-Files ---------------------------------------------------------*/
#include "txw51_framework/ble/service.h"

/*----- Macros ---------------------------------------------------------------*/

/*----- Data types -----------------------------------------------------------*/
/**
 * @brief The different event types that the service signals to the application.
 */
enum TXW51_SERV_LSM330_EventType {
    TXW51_SERV_LSM330_EVT_UNKNOWN,      /**< The event isn't known. Maybe from a damaged package. */
    TXW51_SERV_LSM330_EVT_ACC_EN,       /**< Enable/disable the acceleromter. */
    TXW51_SERV_LSM330_EVT_GYRO_EN,      /**< Enable/disable the gyroscope. */
    TXW51_SERV_LSM330_EVT_TEMP_SAMPLE,  /**< Read the temperature from the sensor. */
    TXW51_SERV_LSM330_EVT_ACC_FSCALE,   /**< Change the full scale of the accelerometer. */
    TXW51_SERV_LSM330_EVT_GYRO_FSCALE,  /**< Change the full scale of the gyroscope. */
    TXW51_SERV_LSM330_EVT_ACC_ODR,      /**< Change the ODR of the accelerometer. */
    TXW51_SERV_LSM330_EVT_GYRO_ODR,     /**< Change the ODR of the gyroscope. */
    TXW51_SERV_LSM330_EVT_TRIGGER_VAL,  /**< Set a value to trigger the sensor. */
    TXW51_SERV_LSM330_EVT_TRIGGER_AXIS  /**< Set the axis to trigger the sensor. */
};

/**
 * @brief An event structure that the service uses to signal the application
 * what happened.
 */
struct TXW51_SERV_LSM330_Event {
	enum TXW51_SERV_LSM330_EventType EventType; /**< Type of the event. */
	uint8_t  *Value;                            /**< Pointer to the value. */
	uint16_t Length;                            /**< Length of the value in byte. */
};

/**
 * @brief Forward declaration of the service handle for the callback.
 */
struct TXW51_SERV_LSM330_Handle;

/**
 * @brief Callback that gets called when the service wants to signal something
 * to the application.
 */
typedef void (*TXW51_SERV_LSM330_EventHandler_t) (struct TXW51_SERV_LSM330_Handle *handle,
		                                          struct TXW51_SERV_LSM330_Event  *event);

/**
 * @brief Structure with the initialization values.
 */
struct TXW51_SERV_LSM330_Init {
    TXW51_SERV_LSM330_EventHandler_t EventHandler;  /**< Callback to register. */
};

/**
 * @brief Handle that defines a specific instance of the service.
 */
struct TXW51_SERV_LSM330_Handle {
    struct TXW51_SERV_Handle    ServiceHandle;              /**< Handle of the service. */
    ble_gatts_char_handles_t    CharHandle_AccEnable;       /**< Handle of the Acc Enable characteristic. */
    ble_gatts_char_handles_t    CharHandle_GyroEnable;      /**< Handle of the Gyro Enable characteristic. */
    ble_gatts_char_handles_t    CharHandle_TempSample;      /**< Handle of the Temp Sample characteristic. */
    ble_gatts_char_handles_t    CharHandle_AccFscale;       /**< Handle of the Acc Full Scale characteristic. */
    ble_gatts_char_handles_t    CharHandle_GyroFscale;      /**< Handle of the Gyro Full Scale characteristic. */
    ble_gatts_char_handles_t    CharHandle_AccOdr;          /**< Handle of the Acc ODR characteristic. */
    ble_gatts_char_handles_t    CharHandle_GyroOdr;         /**< Handle of the Gyro ODR characteristic. */
    ble_gatts_char_handles_t    CharHandle_TriggerValue;    /**< Handle of the Trigger Value characteristic. */
    ble_gatts_char_handles_t    CharHandle_TriggerAxis;     /**< Handle of the Trigger Axis characteristic. */
    TXW51_SERV_LSM330_EventHandler_t EventHandler;          /**< Callback to the application. */
};

/*----- Function prototypes --------------------------------------------------*/

/***************************************************************************//**
* @brief Initializes the LSM330 service.
*
* @param[out] handle The handle for the service.
* @param[in]  init   Structure with the initialization values.
* @return ERR_NONE if no error occurred.
*         ERR_BLE_SERVICE_ADD_UUID if UUID could not be added.
*         ERR_BLE_SERVICE_ADD_SERVICE if service could not be added.
*         ERR_BLE_SERVICE_ADD_CHARACTERISTIC if characteristic could not be
*                                            added.
******************************************************************************/
extern uint32_t TXW51_SERV_LSM330_Init(struct TXW51_SERV_LSM330_Handle *handle,
                                       const struct TXW51_SERV_LSM330_Init *init);

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
extern void TXW51_SERV_LSM330_OnBleEvent(struct TXW51_SERV_LSM330_Handle *handle,
                                         ble_evt_t *bleEvent);

/*----- Data -----------------------------------------------------------------*/

#endif // TXW51_FRAMEWORK_BLE_SERVICE_LSM330_H_
