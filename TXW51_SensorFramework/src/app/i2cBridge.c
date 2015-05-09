/***************************************************************************//**
 * @brief   Module that handles the Bluetooth Smart I2C Ble Bridge service.
 *
 * It initializes and communicates with the Bluetooth Smart service.
 * Additionally, it initializes and communicates on i2c.
 *
 * @file    i2cBridge.c
 * @version 1.0
 * @date    10.04.2015
 * @author  Pascal Bohni
 *
 * @remark  Last Modifications:
 *          08.05.2015 bohnp1 created
 ******************************************************************************/

/*----- Header-Files ---------------------------------------------------------*/
#include "i2cBridge.h"

#include <string.h>
#include <stdio.h>

#include "txw51_framework/hw/i2c.h"
#include "txw51_framework/utils/log.h"

#include "app/appl.h"
#include "app/error.h"
#include "app/fifo.h"

/*----- Macros ---------------------------------------------------------------*/

/*----- Data types -----------------------------------------------------------*/

/*----- Function prototypes --------------------------------------------------*/
static void I2C_BRIDGE_BleEventHandler(struct TXW51_SERV_I2C_BRIDGE_Handle *handle,
                                   struct TXW51_SERV_I2C_BRIDGE_Event *evt);

/*----- Data -----------------------------------------------------------------*/

/*----- Implementation -------------------------------------------------------*/

void APPL_I2C_BRIDGE_Init(void)
{
	TXW51_I2C_Init();
}

/* -------------------------------------------------------------------------- */


uint32_t APPL_I2C_InitService(struct TXW51_SERV_I2C_Handle *serviceHandle)
{
    uint32_t err = ERR_NONE;

    struct TXW51_SERV_I2C_Init init;
    init.EventHandler = APPL_I2C_BleEventHandler;

    err = TXW51_SERV_I2C_Init(serviceHandle, &init);
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
static void APPL_I2C_BleEventHandler(struct TXW51_SERV_I2C_Handle *handle,
                                   struct TXW51_SERV_I2C_Event *evt)
{
	char debugOut [15];
    switch (evt->EventType) {
        case TXW51_SERV_I2C_EVT_ADRESS:
           	i2cAddress = *evt->Value;
           	sprintf(debugOut, "%d", i2cAddress);
           	TXW51_LOG_DEBUG("I2C Address set to:");
           	TXW51_LOG_DEBUG(debugOut);
            break;
        case TXW51_SERV_I2C_EVT_REGISTER:
           	i2cRegister = *evt->Value;
           	sprintf(debugOut, "%d", i2cRegister);
           	TXW51_LOG_DEBUG("I2C Register set to:");
           	TXW51_LOG_DEBUG(debugOut);
            break;
        case TXW51_SERV_I2C_EVT_VALUE_READ:
        	TXW51_I2C_Read(i2cAddress, i2cRegister, evt->Value, 1);
           	TXW51_LOG_DEBUG("");
        	break;
        case TXW51_SERV_I2C_EVT_VALUE_WRITE:
        	TXW51_I2C_Write(i2cAddress, i2cRegister, evt->Value, 1);
           	TXW51_LOG_DEBUG("");
        	break;
        default:
            break;
    }
}
