/***************************************************************************//**
 * @brief   Module that handles the device information values.
 *
 * Loads and saves the device information from and to the flash. It has also
 * default values for new devices.
 *
 * @file    device_info.c
 * @version 1.0
 * @date    04.12.2014
 * @author  Daniel Meer
 *
 * @remark  Last Modifications:
 *          04.12.2014 meerd1 created
 ******************************************************************************/

/*----- Header-Files ---------------------------------------------------------*/
#include "device_info.h"

#include <string.h>

#include "nrf/app_common/pstorage.h"
#include "nrf/nordic_common.h"

#include "txw51_framework/utils/log.h"

#include "app/error.h"

/*----- Macros ---------------------------------------------------------------*/

/*----- Data types -----------------------------------------------------------*/
/**
 * @brief Length of the buffer with the information values.
 */
#define APPL_DEVINFO_INFO_BUFFER_LENGTH     ( APPL_DEVINFO_NUM_OF_ENTRIES * \
                                              APPL_DEVINFO_ENTRY_LENGHT )

/**
 * @brief Length of the buffer to display one information entry on the log.
 */
#define APPL_DEVINFO_OUTPUT_BUFFER_LENGTH   ( APPL_DEVINFO_ENTRY_LENGHT + 30 )

/*----- Function prototypes --------------------------------------------------*/
static void DEVINFO_CallbackHandler(pstorage_handle_t *handle,
                                    uint8_t opCode,
                                    uint32_t result,
                                    uint8_t *data,
                                    uint32_t dataLength);

static void DEVINFO_BleEventHandler(struct TXW51_SERV_DIS_Handle *handle,
                                    struct TXW51_SERV_DIS_Event *evt);

static void DEVINFO_UpdateValue(enum appl_devinfo_value entry,
                                uint8_t *value,
                                int32_t len);

/*----- Data -----------------------------------------------------------------*/
static pstorage_handle_t storageHandle;     /**< Handle for the flash storage. */
static bool isDataLoaded = false;           /**< Flag to indicate when the data is loaded from flash. */
static bool isOperationPending = false;     /**< Flag to indicate when a flash operation is pending. */

/**
 * @brief Buffer to hold the device information.
 */
static uint8_t deviceInfo[APPL_DEVINFO_NUM_OF_ENTRIES][APPL_DEVINFO_ENTRY_LENGHT];

/**
 * @brief Prefixes to use before printing the device information to the log.
 */
static const char const *names[APPL_DEVINFO_NUM_OF_ENTRIES] = {
    "Manufacturer:    ",
    "Model name:      ",
    "Serial number:   ",
    "Hardware version:",
    "Firmware version:",
    "Device name:     "
};

/*----- Implementation -------------------------------------------------------*/

uint32_t APPL_DEVINFO_Init(void)
{
    uint32_t err;

    /* Clear device information buffer. */
    memset(deviceInfo, '\0', APPL_DEVINFO_INFO_BUFFER_LENGTH);

    /* Initialize flash module. */
    err = pstorage_init();
    if (err != NRF_SUCCESS) {
        TXW51_LOG_ERROR("[DevInfo] Initialization failed.");
        return ERR_DEVINFO_FLASH_INIT_FAILED;
    }

    pstorage_module_param_t storageParams = {
        .cb = DEVINFO_CallbackHandler,
        .block_size = APPL_DEVINFO_INFO_BUFFER_LENGTH,
        .block_count = 1
    };

    err = pstorage_register(&storageParams, &storageHandle);
    if (err != NRF_SUCCESS) {
        TXW51_LOG_ERROR("[DevInfo] Initialization failed.");
        return ERR_DEVINFO_FLASH_INIT_FAILED;
    }

    TXW51_LOG_DEBUG("[DevInfo] Initialization successful.");
    return ERR_NONE;
}


/***************************************************************************//**
 * @brief Handle the callbacks of the flash module.
 *
 * @param[in] handle     Handle of the callback.
 * @param[in] op_code    The code of what triggered the callback.
 * @param[in] result     Shows whether the operation was successful or not.
 * @param[in] data       Data to the opCode when available.
 * @param[in] dataLength The length of the data.
 *
 * @return Nothing.
 ******************************************************************************/
