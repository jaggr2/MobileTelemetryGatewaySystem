/***************************************************************************//**
 * @brief   Module to initialize and use the TMP006 via I2C interface on the TXW51.
 *
 * @file    tmp006.h
 * @version 1.0
 * @date    22.04.2015
 * @author  Pascal Bohni
 *
 * @remark  Last Modifications:
 *          22.04.2015 bohnp1 created
 ******************************************************************************/

#ifndef TMP006_H_
#define TMP006_H_

#define TMP006_I2CADDR 0x40
#define TMP006_MANID 0xFE
#define TMP006_DEVID 0xFF

#include "twi_master.h"
#include <stdbool.h>
#include <stdint.h>

/***************************************************************************//**
 * @brief Reads the temperature value.
 * *
 * @param[out] value Buffer to save the temperature.
 *
 * @return ERR_NONE if no error occurred.
 *         ERR_TMP006_READ_FAILED if reading from the sensor failed.
 ******************************************************************************/
extern uint32_t TXW51_TMP006_GetTemperature(uint8_t *value);


#endif /* TMP006_H_ */

