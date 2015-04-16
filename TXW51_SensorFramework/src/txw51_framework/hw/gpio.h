/***************************************************************************//**
 * @brief   Module to initialize and use the GPIOs of the TXW51 module.
 *
 * @file    gpio.h
 * @version 1.0
 * @date    10.11.2014
 * @author  Daniel Meer
 *
 * @remark  Last Modifications:
 *          10.11.2014 meerd1 created
 ******************************************************************************/

#ifndef TXW51_FRAMEWORK_HW_GPIO_H_
#define TXW51_FRAMEWORK_HW_GPIO_H_

/*----- Header-Files ---------------------------------------------------------*/
#include "nrf/nrf_gpio.h"
#include "nrf/nrf_gpiote.h"

/*----- Macros ---------------------------------------------------------------*/
#define TXW51_GPIOTE_IRQ_PRIORITY     ( APP_IRQ_PRIORITY_HIGH )     /**< IRQ priority for the GPIOTE module. */

/*----- Data types -----------------------------------------------------------*/
/**
 * @brief Definition of the available GPIO pins.
 */
enum TXW51_GPIO_Pin {
    TXW51_GPIO_PIN_LED0     = 22,   /**< GPIO pin number for the first LED. */
    TXW51_GPIO_PIN_LED1     = 14,   /**< GPIO pin number for the second LED. */
    TXW51_GPIO_PIN_LED2     = 21,   /**< GPIO pin number for the third LED. */

    TXW51_GPIO_PIN_UART0_RX = 18,   /**< UART RX GPIO pin number. */
    TXW51_GPIO_PIN_UART0_TX = 17,   /**< UART TX GPIO pin number. */

    TXW51_GPIO_PIN_SPI0_SCLK = 7,   /**< SPI0 SCLK GPIO pin number. */
    TXW51_GPIO_PIN_SPI0_MISO = 3,   /**< SPI0 MISO GPIO pin number. */
    TXW51_GPIO_PIN_SPI0_MOSI = 4,   /**< SPI0 MOSI GPIO pin number. */
    TXW51_GPIO_PIN_SPI0_SS_A = 1,   /**< SPI0 SS accel GPIO pin number. */
    TXW51_GPIO_PIN_SPI0_SS_G = 2,   /**< SPI0 SS gyro GPIO pin number. */

    TXW51_GPIO_PIN_INT1_G    = 30,  /**< GPIO pin number for LSM330 INT1_G pin. */
    TXW51_GPIO_PIN_INT2_G    = 0,   /**< GPIO pin number for LSM330 INT2_G pin. */
    TXW51_GPIO_PIN_INT1_A    = 29,  /**< GPIO pin number for LSM330 INT1_A pin. */
    TXW51_GPIO_PIN_INT2_A    = 28,  /**< GPIO pin number for LSM330 INT2_A pin. */
    TXW51_GPIO_PIN_DEN_G     = 24,  /**< GPIO pin number for LSM330 DEN_G pin. */

    TXW51_GPIO_PIN_SPI1_SCLK = 8,   /**< SPI1 SCLK GPIO pin number. */
    TXW51_GPIO_PIN_SPI1_MISO = 9,   /**< SPI1 MISO GPIO pin number. */
    TXW51_GPIO_PIN_SPI1_MOSI = 10,  /**< SPI1 MOSI GPIO pin number. */
    TXW51_GPIO_PIN_SPI1_SS   = 11,  /**< SPI1 SS GPIO pin number. */

    TXW51_GPIO_PIN_ADC0      = 5,   /**< GPIO pin number of ADC input. */
    TXW51_GPIO_PIN_ADC_REF   = 6,   /**< GPIO pin number of ADC reference voltage. */

    TXW51_GPIO_PIN_GPIO0     = 6,   /**< Additionally usable GPIO pin 0 (on connector). */
    TXW51_GPIO_PIN_GPIO1     = 15,  /**< Additionally usable GPIO pin 1 (on connector). */
};