static void DEVINFO_CallbackHandler(pstorage_handle_t *handle,
                                    uint8_t opCode,
                                    uint32_t result,
                                    uint8_t *data,
                                    uint32_t dataLength)
{
    switch(opCode) {
        case PSTORAGE_ERROR_OP_CODE:
            TXW51_LOG_ERROR("[DevInfo] An unknown error occurred.");
            break;

        case PSTORAGE_STORE_OP_CODE:
            break;

        case PSTORAGE_LOAD_OP_CODE:
            if (result == NRF_SUCCESS) {
                isDataLoaded = true;
                TXW51_LOG_DEBUG("[DevInfo] Device information loaded.");
            } else {
                isDataLoaded = false;
                TXW51_LOG_ERROR("[DevInfo] Device information could not be loaded.");
            }
            break;

        case PSTORAGE_CLEAR_OP_CODE:
            break;

        case PSTORAGE_UPDATE_OP_CODE:
            if (result == NRF_SUCCESS) {
                TXW51_LOG_INFO("[DevInfo] Device information updated.");

            } else {
                TXW51_LOG_ERROR("[DevInfo] Device information could not be updated.");
            }
            break;
    }

    isOperationPending = false;
}


uint32_t APPL_DEVINFO_Load(void)
{
    uint32_t err;

    if (isOperationPending) {
        return ERR_DEVINFO_OPERATION_PENDING;
    }

    isOperationPending = true;
    err = pstorage_load(deviceInfo[0], &storageHandle, APPL_DEVINFO_INFO_BUFFER_LENGTH, 0);
    if (err != NRF_SUCCESS) {
        TXW51_LOG_ERROR("[DevInfo] Device information could not be loaded.");
        return ERR_DEVINFO_FLASH_LOAD_FAILED;
    }

    return ERR_NONE;
}


uint32_t APPL_DEVINFO_PrintValues(void)
{
    if (!isDataLoaded) {
        return ERR_DEVINFO_DATA_NOT_LOADED;
    }

    char outputBuffer[APPL_DEVINFO_OUTPUT_BUFFER_LENGTH];

    for (int32_t i = 0; i < APPL_DEVINFO_NUM_OF_ENTRIES; i++) {
        snprintf(outputBuffer, APPL_DEVINFO_OUTPUT_BUFFER_LENGTH, "%s %s", names[i], deviceInfo[i]);
        TXW51_LOG_INFO(outputBuffer);
    }

    return ERR_NONE;
}


uint32_t APPL_DEVINFO_Save(void)
{
    uint32_t err;

    if (isOperationPending) {
        return ERR_DEVINFO_OPERATION_PENDING;
    }

    isOperationPending = true;
    err = pstorage_update(&storageHandle, deviceInfo[0], APPL_DEVINFO_INFO_BUFFER_LENGTH, 0);
    if (err != NRF_SUCCESS) {
        TXW51_LOG_ERROR("[DevInfo] Device information could not be saved.");
        return ERR_DEVINFO_FLASH_SAVE_FAILED;
    }

    return ERR_NONE;
}


uint32_t APPL_DEVINFO_Set(enum appl_devinfo_value entry, char *value)
{
    if (isOperationPending) {
        return ERR_DEVINFO_OPERATION_PENDING;
    }

    strlcpy((char *)deviceInfo[entry], value, APPL_DEVINFO_ENTRY_LENGHT);

    return ERR_NONE;
}


void APPL_DEVINFO_Get(enum appl_devinfo_value entry, char *buffer)
{
    strlcpy(buffer, (char *)deviceInfo[entry], APPL_DEVINFO_ENTRY_LENGHT);
}


uint32_t APPL_DEVINFO_IsBusy(void)
{
    uint32_t err;
    uint32_t count;

    err = pstorage_access_status_get(&count);
    if (err != NRF_SUCCESS) {
        count = -1;
    }

    return count;
}


void APPL_DEVINFO_SetDefaultValues(void)
{
    APPL_DEVINFO_Set(APPL_DEVINFO_VALUE_MANUFACTURER,
                     APPL_DEVINFO_DEFAULT_MANUFACTURER);
    APPL_DEVINFO_Set(APPL_DEVINFO_VALUE_MODEL,
                     APPL_DEVINFO_DEFAULT_MODEL);
    APPL_DEVINFO_Set(APPL_DEVINFO_VALUE_SERIAL,
                     APPL_DEVINFO_DEFAULT_SERIAL);
    APPL_DEVINFO_Set(APPL_DEVINFO_VALUE_HW_REV,
                     APPL_DEVINFO_DEFAULT_HW_REV);
    APPL_DEVINFO_Set(APPL_DEVINFO_VALUE_FW_REV,
                     APPL_DEVINFO_DEFAULT_FW_REV);
    APPL_DEVINFO_Set(APPL_DEVINFO_VALUE_DEVICE_NAME,
                     APPL_DEVINFO_DEFAULT_DEVICE_NAME);
}


