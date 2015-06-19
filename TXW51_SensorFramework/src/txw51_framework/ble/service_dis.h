/***************************************************************************//**
 * @brief   Bluetooth Smart "Device Information Service".
 *
 * This service is slightly different to the standardized DIS service in that
 * it is tailored to our application and has additional strings.
 *
 * @file    service_dis.h
 * @version 1.0
 * @date    11.11.2014
 * @author  Daniel Meer
 *
 * @remark  Last Modifications:
 *          11.11.2014 meerd1 created
 ******************************************************************************/

#ifndef TXW51_FRAMEWORK_BLE_SERVICE_DIS_H_
#define TXW51_FRAMEWORK_BLE_SERVICE_DIS_H_

/*----- Header-Files ---------------------------------------------------------*/
#include "txw51_framework/ble/service.h"

/*----- Macros ---------------------------------------------------------------*/
/* TODO: Set globally once. */
#define TXW51_SERV_DIS_VALUE_MAX_LENGTH     ( 20 )      /**< Maximum length of a device information string. */

/*----- Data types -----------------------------------------------------------*/
/**
 * @brief The different event types that the service signals to the application.
 */
enum TXW51_SERV_DIS_EventType {
    TXW51_SERV_DIS_EVT_UNKNOWN,             /**< The event isn't known. Maybe from a damaged package. */
    TXW51_SERV_DIS_EVT_UPDATE_MANUFACTURER, /**< Manufacturer string has been changed. */
    TXW51_SERV_DIS_EVT_UPDATE_MODEL,        /**< Model string has been changed. */
    TXW51_SERV_DIS_EVT_UPDATE_SERIAL,       /**< Serial number string has been changed. */
    TXW51_SERV_DIS_EVT_UPDATE_HW_REV,       /**< Hardware revision string has been changed. */
    TXW51_SERV_DIS_EVT_UPDATE_FW_REV,       /**< Firmware revision string has been changed. */
    TXW51_SERV_DIS_EVT_UPDATE_DEVICE_NAME,  /**< Device name string has been changed. */
    TXW51_SERV_DIS_EVT_SAVE_VALUES,         /**< User wants the currents string to be saved. */
    TXW51_SERV_DIS_EVT_DISABLE_TIMER		/**< User deactivate power save mode. */
};

/**
 * @brief An event structure that the service uses to signal the application
 * what happened.
 */
struct TXW51_SERV_DIS_Event {
    enum TXW51_SERV_DIS_EventType EventType;    /**< Type of the event. */
    uint8_t  *Value;                            /**< Pointer to the value. */
    uint16_t Length;                            /**< Length of the value in byte. */
};

/**
 * @brief Forward declaration of the service handle for the callback.
 */
struct TXW51_SERV_DIS_Handle;

/**
 * @brief Callback that gets called when the service wants to signal something
 * to the application.
 */
typedef void (*TXW51_SERV_DIS_EventHandler_t) (struct TXW51_SERV_DIS_Handle *handle,
                                               struct TXW51_SERV_DIS_Event  *event);

/**
 * @brief Structure with the initialization values.
 */
struct TXW51_SERV_DIS_Init {
    TXW51_SERV_DIS_EventHandler_t EventHandler; /**< Callback to register. */
    uint8_t *String_Manufacturer;               /**< Initial Manufacturer string. */
    uint8_t *String_Model;                      /**< Initial Model string. */
    uint8_t *String_Serial;                     /**< Initial Serial number string. */
    uint8_t *String_HwRev;                      /**< Initial Hardware revision string. */
    uint8_t *String_FwRev;                      /**< Initial Firmware revision string. */
    uint8_t *String_DeviceName;                 /**< Initial Device name string. */
    uint8_t *String_DisablePowerSave;			/**< Initial Device Power save mode */
};

/**
 * @brief Handle that defines a specific instance of the service.
 */
struct TXW51_SERV_DIS_Handle {
    struct TXW51_SERV_Handle      ServiceHandle;            /**< Handle of the service. */
    ble_gatts_char_handles_t      CharHandle_Manufacturer;  /**< Handle of the Manufacturer characteristic. */
    ble_gatts_char_handles_t      CharHandle_Model;         /**< Handle of the Model characteristic. */
    ble_gatts_char_handles_t      CharHandle_Serial;        /**< Handle of the Serial number characteristic. */
    ble_gatts_char_handles_t      CharHandle_HwRev;         /**< Handle of the Hardware revision characteristic. */
    ble_gatts_char_handles_t      CharHandle_FwRev;         /**< Handle of the Firmware revision characteristic. */
    ble_gatts_char_handles_t      CharHandle_DeviceName;    /**< Handle of the Device name characteristic. */
    ble_gatts_char_handles_t      CharHandle_SaveValues;    /**< Handle of the Save values characteristic. */
    ble_gatts_char_handles_t	  CharHandle_DisableTimer;	/**< Handle of the Disable Timer characteristic. */
    TXW51_SERV_DIS_EventHandler_t EventHandler;             /**< Callback to the application. */
};

/*----- Function prototypes --------------------------------------------------*/

/***************************************************************************//**
* @brief Initializes the DIS service.
*
* @param[out] handle The handle for the service.
* @param[in]  init   Structure with the initialization values.
* @return ERR_NONE if no error occurred.
*         ERR_BLE_SERVICE_ADD_UUID if UUID could not be added.
*         ERR_BLE_SERVICE_ADD_SERVICE if service could not be added.
*         ERR_BLE_SERVICE_ADD_CHARACTERISTIC if characteristic could not be
*                                            added.
******************************************************************************/
extern uint32_t TXW51_SERV_DIS_Init(struct TXW51_SERV_DIS_Handle *handle,
                                    const struct TXW51_SERV_DIS_Init *init);

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
extern void TXW51_SERV_DIS_OnBleEvent(struct TXW51_SERV_DIS_Handle *handle,
                                      ble_evt_t *bleEvent);

/*----- Data -----------------------------------------------------------------*/

#endif /* TXW51_FRAMEWORK_BLE_SERVICE_DIS_H_ */
