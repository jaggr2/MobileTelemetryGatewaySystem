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

#ifndef TXW51_APPLICATION_CONTACTLESS_TEMP_H_
#define TXW51_APPLICATION_CONTACTLESS_TEMP_H_

/*----- Header-Files ---------------------------------------------------------*/
#include <stdint.h>

#include "txw51_framework/ble/service_tempContactless.h"

/*----- Macros ---------------------------------------------------------------*/
#define APPL_CONTACTLESS_TEMP_VALUES_PER_FIFO_BLOCK     ( 20 )    /**< The level of the sensor FIFO until a watermark interrupt gets generated. */

/*----- Data types -----------------------------------------------------------*/

/*----- Function prototypes --------------------------------------------------*/

/***************************************************************************//**
 * @brief Initializes the TMP006 sensor module..
 *
 * @return Nothing.
 ******************************************************************************/
extern void APPL_CONTACTLESS_TEMP_Init(void);

/***************************************************************************//**
 * @brief Puts the sensors into measurement mode that have been enabled by the
 * Bluetooth Smart contactless Temp service .
 *
 * @return Nothing.
 ******************************************************************************/
extern void APPL_CONTACTLESS_TEMP_StartToMeasure(void);

/***************************************************************************//**
 * @brief Stops the measurement mode of all sensors.
 *
 * @return Nothing.
 ******************************************************************************/
extern void APPL_CONTACTLESS_TEMP_StopToMeasure(void);

/***************************************************************************//**
 * @brief Reconfigures the TMP006 sensor to generate an interrupt when movement
 *        has been detected.
 *
 * @return Nothing.
 ******************************************************************************/
extern void APPL_CONTACTLESS_TEMP_SetupMotionWakeup(void);

/***************************************************************************//**
 * @brief Handles the GPIOTE interrupts of the TMP006 sensor.
 *
 * @param[in] channel GPIOTE channel that generated the interrupt.
 *
 * @return Nothing.
 ******************************************************************************/
extern void APPL_CONTACTLESS_TEMP_HandleInterrupt(int32_t channel);

/***************************************************************************//**
 * @brief Initializes the Bluetooth Smart LSM330 service.
 *
 * @param[out] serviceHandle The handle for the service.
 *
 * @return Nothing.
 ******************************************************************************/
extern uint32_t APPL_CONTACTLESS_TEMP_InitService(struct TXW51_SERV_TEMP_CONTACTLESS_Handle *serviceHandle);

/*----- Data -----------------------------------------------------------------*/

#endif /* TXW51_APPLICATION_CONTACTLESS_TEMP_H_ */