/*----------------------------------------------------------------------------*/


uint32_t APPL_DEVINFO_InitService(struct TXW51_SERV_DIS_Handle *serviceHandle)
{
    uint32_t err = ERR_NONE;

    struct TXW51_SERV_DIS_Init disInit;
    disInit.EventHandler        = DEVINFO_BleEventHandler;
    disInit.String_Manufacturer = deviceInfo[APPL_DEVINFO_VALUE_MANUFACTURER];
    disInit.String_Model        = deviceInfo[APPL_DEVINFO_VALUE_MODEL];
    disInit.String_Serial       = deviceInfo[APPL_DEVINFO_VALUE_SERIAL];
    disInit.String_HwRev        = deviceInfo[APPL_DEVINFO_VALUE_HW_REV];
    disInit.String_FwRev        = deviceInfo[APPL_DEVINFO_VALUE_FW_REV];
    disInit.String_DeviceName   = deviceInfo[APPL_DEVINFO_VALUE_DEVICE_NAME];

    err = TXW51_SERV_DIS_Init(serviceHandle, &disInit);
    if (err != ERR_NONE) {
        return ERR_BLE_SERVICE_INIT_FAILED;
    }
    return ERR_NONE;
}


/***************************************************************************//**
 * @brief Handles the BLE events from the device information service.
 *
 * This function gets called from the BLE service as a callback.
 *
 * @param[in] handle The handle of the service.
 * @param[in] evt    The information to the event.
 *
 * @return Nothing.
 ******************************************************************************/
static void DEVINFO_BleEventHandler(struct TXW51_SERV_DIS_Handle *handle,
                                    struct TXW51_SERV_DIS_Event *evt)
{
    switch (evt->EventType) {
        case TXW51_SERV_DIS_EVT_UPDATE_MANUFACTURER:
            TXW51_LOG_INFO("[DIS Service] Update manufacturer");
            DEVINFO_UpdateValue(APPL_DEVINFO_VALUE_MANUFACTURER, evt->Value, evt->Length);
            break;
        case TXW51_SERV_DIS_EVT_UPDATE_MODEL:
            TXW51_LOG_INFO("[DIS Service] Update model");
            DEVINFO_UpdateValue(APPL_DEVINFO_VALUE_MODEL, evt->Value, evt->Length);
            break;
        case TXW51_SERV_DIS_EVT_UPDATE_SERIAL:
            TXW51_LOG_INFO("[DIS Service] Update serial");
            DEVINFO_UpdateValue(APPL_DEVINFO_VALUE_SERIAL, evt->Value, evt->Length);
            break;
        case TXW51_SERV_DIS_EVT_UPDATE_HW_REV:
            TXW51_LOG_INFO("[DIS Service] Update hw rev");
            DEVINFO_UpdateValue(APPL_DEVINFO_VALUE_HW_REV, evt->Value, evt->Length);
            break;
        case TXW51_SERV_DIS_EVT_UPDATE_FW_REV:
            TXW51_LOG_INFO("[DIS Service] Update fw rev");
            DEVINFO_UpdateValue(APPL_DEVINFO_VALUE_FW_REV, evt->Value, evt->Length);
            break;
        case TXW51_SERV_DIS_EVT_UPDATE_DEVICE_NAME:
            TXW51_LOG_INFO("[DIS Service] Update device name");
            DEVINFO_UpdateValue(APPL_DEVINFO_VALUE_DEVICE_NAME, evt->Value, evt->Length);
            break;
        case TXW51_SERV_DIS_EVT_SAVE_VALUES:
            TXW51_LOG_INFO("[DIS Service] Save values to flash");
            APPL_DEVINFO_Save();
            break;
        default:
            break;
    }
}


/***************************************************************************//**
 * @brief Updates a device information entry.
 *
 * @param[in] entry The entry to change.
 * @param[in] value The new value.
 * @param[in] len   The length of the value.
 *
 * @return Nothing.
 ******************************************************************************/
static void DEVINFO_UpdateValue(enum appl_devinfo_value entry,
                                uint8_t *value,
                                int32_t len)
{
    int32_t numberOfChars = MIN(len, APPL_DEVINFO_ENTRY_LENGHT);

    strlcpy((char *)deviceInfo[entry], (char *)value, numberOfChars);
}

