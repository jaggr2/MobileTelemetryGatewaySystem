/***************************************************************************//**
 * @brief   Module that handles the device information values.
 *
 * Loads and saves the device information from and to the flash. It has also
 * default values for new devices.
 *
 * @file    device_info.h
 * @version 1.0
 * @date    04.12.2014
 * @author  Daniel Meer
 *
 * @remark  Last Modifications:
 *          04.12.2014 meerd1 created
 ******************************************************************************/

#ifndef TXW51_APPLICATION_DEVICE_INFO_H_
#define TXW51_APPLICATION_DEVICE_INFO_H_

/*----- Header-Files ---------------------------------------------------------*/
#include <stdint.h>

#include "txw51_framework/ble/service_dis.h"

/*----- Macros ---------------------------------------------------------------*/
#define APPL_DEVINFO_NUM_OF_ENTRIES     ( 6 )   /**< Number of device information entries. */
#define APPL_DEVINFO_ENTRY_LENGHT       ( 32 )  /**< Maximal length of a device information entry (has to be a multiple of 4). */
#define APPL_DEVINFO_FLAG_LENGTH		( 4 )   /**< length of DeviceInfo Flags (has to be a multiple of 4). */

#define APPL_DEVINFO_DEFAULT_MANUFACTURER   "Berner FH"     /**< Default manufacturer entry. */
#define APPL_DEVINFO_DEFAULT_MODEL          "TXW51"         /**< Default model name entry. */
#define APPL_DEVINFO_DEFAULT_SERIAL         "000000"        /**< Default serial number entry. */
#define APPL_DEVINFO_DEFAULT_HW_REV         "1.0"           /**< Default hardware revision entry. */
#define APPL_DEVINFO_DEFAULT_FW_REV         "1.0"           /**< Default firmware revision entry. */
#define APPL_DEVINFO_DEFAULT_DEVICE_NAME    "Jack"          /**< Default device name entry. */

#define APPL_DEVINFO_FLAG_POWER_SAVE_DIS	0x01			/**< Flag to disable Power Save Mode */

/*----- Data types -----------------------------------------------------------*/
/**
 * @brief List of the different device information entries.
 */
enum appl_devinfo_value {
    APPL_DEVINFO_VALUE_MANUFACTURER = 0,    /**< The manufacturer of the device. */
    APPL_DEVINFO_VALUE_MODEL        = 1,    /**< The model name. */
    APPL_DEVINFO_VALUE_SERIAL       = 2,    /**< The serial number. */
    APPL_DEVINFO_VALUE_HW_REV       = 3,    /**< The hardware revision. */
    APPL_DEVINFO_VALUE_FW_REV       = 4,    /**< The firmware revision. */
    APPL_DEVINFO_VALUE_DEVICE_NAME  = 5,    /**< The device name. */
    APPL_DEVINFO_VALUE_POWER_SAVE	= 6		/**< Flag if Power save mode is disabled. */
};

/*----- Function prototypes --------------------------------------------------*/

/***************************************************************************//**
 * @brief Initializes the device information module and the flash.
 *
 * @return ERR_NONE if no error occurred.
 *         ERR_DEVINFO_FLASH_INIT_FAILED if initialization of the flash failed.
 ******************************************************************************/
extern uint32_t APPL_DEVINFO_Init(void);

/***************************************************************************//**
 * @brief Loads the device information from the flash.
 *
 * @return ERR_NONE if no error occurred.
 *         ERR_DEVINFO_OPERATION_PENDING if already another flash operation is
 *                                       pending.
 *         ERR_DEVINFO_FLASH_LOAD_FAILED if values could not be read from flash.
 ******************************************************************************/
extern uint32_t APPL_DEVINFO_Load(void);

/***************************************************************************//**
 * @brief Prints the device information values to the log.
 *
 * @return ERR_NONE if no error occurred.
 *         ERR_DEVINFO_DATA_NOT_LOADED if data has not been loaded from flash.
 ******************************************************************************/
extern uint32_t APPL_DEVINFO_PrintValues(void);

/***************************************************************************//**
 * @brief Saves the device information to the flash.
 *
 * @return ERR_NONE if no error occurred.
 *         ERR_DEVINFO_OPERATION_PENDING if already another flash operation is
 *                                       pending.
 *         ERR_DEVINFO_FLASH_SAVE_FAILED if values could not be written to the
 *                                       flash.
 ******************************************************************************/
extern uint32_t APPL_DEVINFO_Save(void);

/***************************************************************************//**
 * @brief Sets the value of one device info.
 *
 * @param[in] entry Which entry to change.
 * @param[in] value The new information value.
 *
 * @return ERR_NONE if no error occurred.
 *         ERR_DEVINFO_OPERATION_PENDING if a flash operation is pending.
 ******************************************************************************/
extern uint32_t APPL_DEVINFO_Set(enum appl_devinfo_value entry, char *value);

/***************************************************************************//**
 * @brief Gets the value of one device info entry.
 *
 * The entry gets copied into the supplied buffer.
 *
 * @param[in]  entry  Which entry to read.
 * @param[out] buffer Buffer to copy the information into.
 *
 * @return Nothing.
 ******************************************************************************/
extern void APPL_DEVINFO_Get(enum appl_devinfo_value entry, char *buffer);

/***************************************************************************//**
 * @brief Test if the device info module is busy with a flash operation.
 *
 * @return Number of pending flash operations if no error occurred.
 *         -1 if an error occurred.
 ******************************************************************************/
extern uint32_t APPL_DEVINFO_IsBusy(void);

/***************************************************************************//**
 * @brief Fill the device information array with the default values.
 *
 * @return Nothing.
 ******************************************************************************/
extern void APPL_DEVINFO_SetDefaultValues(void);

/***************************************************************************//**
 * @brief Initialize the device information Bluetooth Smart service.
 *
 * @param[out] serviceHandle The handle for the service.
 *
 * @return ERR_NONE if no error occurred.
 *         ERR_BLE_SERVICE_INIT_FAILED if service initialization failed.
 ******************************************************************************/
extern uint32_t APPL_DEVINFO_InitService(struct TXW51_SERV_DIS_Handle *serviceHandle);

/***************************************************************************//**
 * @brief Ask if Power Save Mode is deactivated
 *
 * @return ERR_NONE if no error occurred.
 *         ERR_DEVINFO_OPERATION_PENDING if already another flash operation is
 *                                       pending.
 *         ERR_DEVINFO_FLASH_LOAD_FAILED if values could not be read from flash.
 ******************************************************************************/
extern bool APPL_DEVINFO_IsPowerSaveDisabled(void);

/*----- Data -----------------------------------------------------------------*/

#endif /* TXW51_APPLICATION_DEVICE_INFO_H_ */
