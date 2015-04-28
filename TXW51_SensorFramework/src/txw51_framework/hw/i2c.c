/***************************************************************************//**
 * @brief   Module to initialize and use an I2C interface on the TXW51.
 *
 * @file    i2c.h
 * @version 1.0
 * @date    24.04.2015
 * @author  Pascal Bohni
 *
 * @remark  Last Modifications:
 *          24.04.2015 created
 ******************************************************************************/

/*----- Header-Files ---------------------------------------------------------*/
#include "i2c.h"

#include "nrf/nrf.h"
#include "nrf/sd_common/app_util_platform.h"

#include "txw51_framework/hw/gpio.h"
#include "txw51_framework/utils/log.h"
#include "txw51_framework/utils/txw51_errors.h"

/*----- Macros ---------------------------------------------------------------*/

/*----- Data types -----------------------------------------------------------*/

/*----- Function prototypes --------------------------------------------------*/
static void I2C_EventHandler();

/*----- Data -----------------------------------------------------------------*/


/*----- Implementation -------------------------------------------------------*/

/***************************************************************************//**
 * @brief Function that handles the events from the SPI0 interface.
 *
 * @param[in] event The event that was generated.
 *
 * @return Nothing.
 ******************************************************************************/
static void I2C_EventHandler()
{

}

uint32_t TXW51_I2C_Init()
{
    uint32_t err = NRF_SUCCESS;

    /* Configure I2C master. */
    twi_master_init();
    return ERR_NONE;
}


void TXW51_I2C_Deinit()
{

}


uint32_t TXW51_I2C_Read(uint8_t addr,
						uint8_t reg,
                        uint8_t *values,
                        uint32_t len)
{

	// initialize values to zero so we don't return random values.
	for (int i = 0; i < len; i++)
	{
		values[i] = 0;
	}

	// Write: register address we want to start reading from
	if (twi_master_transfer(addr, &reg, 1, TWI_DONT_ISSUE_STOP))
	{
		// Read: the number of bytes requested.
		if (twi_master_transfer(addr | TWI_READ_BIT, values, len, TWI_ISSUE_STOP))
		{
		  // Read succeeded.
				return ERR_NONE;
		}
	}

	// read or write failed.
	return false;
}


uint32_t TXW51_I2C_Write(uint8_t addr,
                         uint8_t value)
{




    return ERR_NONE;
}

