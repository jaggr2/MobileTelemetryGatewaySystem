/***************************************************************************//**
 * @brief   Module to initialize and use the SPI interface on the TXW51.
 *
 * @file    spi.h
 * @version 1.0
 * @date    26.11.2014
 * @author  Daniel Meer
 *
 * @remark  Last Modifications:
 *          26.11.2014 meerd1 created
 ******************************************************************************/

#ifndef TXW51_FRAMEWORK_HW_SPI_H_
#define TXW51_FRAMEWORK_HW_SPI_H_

/*----- Header-Files ---------------------------------------------------------*/
#include "nrf/spi_master.h"

/*----- Macros ---------------------------------------------------------------*/
#define TXW51_SPI_WAIT_TIMEOUT      ( 100000 )      /**< Timeout for the module to wait during SPI communication (for-loop). */
#define TXW51_SPI_FLAG_TX           ( 0x01 << 7 )   /**< Write flag for the SPI communication. */
#define TXW51_SPI_FLAG_RX           ( 0x00 << 7 )   /**< Read flag for the SPI communication. */

/*----- Data types -----------------------------------------------------------*/
/**
 * List of the available SPI interfaces.
 */
enum TXW51_SPI_Instance {
    TXW51_SPI_0 = SPI_MASTER_0,     /**< Use SPI0 interface. */
    TXW51_SPI_1 = SPI_MASTER_1      /**< Use SPI1 interface. */
};

/*----- Function prototypes --------------------------------------------------*/

/***************************************************************************//**
 * @brief Initializes the SPI interface.
 *
 * At the moment only SPI0 works.
 *
 * @param[in] spiInstance Which SPI to use.
 *
 * @return ERR_NONE if no error occurred.
 *         ERR_SPI_INIT_FAILED if initialization failed.
 ******************************************************************************/
extern uint32_t TXW51_SPI_Init(enum TXW51_SPI_Instance spiInstance);

/***************************************************************************//**
 * @brief Deinitializes the SPI interface.
 *
 * This can be used to save energy on the GPIO pins.
 *
 * @param[in] spiInstance Which SPI to use.
 *
 * @return Nothing.
 ******************************************************************************/
extern void TXW51_SPI_Deinit(enum TXW51_SPI_Instance spiInstance);

/***************************************************************************//**
 * @brief Reads from the SPI interface.
 *
 * If multiple registers are read, addr is the starting address and we
 * increment afterwards.
 *
 * @param[in]  spiInstance Which SPI to use.
 * @param[in]  addr        Address of the register.
 * @param[out] values      Buffer to save the values.
 * @param[in]  n           How many bytes to read.
 *
 * @return ERR_NONE if no error occurred.
 *         ERR_SPI_READ_FAILED if the values could not be read.
 ******************************************************************************/
extern uint32_t TXW51_SPI_Read(enum TXW51_SPI_Instance spiInstance,
                               uint8_t addr,
                               uint8_t *values,
                               uint32_t n);

/***************************************************************************//**
 * @brief Writes to the SPI interface.
 *
 * @param[in] spiInstance Which SPI to use.
 * @param[in] addr        Address of the register.
 * @param[in] value       Value to write.
 *
 * @return ERR_NONE if no error occurred.
 *         ERR_SPI_WRITE_FAILED if the values could not be written.
 ******************************************************************************/
extern uint32_t TXW51_SPI_Write(enum TXW51_SPI_Instance spiInstance,
                                uint8_t addr,
                                uint8_t value);

/*----- Data -----------------------------------------------------------------*/

#endif /* TXW51_FRAMEWORK_HW_SPI_H_ */
