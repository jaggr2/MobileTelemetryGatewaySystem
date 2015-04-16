/***************************************************************************//**
 * @brief   This module implements a FIFO to be used as a buffer.
 *
 * @file    fifo.c
 * @version 1.0
 * @date    05.12.2014
 * @author  Daniel Meer
 *
 * @remark  Last Modifications:
 *          05.12.2014 meerd1 created
 ******************************************************************************/

/*----- Header-Files ---------------------------------------------------------*/
#include "fifo.h"

#include "nrf/app_common/app_fifo.h"

#include "txw51_framework/utils/log.h"

#include "app/error.h"

/*----- Macros ---------------------------------------------------------------*/

/*----- Data types -----------------------------------------------------------*/

/*----- Function prototypes --------------------------------------------------*/

/*----- Data -----------------------------------------------------------------*/
static app_fifo_t fifoHandleAcc;    /**< Handle of the FIFO for the acceleration values. */
static app_fifo_t fifoHandleGyro;   /**< Handle of the FIFO for the gyroscope values. */
static uint8_t bufferAcc[APPL_FIFO_BUFFER_SIZE_ACC];    /**< Memory of the acceleration FIFO. */
static uint8_t bufferGyro[APPL_FIFO_BUFFER_SIZE_GYRO];  /**< Memory of the gyroscope FIFO. */

/*----- Implementation -------------------------------------------------------*/

uint32_t APPL_FIFO_Init(void)
{
    uint32_t err;

    err = app_fifo_init(&fifoHandleAcc, bufferAcc, APPL_FIFO_BUFFER_SIZE_ACC);
    if (err != NRF_SUCCESS) {
        TXW51_LOG_ERROR("[FIFO] Initialization failed.");
        return ERR_FIFO_INIT_FAILED;
    }

    err = app_fifo_init(&fifoHandleGyro, bufferGyro, APPL_FIFO_BUFFER_SIZE_GYRO);
    if (err != NRF_SUCCESS) {
        TXW51_LOG_ERROR("[FIFO] Initialization failed.");
        return ERR_FIFO_INIT_FAILED;
    }

    TXW51_LOG_DEBUG("[FIFO] Initialization successful.");
    return ERR_NONE;
}


uint32_t APPL_FIFO_Put(enum appl_fifo_type bufferType,
                       uint8_t *buffer,
                       uint32_t numberOfBytes)
{
    uint32_t err;

    app_fifo_t *fifoHandle;
    if (bufferType == APPL_FIFO_BUFFER_ACC) {
        fifoHandle = &fifoHandleAcc;
    } else {
        fifoHandle = &fifoHandleGyro;
    }

    for (int32_t i = 0; i < numberOfBytes; i++) {
        err = app_fifo_put(fifoHandle, buffer[i]);
        if (err != NRF_SUCCESS) {
            TXW51_LOG_ERROR("[FIFO] Could not put all values into buffer.");
            return ERR_FIFO_PUT_FAILED;
        }
    }

    return ERR_NONE;
}


uint32_t APPL_FIFO_Get(enum appl_fifo_type bufferType,
                       uint8_t *buffer,
                       uint32_t numberOfBytes)
{
    uint32_t err;
    uint32_t bytesRead = 0;

    app_fifo_t *fifoHandle;
    if (bufferType == APPL_FIFO_BUFFER_ACC) {
        fifoHandle = &fifoHandleAcc;
    } else {
        fifoHandle = &fifoHandleGyro;
    }

    for (int32_t i = 0; i < numberOfBytes; i++) {
        err = app_fifo_get(fifoHandle, &buffer[i]);
        if (err != NRF_SUCCESS) {
            break;
        }
        bytesRead++;
    }

    return bytesRead;
}

