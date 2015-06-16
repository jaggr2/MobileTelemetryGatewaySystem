/***************************************************************************//**
 * @brief   Module that handles the Bluetooth Smart LSM330 service.
 *
 * It initializes and communicates with the Bluetooth Smart service.
 * Additionally, it initializes and sets the LSM330 sensor over SPI.
 *
 * @file    sensor.h
 * @version 1.0
 * @date    05.12.2014
 * @author  Daniel Meer
 *
 * @remark  Last Modifications:
 *          05.12.2014 meerd1 created
 ******************************************************************************/

#ifndef TXW51_APPLICATION_SENSOR_H_
#define TXW51_APPLICATION_SENSOR_H_

/*----- Header-Files ---------------------------------------------------------*/
#include <stdint.h>

#include "txw51_framework/ble/service_lsm330.h"

/*----- Macros ---------------------------------------------------------------*/
#define APPL_SENSOR_VALUES_PER_FIFO_BLOCK     ( 20 )    /**< The level of the sensor FIFO until a watermark interrupt gets generated. */

/*----- Data types -----------------------------------------------------------*/

/*----- Function prototypes --------------------------------------------------*/

/***************************************************************************//**
 * @brief Initializes the LSM330 sensor module..
 *
 * @return Nothing.
 ******************************************************************************/
extern void APPL_SENSOR_Init(void);

/***************************************************************************//**
 * @brief Puts the sensors into measurement mode that have been enabled by the
 * Bluetooth Smart LSM330 service .
 *
 * @return Nothing.
 ******************************************************************************/
extern void APPL_SENSOR_StartToMeasure(void);

/***************************************************************************//**
 * @brief Stops the measurement mode of all sensors.
 *
 * @return Nothing.
 ******************************************************************************/
extern void APPL_SENSOR_StopToMeasure(void);

/***************************************************************************//**
 * @brief Reconfigures the LSM330 sensor to generate an interrupt when movement
 *        has been detected.
 *
 * @return Nothing.
 ******************************************************************************/
extern void APPL_SENSOR_SetupMotionWakeup(void);

/***************************************************************************//**
 * @brief Handles the GPIOTE interrupts of the LSM330 sensor.
 *
 * @param[in] channel GPIOTE channel that generated the interrupt.
 *
 * @return Nothing.
 ******************************************************************************/
extern void APPL_SENSOR_HandleInterrupt(int32_t channel);

/***************************************************************************//**
 * @brief Initializes the Bluetooth Smart LSM330 service.
 *
 * @param[out] serviceHandle The handle for the service.
 *
 * @return Nothing.
 ******************************************************************************/
extern uint32_t APPL_SENSOR_InitService(struct TXW51_SERV_LSM330_Handle *serviceHandle);

/*----- Data -----------------------------------------------------------------*/

#endif /* TXW51_APPLICATION_SENSOR_H_ */
