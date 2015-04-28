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

#ifndef TXW51_FRAMEWORK_HW_I2C_H_
#define TXW51_FRAMEWORK_HW_I2C_H_

/*----- Header-Files ---------------------------------------------------------*/
#include "nrf/twi_master.h"

/*----- Macros ---------------------------------------------------------------*/

/*----- Data types -----------------------------------------------------------*/

/*----- Function prototypes --------------------------------------------------*/

/***************************************************************************//**
 * @brief Initializes the I2C interface.
 *
 *
 * @return ERR_NONE if no error occurred.
 *         ERR_SPI_INIT_FAILED if initialization failed.
 ******************************************************************************/
extern uint32_t TXW51_I2C_Init();

/***************************************************************************//**
 * @brief Deinitializes the I2C interface.
 *
 * This can be used to save energy on the GPIO pins.
 *
 * @return Nothing.
 ******************************************************************************/
extern void TXW51_I2C_Deinit();

/***************************************************************************//**
 * @brief Reads from the I2C interface.
 *
 * If multiple registers are read, addr is the starting address and we
 * increment afterwards.
 * @param[in]  addr        Address of the register.
 * @param[out] values      Buffer to save the values.
 * @param[in]  n           How many bytes to read.
 *
 * @return ERR_NONE if no error occurred.
 *         ERR_SPI_READ_FAILED if the values could not be read.
 ******************************************************************************/
extern uint32_t TXW51_I2C_Read(uint8_t addr,
							   uint8_t reg,
							   uint8_t *values,
							   uint32_t len);

/***************************************************************************//**
 * @brief Writes to the I2C interface.
 *
 * @param[in] addr        Address of the register.
 * @param[in] value       Value to write.
 *
 * @return ERR_NONE if no error occurred.
 *         ERR_SPI_WRITE_FAILED if the values could not be written.
 ******************************************************************************/
extern uint32_t TXW51_I2C_Write(uint8_t addr,
                                uint8_t value);

/*----- Data -----------------------------------------------------------------*/

#endif /* TXW51_FRAMEWORK_HW_I2C_H_ */
