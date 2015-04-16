/***************************************************************************//**
 * @brief   Functions to create Bluetooth Smart Services.
 *
 * This module allows the creation and handling of Bluetooth Smart Services.
 * Furthermore it includes helper functions for often used operations.
 *
 * @file    service.h
 * @version 1.0
 * @date    18.11.2014
 * @author  Daniel Meer
 *
 * @remark  Last Modifications:
 *          18.11.2014 meerd1 created
 ******************************************************************************/

#ifndef TXW51_FRAMEWORK_BLE_SERVICE_H_
#define TXW51_FRAMEWORK_BLE_SERVICE_H_

/*----- Header-Files ---------------------------------------------------------*/
#include "nrf/s110/ble.h"
#include "nrf/ble/ble_services/ble_srv_common.h"

/*----- Macros ---------------------------------------------------------------*/

/*----- Data types -----------------------------------------------------------*/
/**
 * @brief Structure for a characteristic initialization values.
 */
struct TXW51_SERV_CharInit {
    ble_gatts_char_md_t Metadata;       /**< GATT meta data. */
    ble_gatts_attr_t    Attribute;      /**< GATT attribute. */
    ble_gatts_attr_md_t AttrMetadata;   /**< Attribute meta data. */
    ble_uuid_t          BleUuid;        /**< Characteristic UUID (16 bit). */
};

/**
 * @brief Structure for a service initialization values.
 */
struct TXW51_SERV_ServiceInit {
    ble_uuid128_t BaseUuid;     /**< Base UUID (128 bit). */
    uint16_t      ServiceUuid;  /**< Service UUID (16 bit). */
    bool          IsSecondary;  /**< Primary or secondary service. */
};

/**
 * @brief Handle that defines a specific instance of a service.
 */
struct TXW51_SERV_Handle {
    uint16_t ServiceHandle; /**< Handle of the service. */
    uint8_t  UuidType;      /**< Type information of the UUID. */
    uint16_t ConnHandle;    /**< Handle of the current connection (is BLE_CONN_HANDLE_INVALID if not connected). */
};


/*----- Function prototypes --------------------------------------------------*/

/***************************************************************************//**
* @brief Creates a new service.
*
* Adds a new service and its UUID to the Bluetooth Stack.
*
* @param[in]  init   Structure with the initialization values.
* @param[out] handle The handle for the service.
* @return ERR_NONE if no error occurred.
*         ERR_BLE_SERVICE_ADD_UUID if UUID could not be added.
*         ERR_BLE_SERVICE_ADD_SERVICE if service could not be added.
******************************************************************************/
extern uint32_t TXW51_SERV_Create(struct TXW51_SERV_ServiceInit *init,
                                  struct TXW51_SERV_Handle *handle);

/***************************************************************************//**
* @brief Initializes the characteristics init structure.
*
* This function fills the characteristics init structure with sensible defaults,
* which can in a next step be changed if needed. Furthermore, it correctly sets
* the UUID for the characteristic.
*
* @param[in]  serviceHandle The handle for the service.
* @param[in]  charUuid      UUID of the characteristic.
* @param[out] charac        Characteristic init structure to fill.
* @return Nothing.
******************************************************************************/
extern void TXW51_SERV_InitChar(struct TXW51_SERV_Handle *serviceHandle,
                                uint16_t charUuid,
                                struct TXW51_SERV_CharInit *charac);

/***************************************************************************//**
* @brief Adds a characteristic to the service in the Bluetooth stack.
*
* @param[in]  serviceHandle The handle for the service.
* @param[in]  charInit      The characteristics init structure.
* @param[out] charHandle    The handle for the characteristic.
* @return ERR_NONE if no error occurred.
*         ERR_BLE_SERVICE_ADD_CHARACTERISTIC if characteristic could not be
*                                            added.
******************************************************************************/
extern uint32_t TXW51_SERV_AddChar(struct TXW51_SERV_Handle *serviceHandle,
                                   struct TXW51_SERV_CharInit *charInit,
                                   ble_gatts_char_handles_t *charHandle);

/***************************************************************************//**
* @brief BLE event callback for all services.
*
* The service has to call this function every time a BLE event occurs.
* So that the service can operate accordingly.
*
* @param[in,out] handle   The handle for the service.
* @param[in]     bleEvent The BLE event that occurred.
* @return Nothing.
******************************************************************************/
extern void TXW51_SERV_OnBleEvent(struct TXW51_SERV_Handle *handle,
                                  ble_evt_t *bleEvent);

/***************************************************************************//**
* @brief Helper function that adds a user description to a characteristic.
*
* @param[out] charHandle  The handle for the characteristic.
* @param[in]  description The description.
* @return Nothing.
******************************************************************************/
extern void TXW51_SERV_AddUserDescription(struct TXW51_SERV_CharInit *charHandle,
                                          uint8_t *description);

/*----- Data -----------------------------------------------------------------*/

#endif /* TXW51_FRAMEWORK_BLE_SERVICE_H_ */