/*----- Function prototypes --------------------------------------------------*/

/***************************************************************************//**
 * @brief Initializes the three LEDs of the TXW51.
 *
 * @return Nothing.
 ******************************************************************************/
extern void TXW51_GPIO_InitLed(void);

/***************************************************************************//**
 * @brief Deinitializes the three LEDs of the TXW51.
 *
 * This can be used to save energy on the GPIO pins.
 *
 * @return Nothing.
 ******************************************************************************/
extern void TXW51_GPIO_DeinitLed(void);

/***************************************************************************//**
 * @brief Sets a GPIO pin.
 *
 * @param[in] pinNumber The GPIO number (between 0 and 31).
 * @return Nothing.
 ******************************************************************************/
extern void TXW51_GPIO_SetGpio(enum TXW51_GPIO_Pin pinNumber);

/***************************************************************************//**
 * @brief Clears a GPIO pin.
 *
 * @param[in] pinNumber The GPIO number (between 0 and 31).
 * @return Nothing.
 ******************************************************************************/
extern void TXW51_GPIO_ClearGpio(enum TXW51_GPIO_Pin pinNumber);

/***************************************************************************//**
 * @brief Toggles a GPIO pin.
 *
 * @param[in] pinNumber The GPIO number (between 0 and 31).
 * @return Nothing.
 ******************************************************************************/
extern void TXW51_GPIO_ToggleGpio(enum TXW51_GPIO_Pin pinNumber);

/***************************************************************************//**
 * @brief Configures a GPIO pin as an output.
 *
 * @param[in] pinNumber The GPIO number (between 0 and 31).
 * @return Nothing.
 ******************************************************************************/
extern void TXW51_GPIO_ConfigGpioAsOutput(enum TXW51_GPIO_Pin pinNumber);

/***************************************************************************//**
 * @brief Configures a GPIO pin as an input.
 *
 * @param[in] pinNumber  The GPIO number (between 0 and 31).
 * @param[in] pullConfig Pull-up/down configuration.
 * @return Nothing.
 ******************************************************************************/
extern void TXW51_GPIO_ConfigGpioAsInput(enum TXW51_GPIO_Pin pinNumber,
                                         nrf_gpio_pin_pull_t pullConfig);

/***************************************************************************//**
 * @brief Configures a GPIO pin as disconnected.
 *
 * This can be used to lower the power consumption. Through disconnected pins
 * can flow no current.
 *
 * @param[in] pinNumber The GPIO number (between 0 and 31).
 * @return Nothing.
 ******************************************************************************/
extern void TXW51_GPIO_ConfigGpioAsDisconnected(enum TXW51_GPIO_Pin pinNumber);

/***************************************************************************//**
 * @brief Configure the sense functionality of a GPIO.
 *
 * With the sense functionality, an external interrupt can wake up the device
 * in its lowest energy mode.
 *
 * @param[in] pinNumber  The GPIO number (between 0 and 31).
 * @param[in] pullConfig Pull-up/down configuration.
 * @return Nothing.
 ******************************************************************************/
extern void TXW51_GPIO_SetSenseInterrupt(enum TXW51_GPIO_Pin pinNumber,
                                         nrf_gpio_pin_pull_t pullConfig);

/***************************************************************************//**
 * @brief Use the GPIOTE module to configure one of four possible GPIO
 *        interrupts.
 *
 * @param[in] channel   The GPIOTE channel to use (between 0 and 4).
 * @param[in] pinNumber The GPIO number (between 0 and 31).
 * @param[in] polarity  Polarity on how the interrupt gets triggered.
 * @return Nothing.
 ******************************************************************************/
extern void TXW51_GPIOTE_SetInterrupt(uint32_t channel,
                                      enum TXW51_GPIO_Pin pinNumber,
                                      nrf_gpiote_polarity_t polarity);

/*----- Data -----------------------------------------------------------------*/

#endif /* TXW51_FRAMEWORK_HW_GPIO_H_ */
