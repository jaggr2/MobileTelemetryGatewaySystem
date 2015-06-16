/***************************************************************************//**
 * @brief   Module to initialize and use the GPIOs of the TXW51 module.
 *
 * @file    gpio.c
 * @version 1.0
 * @date    10.11.2014
 * @author  Daniel Meer
 *
 * @remark  Last Modifications:
 *          10.11.2014 meerd1 created
 ******************************************************************************/

/*----- Header-Files ---------------------------------------------------------*/
#include "gpio.h"

#include "nrf/s110/nrf_soc.h"
#include "nrf/sd_common/app_util_platform.h"

#include "txw51_framework/config/config.h"

/*----- Macros ---------------------------------------------------------------*/

/*----- Data types -----------------------------------------------------------*/

/*----- Function prototypes --------------------------------------------------*/
static __INLINE void GPIO_ConfigGpioAsLed(uint32_t pinNumber);

/*----- Data -----------------------------------------------------------------*/

/*----- Implementation -------------------------------------------------------*/

void TXW51_GPIO_InitLed(void)
{
    GPIO_ConfigGpioAsLed(TXW51_GPIO_PIN_LED0);
    GPIO_ConfigGpioAsLed(TXW51_GPIO_PIN_LED1);
    GPIO_ConfigGpioAsLed(TXW51_GPIO_PIN_LED2);

    TXW51_GPIO_ClearGpio(TXW51_GPIO_PIN_LED0);
    TXW51_GPIO_ClearGpio(TXW51_GPIO_PIN_LED1);
    TXW51_GPIO_ClearGpio(TXW51_GPIO_PIN_LED2);
}


void TXW51_GPIO_DeinitLed(void)
{
    TXW51_GPIO_ConfigGpioAsDisconnected(TXW51_GPIO_PIN_LED0);
    TXW51_GPIO_ConfigGpioAsDisconnected(TXW51_GPIO_PIN_LED1);
    TXW51_GPIO_ConfigGpioAsDisconnected(TXW51_GPIO_PIN_LED2);
}


void TXW51_GPIO_SetGpio(enum TXW51_GPIO_Pin pinNumber)
{
    nrf_gpio_pin_set(pinNumber);
}


void TXW51_GPIO_ClearGpio(enum TXW51_GPIO_Pin pinNumber)
{
    nrf_gpio_pin_clear(pinNumber);
}


void TXW51_GPIO_ToggleGpio(enum TXW51_GPIO_Pin pinNumber)
{
    nrf_gpio_pin_toggle(pinNumber);
}


void TXW51_GPIO_ConfigGpioAsOutput(enum TXW51_GPIO_Pin pinNumber)
{
    nrf_gpio_cfg_output(pinNumber);
}


void TXW51_GPIO_ConfigGpioAsInput(enum TXW51_GPIO_Pin pinNumber,
                                  nrf_gpio_pin_pull_t pullConfig)
{
    nrf_gpio_cfg_input(pinNumber, pullConfig);
}


void TXW51_GPIO_ConfigGpioAsDisconnected(enum TXW51_GPIO_Pin pinNumber)
{
    NRF_GPIO->PIN_CNF[pinNumber] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos) |
                                   (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos) |
                                   (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos) |
                                   (GPIO_PIN_CNF_INPUT_Disconnect << GPIO_PIN_CNF_INPUT_Pos) |
                                   (GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos);
}


/* TODO: Add unset function. */
void TXW51_GPIOTE_SetInterrupt(uint32_t channel,
                               enum TXW51_GPIO_Pin pinNumber,
                               nrf_gpiote_polarity_t polarity)
{
    /* Configure GPIOTE channel to generate event. */
    nrf_gpiote_event_config(channel, pinNumber, polarity);

    /* Enable interrupt for NRF_GPIOTE->EVENTS_IN[x] event. */
    switch (channel) {
        case 0:
            NRF_GPIOTE->INTENSET = GPIOTE_INTENSET_IN0_Msk;
            break;

        case 1:
            NRF_GPIOTE->INTENSET = GPIOTE_INTENSET_IN1_Msk;
            break;

        case 2:
            NRF_GPIOTE->INTENSET = GPIOTE_INTENSET_IN2_Msk;
            break;

        case 3:
            NRF_GPIOTE->INTENSET = GPIOTE_INTENSET_IN3_Msk;
            break;
    }

    /* Enable GPIOTE interrupt in Nested Vector Interrupt Controller. */
    sd_nvic_ClearPendingIRQ(GPIOTE_IRQn);
    sd_nvic_SetPriority(GPIOTE_IRQn, TXW51_GPIOTE_IRQ_PRIORITY);
    sd_nvic_EnableIRQ(GPIOTE_IRQn);
}


void TXW51_GPIO_SetSenseInterrupt(enum TXW51_GPIO_Pin pinNumber,
                                  nrf_gpio_pin_pull_t pullConfig)
{
    nrf_gpio_cfg_sense_input(pinNumber, pullConfig, GPIO_PIN_CNF_SENSE_High);
}


/***************************************************************************//**
 * @brief Configures a GPIO pin as a LED output.
 *
 * The difference to a normal GPIO pin is that the output drive strength is
 * higher.
 *
 * @param[in] pinNumber The GPIO number (between 0 and 31).
 * @return Nothing.
 ******************************************************************************/
static __INLINE void GPIO_ConfigGpioAsLed(uint32_t pinNumber)
{
    NRF_GPIO->PIN_CNF[pinNumber] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos) |
                                   (GPIO_PIN_CNF_DRIVE_S0H1 << GPIO_PIN_CNF_DRIVE_Pos) |
                                   (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos) |
                                   (GPIO_PIN_CNF_INPUT_Disconnect << GPIO_PIN_CNF_INPUT_Pos) |
                                   (GPIO_PIN_CNF_DIR_Output << GPIO_PIN_CNF_DIR_Pos);
}

