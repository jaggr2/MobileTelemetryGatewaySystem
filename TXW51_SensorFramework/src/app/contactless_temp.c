/***************************************************************************//**
 * @brief   Module that handles the Bluetooth Smart contactless temperature service.
 *
 * It initializes and communicates with the Bluetooth Smart service.
 * Additionally, it initializes and sets the tmp006 sensor over i2c.
 *
 * @file    contactless_temp.c
 * @version 1.0
 * @date    10.04.2015
 * @author  Pascal Bohni
 *
 * @remark  Last Modifications:
 *          10.04.2015 bohnp1 created
 ******************************************************************************/

/*----- Header-Files ---------------------------------------------------------*/
#include "contactless_temp.h"

#include <string.h>
#include <stdio.h>

//#include "txw51_framework/hw/lsm330.h"
#include "txw51_framework/utils/log.h"

#include "app/appl.h"
#include "app/error.h"
#include "app/fifo.h"

/*----- Macros ---------------------------------------------------------------*/

/*----- Data types -----------------------------------------------------------*/

/*----- Function prototypes --------------------------------------------------*/
static void CONTACTLESS_TEMP_BleEventHandler(struct TXW51_SERV_TEMP_CONTACTLESS_Handle *handle,
                                   struct TXW51_SERV_TEMP_CONTACTLESS_Event *evt);
static void CONTACTLESS_TEMP_GetTemperature(uint8_t *value);

/*----- Data -----------------------------------------------------------------*/

/*----- Implementation -------------------------------------------------------*/

void APPL_CONTACTLESS_TEMP_Init(void)
{
	//TXW51_SERV_TEMP_CONTACTLESS_Init();
}



void APPL_CONTACTLESS_TEMP_SetupMotionWakeup(void)
{
    //TXW51_CONTACTLESS_TEMP_SetMotionWakeup();
}

/* -------------------------------------------------------------------------- */


uint32_t APPL_CONTACTLESS_TEMP_InitService(struct TXW51_SERV_TEMP_CONTACTLESS_Handle *serviceHandle)
{
    uint32_t err = ERR_NONE;

    struct TXW51_SERV_TEMP_CONTACTLESS_Init init;
    init.EventHandler = CONTACTLESS_TEMP_BleEventHandler;

    err = TXW51_SERV_TEMP_CONTACTLESS_Init(serviceHandle, &init);
    if (err != ERR_NONE) {
        return ERR_BLE_SERVICE_INIT_FAILED;
    }
    return ERR_NONE;
}


/***************************************************************************//**
 * @brief Handles the BLE events from the CONTACTLESS_TEMP service.
 *
 * This function gets called from the BLE service as a callback.
 *
 * @param[in] handle The handle of the service.
 * @param[in] evt    The information to the event.
 *
 * @return Nothing.
 ******************************************************************************/
static void CONTACTLESS_TEMP_BleEventHandler(struct TXW51_SERV_TEMP_CONTACTLESS_Handle *handle,
                                   struct TXW51_SERV_TEMP_CONTACTLESS_Event *evt)
{
    switch (evt->EventType) {
        case TXW51_SERV_TEMP_CONTACTLESS_EVT_TEMP_SAMPLE:
            CONTACTLESS_TEMP_GetTemperature(evt->Value);
            break;
        default:
            break;
    }
}



/***************************************************************************//**
 * @brief Reads the temperature sensor.
 *
 * @param[out] value The temperature value.
 *
 * @return Nothing.
 ******************************************************************************/
static void CONTACTLESS_TEMP_GetTemperature(uint8_t *value)
{
    //TXW51_TEMP_CONTACTLESS_GetTemperature(value);

    /* Convert sensor value to real temperature. */
    //*value = TXW51_CONTACTLESS_TEMP_TEMP_REF - *value;

    TXW51_LOG_DEBUG("[CONTACTLESS_TEMP Sensor] Temperature read.");
}

