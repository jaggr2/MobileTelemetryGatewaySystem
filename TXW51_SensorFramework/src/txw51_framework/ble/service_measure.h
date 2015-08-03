/***************************************************************************//**
 * @brief   Bluetooth Smart "Measurement Service".
 *
 * This service allows the user to configure and start a new measurement with
 * the TXW51 sensor.
 *
 * @file    service_measure.h
 * @version 1.0
 * @date    09.12.2014
 * @author  Daniel Meer
 *
 * @remark  Last Modifications:
 *          09.12.2014 meerd1 created
 ******************************************************************************/

#ifndef TXW51_FRAMEWORK_BLE_SERVICE_MEASURE_H_
#define TXW51_FRAMEWORK_BLE_SERVICE_MEASURE_H_

/*----- Header-Files ---------------------------------------------------------*/
#include "txw51_framework/ble/service.h"

/*----- Macros ---------------------------------------------------------------*/

/*----- Data types -----------------------------------------------------------*/
/**
 * @brief The different type how we can send data to the peer device.
 */
enum TXW51_SERV_MEASURE_TxType {
    TXW51_SERV_MEASURE_TX_NOTIFICATION = BLE_GATT_HVX_NOTIFICATION, /**< Send data via notifications. */
    TXW51_SERV_MEASURE_TX_INDICATION   = BLE_GATT_HVX_INDICATION    /**< Send data via indications. */
};

/**
 * @brief The different axes that can be specified in a data package header.
 *        Use the OR operation for multiple axes.
 */
enum TXW51_SERV_MEASURE_DataPackageAxisType {
    TXW51_SERV_MEASURE_DATA_AXIS_X = 0x01U, /**< Specify X axis. */
    TXW51_SERV_MEASURE_DATA_AXIS_Y = 0x02U, /**< Specify Y axis. */
    TXW51_SERV_MEASURE_DATA_AXIS_Z = 0x04U  /**< Specify Z axis. */
};

/**
 * @brief The two sensor sources that can be specified in a data package header.
 */
enum TXW51_SERV_MEASURE_DataPackageSensorType {
    TXW51_SERV_MEASURE_DATA_SENSOR_ACC  = 0x00U, /**< Specify accelerometer. */
    TXW51_SERV_MEASURE_DATA_SENSOR_GYRO = 0x01U, /**< Specify gyroscope. */
};

/**
 * @brief The different event types that the service signals to the application.
 */
enum TXW51_SERV_MEASURE_EventType {
    TXW51_SERV_MEASURE_EVT_UNKNOWN,             /**< The event isn't known. Maybe from a damaged package.. */
    TXW51_SERV_MEASURE_EVT_START,               /**< Start the measurement. */
    TXW51_SERV_MEASURE_EVT_STOP,                /**< Stop the measurement. */
    TXW51_SERV_MEASURE_EVT_SET_DURATION,        /**< Set duration of a measurement. */
    TXW51_SERV_MEASURE_EVT_ENABLE_DATASTREAM,   /**< CCCD for data streaming has been set. */
    TXW51_SERV_MEASURE_EVT_DISABLE_DATASTREAM,  /**< CCCD for data streaming has been unset. */
    TXW51_SERV_MEASURE_EVT_INDICATION_RECEIVED, /**< The indication has been received by the peer device. */
    TXW51_SERV_MEASURE_EVT_NOTIFICATIONS_SENT,  /**< The notification has been sent (no guarantee of receiving). */
    TWX51_SERV_MEASURE_EVT_ADC					/**< Get Value from ADC */
};

/**
 * @brief An event structure that the service uses to signal the application
 * what happened.
 */
struct TXW51_SERV_MEASURE_Event {
    enum TXW51_SERV_MEASURE_EventType EventType;    /**< Type of the event. */
    uint8_t  *Value;                                /**< Pointer to the value. */
    uint16_t Length;                                /**< Length of the value in byte. */
};

/**
 * @brief Forward declaration of the service handle for the callback.
 */
struct TXW51_SERV_MEASURE_Handle;

/**
 * @brief Callback that gets called when the service wants to signal something
 * to the application.
 */
