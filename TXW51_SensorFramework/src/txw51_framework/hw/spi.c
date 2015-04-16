/***************************************************************************//**
 * @brief   Module to initialize and use the SPI interface on the TXW51.
 *
 * @file    spi.c
 * @version 1.0
 * @date    26.11.2014
 * @author  Daniel Meer
 *
 * @remark  Last Modifications:
 *          26.11.2014 meerd1 created
 ******************************************************************************/

/*----- Header-Files ---------------------------------------------------------*/
#include "spi.h"

#include "nrf/nrf.h"
#include "nrf/sd_common/app_util_platform.h"

#include "txw51_framework/hw/gpio.h"
#include "txw51_framework/utils/log.h"
#include "txw51_framework/utils/txw51_errors.h"

/*----- Macros ---------------------------------------------------------------*/

/*----- Data types -----------------------------------------------------------*/

/*----- Function prototypes --------------------------------------------------*/
static void SPI0_EventHandler(spi_master_evt_t event);
static void SPI1_EventHandler(spi_master_evt_t event);

/*----- Data -----------------------------------------------------------------*/
// TODO: Find better way than two static variables.
static bool hasReceivedSpi0 = false;    /**< Flag to signal when a message has been received from SPI0. */
static bool hasReceivedSpi1 = false;    /**< Flag to signal when a message has been received from SPI1. */

/*----- Implementation -------------------------------------------------------*/

/***************************************************************************//**
 * @brief Function that handles the events from the SPI0 interface.
 *
 * @param[in] event The event that was generated.
 *
 * @return Nothing.
 ******************************************************************************/
static void SPI0_EventHandler(spi_master_evt_t event)
{
    if (event.evt_type == SPI_MASTER_EVT_TRANSFER_COMPLETED) {
        hasReceivedSpi0 = true;
    }
}


/***************************************************************************//**
 * @brief Function that handles the events from the SPI1 interface.
 *
 * @param[in] event The event that was generated.
 *
 * @return Nothing.
 ******************************************************************************/
static void SPI1_EventHandler(spi_master_evt_t event)
{
    if (event.evt_type == SPI_MASTER_EVT_TRANSFER_COMPLETED) {
        hasReceivedSpi1 = true;
    }
}


uint32_t TXW51_SPI_Init(enum TXW51_SPI_Instance spiInstance)
{
    uint32_t err = NRF_SUCCESS;

    /* Configure SPI master. */
    spi_master_config_t spi_config = SPI_MASTER_INIT_DEFAULT;
    spi_config.SPI_Pin_SCK         = TXW51_GPIO_PIN_SPI0_SCLK;
    spi_config.SPI_Pin_MISO        = TXW51_GPIO_PIN_SPI0_MISO;
    spi_config.SPI_Pin_MOSI        = TXW51_GPIO_PIN_SPI0_MOSI;
    spi_config.SPI_CONFIG_ORDER    = SPI_CONFIG_ORDER_MsbFirst;
    spi_config.SPI_CONFIG_CPOL     = SPI_CONFIG_CPOL_ActiveLow;
    spi_config.SPI_CONFIG_CPHA     = SPI_CONFIG_CPHA_Trailing;

    err = spi_master_open(spiInstance, &spi_config);
    if (err != NRF_SUCCESS) {
        TXW51_LOG_ERROR("[SPI] Initialization failed.");
        return ERR_SPI_INIT_FAILED;
    }

    /* Register event handler for SPI master. */
    if (spiInstance == TXW51_SPI_0) {
        spi_master_evt_handler_reg(spiInstance, SPI0_EventHandler);
    } else {
        spi_master_evt_handler_reg(spiInstance, SPI1_EventHandler);
    }

    return ERR_NONE;
}


void TXW51_SPI_Deinit(enum TXW51_SPI_Instance spiInstance)
{
    spi_master_close(spiInstance);

    if (spiInstance == TXW51_SPI_0) {
        TXW51_GPIO_ConfigGpioAsDisconnected(TXW51_GPIO_PIN_SPI0_SCLK);
        TXW51_GPIO_ConfigGpioAsDisconnected(TXW51_GPIO_PIN_SPI0_MISO);
        TXW51_GPIO_ConfigGpioAsDisconnected(TXW51_GPIO_PIN_SPI0_MOSI);
        TXW51_GPIO_ConfigGpioAsDisconnected(TXW51_GPIO_PIN_SPI0_SS_A);
        TXW51_GPIO_ConfigGpioAsDisconnected(TXW51_GPIO_PIN_SPI0_SS_G);
    }
}


uint32_t TXW51_SPI_Read(enum TXW51_SPI_Instance spiInstance,
                        uint8_t addr,
                        uint8_t *values,
                        uint32_t n)
{
    uint32_t err;
    uint8_t txBuffer[1];
    uint8_t rxBuffer[n+1];

    bool *hasReceived = (spiInstance == TXW51_SPI_0) ?
            &hasReceivedSpi0 : &hasReceivedSpi1;

    txBuffer[0] = addr | TXW51_SPI_FLAG_TX;
    err = spi_master_send_recv(spiInstance, txBuffer, 1, rxBuffer, n+1);
    if (err != NRF_SUCCESS) {
        TXW51_LOG_ERROR("[SPI] Could not read from SPI.");
        return ERR_SPI_READ_FAILED;
    }

    for (int32_t i = 0; !(*hasReceived); i++) {
        if (i >= TXW51_SPI_WAIT_TIMEOUT) {
            return ERR_SPI_READ_FAILED;
        }
    }
    *hasReceived = false;

    for (int32_t i = 0; i < n; i++) {
        values[i] = rxBuffer[i+1];
    }

    return ERR_NONE;
}


uint32_t TXW51_SPI_Write(enum TXW51_SPI_Instance spiInstance,
                         uint8_t addr,
                         uint8_t value)
{
    uint32_t err;
    uint8_t tx_buffer[2];
    uint8_t rx_buffer[2] = {0, 0};

    bool *hasReceived = (spiInstance == TXW51_SPI_0) ?
            &hasReceivedSpi0 : &hasReceivedSpi1;

    tx_buffer[0] = addr | TXW51_SPI_FLAG_RX;
    tx_buffer[1] = value;
    err = spi_master_send_recv(spiInstance, tx_buffer, 2, rx_buffer, 0);
    if (err != NRF_SUCCESS) {
        TXW51_LOG_ERROR("[SPI] Could not write to SPI.");
        return ERR_SPI_WRITE_FAILED;
    }

    for (int32_t i = 0; !(*hasReceived); i++) {
        if (i >= TXW51_SPI_WAIT_TIMEOUT) {
            return ERR_SPI_WRITE_FAILED;
        }
    }
    *hasReceived = false;

    return ERR_NONE;
}

