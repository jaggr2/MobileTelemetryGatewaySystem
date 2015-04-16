/***************************************************************************//**
 * @brief   Module to initialize and use the analog-digial-converter of the
 *          TXW51 board.
 *
 * @file    adc.h
 * @version 1.0
 * @date    12.12.2014
 * @author  Daniel Meer
 *
 * @remark  Last Modifications:
 *          12.12.2014 meerd1 created
 ******************************************************************************/

#ifndef TXW51_FRAMEWORK_HW_ADC_H_
#define TXW51_FRAMEWORK_HW_ADC_H_

/*----- Header-Files ---------------------------------------------------------*/
#include <stdint.h>

#include "nrf/s110/nrf_soc.h"

/*----- Macros ---------------------------------------------------------------*/
#define TXW51_ADC_IRQ_PRIORITY          ( (uint8_t)NRF_APP_PRIORITY_LOW )   /**< IRQ priority for the ADC interrupt. */

#define TXW51_ADC_PIN_SELECTION         ( ADC_CONFIG_PSEL_AnalogInput6 )    /**< Definition of the ADC hardware pin. */
#define TXW51_ADC_REF_PIN_SELECTION     ( ADC_CONFIG_EXTREFSEL_AnalogReference1 )   /**< Definition of the ADC reference hardware pin. */

/*----- Data types -----------------------------------------------------------*/
/**
 * @brief Structure with the initialization parameters.
 */
struct TXW51_ADC_InitTab {
    uint32_t RefSelection;      /**< Source of the reference voltage. */
    uint32_t InputSelection;    /**< Source of the input voltage. */
    uint32_t Resolution;        /**< Chosen bit resolution (8, 9 oder 10 bit). */
};

/*----- Function prototypes --------------------------------------------------*/

/***************************************************************************//**
 * @brief Initializes the ADC.
 *
 * @param[in] init Structure with the initialization parameters.
 * @return Nothing.
 ******************************************************************************/
extern void TXW51_ADC_Init(const struct TXW51_ADC_InitTab *init);

/*----- Data -----------------------------------------------------------------*/

#endif /* TXW51_FRAMEWORK_HW_ADC_H_ */
