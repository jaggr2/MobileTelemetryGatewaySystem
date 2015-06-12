/***************************************************************************//**
 * @brief   This module can be used to test the SPI according to the Test
 *          Specification entry T124.
 *
 * You have to include this file to the build and exclude the file with the
 * real main function.
 *
 * It prints the content of the WHO_AM_I register of the LSM330 accelerometer
 * to the log.
 *
 * @file    test_spi.c
 * @version 1.0
 * @date    15.01.2015
 * @author  Daniel Meer
 *
 * @remark  Last Modifications:
 *          15.01.2015 meerd1 created
 ******************************************************************************/

/*----- Header-Files ---------------------------------------------------------*/
#include <stdbool.h>

#include "nrf/sd_common/softdevice_handler.h"

#include "txw51_framework/hw/lsm330.h"
#include "txw51_framework/hw/spi.h"
#include "txw51_framework/utils/log.h"

/*----- Macros ---------------------------------------------------------------*/

/*----- Data types -----------------------------------------------------------*/

/*----- Function prototypes --------------------------------------------------*/

/*----- Data -----------------------------------------------------------------*/

/*----- Implementation -------------------------------------------------------*/

int main(void)
{
    TXW51_LOG_Init();
    SOFTDEVICE_HANDLER_INIT(CONFIG_CLOCK_LFCLK_SOURCE, false);
    TXW51_SPI_Init(SPI_MASTER_0);
    TXW51_LSM330_Init();

    TXW51_UART_WriteString((uint8_t *)"\r\n\r\n");
    TXW51_UART_WriteString((uint8_t *)"SPI-Test\r\n");

    TXW51_UART_WriteString((uint8_t *)"WHO_AM_I ");
    TXW51_LSM330_ACC_Debug_ShowRegister(TXW51_LSM330_REG_WHO_AM_I_A);

    while (true) {
        /* Do nothing. */
    }

    return 0;
}