typedef void (*TXW51_SERV_MEASURE_EventHandler_t) (struct TXW51_SERV_MEASURE_Handle *handle,
                                                   struct TXW51_SERV_MEASURE_Event  *event);

/**
 * @brief Structure with the initialization values.
 */
struct TXW51_SERV_MEASURE_Init {
    TXW51_SERV_MEASURE_EventHandler_t EventHandler; /**< Callback to register. */
};

/**
 * @brief Handle that defines a specific instance of the service.
 */
struct TXW51_SERV_MEASURE_Handle {
    struct TXW51_SERV_Handle    ServiceHandle;          /**< Handle of the service. */
    ble_gatts_char_handles_t    CharHandle_Start;       /**< Handle of the Start characteristic. */
    ble_gatts_char_handles_t    CharHandle_Stop;        /**< Handle of the Stop characteristic. */
    ble_gatts_char_handles_t    CharHandle_Duration;    /**< Handle of the Duration characteristic. */
    ble_gatts_char_handles_t    CharHandle_DataStream;  /**< Handle of the Data Stream characteristic. */
    ble_gatts_char_handles_t    CharHandle_ADC;  		/**< Handle of the ADC characteristic. */
    TXW51_SERV_MEASURE_EventHandler_t EventHandler;     /**< Callback to the application. */
};

/**
 * @brief Data packet that gets sent over the Bluetooth Smart link.
 */
struct TXW51_SERV_MEASURE_DataPacket {
    struct {
        uint8_t NumberOfSamples:4;  /**< bit: 0..3  Number of samples in the package. */
        uint8_t Axis:3;             /**< bit: 4..6  Which axis are sent. Bit (4,5,6) for (X,Y,Z) */
        uint8_t AccOrGyro:1;        /**< bit:    7  Which sensor generated the data. 0 for acc, 1 for gyro. */
    } Header;                       /**< The header of a data package. */
    uint8_t Number;                 /**< An 8 bit sequence number for the package. */
    uint8_t Data[18];               /**< The data bytes. */
};

/*----- Function prototypes --------------------------------------------------*/

/***************************************************************************//**
* @brief Initializes the Measurement service.
*
* @param[out] handle The handle for the service.
* @param[in]  init   Structure with the initialization values.
* @return ERR_NONE if no error occurred.
*         ERR_BLE_SERVICE_ADD_UUID if UUID could not be added.
*         ERR_BLE_SERVICE_ADD_SERVICE if service could not be added.
*         ERR_BLE_SERVICE_ADD_CHARACTERISTIC if characteristic could not be
*                                            added.
******************************************************************************/
extern uint32_t TXW51_SERV_MEASURE_Init(struct TXW51_SERV_MEASURE_Handle *handle,
                                        const struct TXW51_SERV_MEASURE_Init *init);

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
extern void TXW51_SERV_MEASURE_OnBleEvent(struct TXW51_SERV_MEASURE_Handle *handle,
                                          ble_evt_t *bleEvent);

/***************************************************************************//**
* @brief Sends new data via indication or notification to the peer device.
*
* For this to function the CCCD of the Data Stream characteristic has to be
* previously set by the peer device. We send always a full package of 20 byte.
*
* @param[in,out] txType Sets whether an indication or a notification shall be
*                       used to send the data.
* @param[in,out] handle The handle for the service.
* @param[in,out] data The data to send.
* @return ERR_NONE if no error occurred.
*         ERR_SERVICE_MEASURE_HVC_COULD_NOT_SEND if notification or indication
*                                                could not be sent.
*         ERR_SERVICE_MEASURE_CCCD_NOT_ENABLED if CCCD is not enabled.
******************************************************************************/
extern uint32_t TXW51_SERV_MEASURE_SendData(enum TXW51_SERV_MEASURE_TxType txType,
                                            struct TXW51_SERV_MEASURE_Handle *handle,
                                            struct TXW51_SERV_MEASURE_DataPacket *data);

/*----- Data -----------------------------------------------------------------*/

#endif /* TXW51_FRAMEWORK_BLE_SERVICE_MEASURE_H_ */
