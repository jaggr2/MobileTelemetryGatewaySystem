/***************************************************************************//**
 * @brief   Module to initialize and use the TMP06 via I2C interface on the TXW51.
 *
 * @file    tmp.c
 * @version 1.0
 * @date    22.04.2015
 * @author  Pascal Bohni
 *
 * @remark  Last Modifications:
 *          22.04.15 bohnp1 created
 ******************************************************************************/

/*----- Header-Files ---------------------------------------------------------*/
#include "tmp006.h"

#include "nrf/nrf.h"
#include "nrf/sd_common/app_util_platform.h"

#include "txw51_framework/hw/gpio.h"
#include "txw51_framework/utils/log.h"
#include "txw51_framework/utils/txw51_errors.h"



uint32_t TXW51_TMP006_GetTemperature(uint8_t *value)
{

    TXW51_LOG_DEBUG("TXW51_TMP006_GetTemperature: Start");

    //uint32_t err;
    uint8_t address = TMP006_I2CADDR;
    uint8_t len = 1;
    uint8_t reg = 0x00; //Voltage Value

    *value = 0;

    twi_master_init();

	// Write: register address we want to start reading from
	if (twi_master_transfer((address << 1), &reg, 1, TWI_DONT_ISSUE_STOP))
	{

		// Read: the number of bytes requested.
		if (twi_master_transfer((address << 1) | TWI_READ_BIT, value, len, TWI_DONT_ISSUE_STOP))
		{
		  // Read succeeded.

				TXW51_LOG_DEBUG("TXW51_TMP006_GetTemperature: Success");
				return ERR_NONE;
		}
	}

	// read or write failed.

	TXW51_LOG_WARNING("[TMP006] Could not read temperature.");
	return false;

    //return ERR_NONE;
}
