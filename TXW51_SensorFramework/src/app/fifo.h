/***************************************************************************//**
 * @brief   This module implements a FIFO to be used as a buffer.
 *
 * @file    fifo.h
 * @version 1.0
 * @date    05.12.2014
 * @author  Daniel Meer
 *
 * @remark  Last Modifications:
 *          05.12.2014 meerd1 created
 ******************************************************************************/

#ifndef TXW51_APPLICATION_FIFO_H_
#define TXW51_APPLICATION_FIFO_H_

/*----- Header-Files ---------------------------------------------------------*/
#include <stdint.h>

/*----- Macros ---------------------------------------------------------------*/
#define APPL_FIFO_BUFFER_SIZE_ACC       ( 1024 )    /**< Size of the FIFO for the accelerometer (has to be a power of 2). */
#define APPL_FIFO_BUFFER_SIZE_GYRO      ( 1024 )    /**< Size of the FIFO for the gyroscope (has to be a power of 2). */

/*----- Data types -----------------------------------------------------------*/
/**
 * List of the different FIFO buffers.
 */
enum appl_fifo_type {
    APPL_FIFO_BUFFER_ACC,   /**< FIFO for the accelerometer values. */
    APPL_FIFO_BUFFER_GYRO   /**< FIFO for the gyroscope values. */
};

/*----- Function prototypes --------------------------------------------------*/

/***************************************************************************//**
 * @brief Initializes the FIFO module.
 *
 * @return ERR_NONE if no error occurred.
 *         ERR_FIFO_INIT_FAILED if the initialization of the FIFO failed.
 ******************************************************************************/
extern uint32_t APPL_FIFO_Init(void);

/***************************************************************************//**
 * @brief Puts multiple bytes into the FIFO buffer.
 *
 * @param[in] bufferType    Which FIFO buffer to use.
 * @param[in] buffer        Buffer with the values to add.
 * @param[in] numberOfBytes Number of bytes to put into the FIFO.
 *
 * @return ERR_NONE if no error occurred.
 *         ERR_FIFO_PUT_FAILED if not all values could be put into the FIFO.
 ******************************************************************************/
extern uint32_t APPL_FIFO_Put(enum appl_fifo_type bufferType,
                              uint8_t *buffer,
                              uint32_t numberOfBytes);

/***************************************************************************//**
 * @brief Gets multiple bytes from the FIFO buffer.
 *
 * @param[in] bufferType    Which FIFO buffer to use.
 * @param[in] buffer        Buffer to save the values.
 * @param[in] numberOfBytes Number of bytes to get from the FIFO.
 *
 * @return Number of bytes read from the FIFO.
 ******************************************************************************/
extern uint32_t APPL_FIFO_Get(enum appl_fifo_type bufferType,
                              uint8_t *buffer,
                              uint32_t numberOfBytes);

/*----- Data -----------------------------------------------------------------*/

#endif /* TXW51_APPLICATION_FIFO_H_ */
