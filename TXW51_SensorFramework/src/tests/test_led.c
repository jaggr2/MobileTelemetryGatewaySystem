/***************************************************************************//**
 * @brief   This module can be used to test the LEDs according to the Test
 *          Specification entry T122.
 *
 * You have to include this file to the build and exclude the file with the
 * real main function.
 *
 * It turns each LED one after another on and off again.
 *
 * @file    test_led.c
 * @version 1.0
 * @date    15.01.2015
 * @author  Daniel Meer
 *
 * @remark  Last Modifications:
 *          15.01.2015 meerd1 created
 ******************************************************************************/

/*----- Header-Files ---------------------------------------------------------*/
#include <stdbool.h>

#include "txw51_framework/hw/gpio.h"
#include "txw51_framework/utils/nrf_delay.h"

/*----- Macros ---------------------------------------------------------------*/

/*----- Data types -----------------------------------------------------------*/

/*----- Function prototypes --------------------------------------------------*/

/*----- Data -----------------------------------------------------------------*/

/*----- Implementation -------------------------------------------------------*/

int main(void)
{
    TXW51_GPIO_InitLed();

    int32_t delay = 100000;
    while (true) {
        nrf_delay_us(5*delay);

        TXW51_GPIO_SetGpio(TXW51_GPIO_PIN_LED0);
        nrf_delay_us(delay);
        TXW51_GPIO_ClearGpio(TXW51_GPIO_PIN_LED0);
        nrf_delay_us(delay);

        TXW51_GPIO_SetGpio(TXW51_GPIO_PIN_LED1);
        nrf_delay_us(delay);
        TXW51_GPIO_ClearGpio(TXW51_GPIO_PIN_LED1);
        nrf_delay_us(delay);

        TXW51_GPIO_SetGpio(TXW51_GPIO_PIN_LED2);
        nrf_delay_us(delay);
        TXW51_GPIO_ClearGpio(TXW51_GPIO_PIN_LED2);
        nrf_delay_us(delay);
    }
    return 0;
}

