/***************************************************************************//**
 * @brief   Module that handles the Bluetooth Smart i2c ble Bridge service.
 *
 * It initializes and communicates with the Bluetooth Smart service.
 * Additionally, it initializes and communicates over i2c.
 *
 * @file    i2cBridge.h
 * @version 1.0
 * @date    08.05.2015
 * @author  Pascal Bohni
 *
 * @remark  Last Modifications:
 *          08.05.2015 bohnp1 created
 ******************************************************************************/

#ifndef TXW51_APPLICATION_I2C_BRIDGE_H_
#define TXW51_APPLICATION_I2C_BRIDGE_H_

/*----- Header-Files ---------------------------------------------------------*/
#include <stdint.h>

#include "txw51_framework/ble/service_i2c.h"

/*----- Macros ---------------------------------------------------------------*/

/*----- Data types -----------------------------------------------------------*/

/*----- Function prototypes --------------------------------------------------*/

/***************************************************************************//**
 * @brief Initializes the I2C Bus .
 *
 * @return Nothing.
 ******************************************************************************/
extern void APPL_I2C_BRIDGE_Init(void);

/***************************************************************************//**
 * @brief Initializes the Bluetooth Smart to I2C service.
 *
 * @param[out] serviceHandle The handle for the service.
 *
 * @return Nothing.
 ******************************************************************************/
extern uint32_t APPL_I2C_BRIDGE_InitService(struct TXW51_SERV_I2C_Handle *serviceHandle);


/*----- Data -----------------------------------------------------------------*/
uint8_t i2cAddress;
uint8_t i2cRegister;
uint8_t i2cLength;

#endif /* TXW51_APPLICATION_I2C_BRIDGE_H_ */
