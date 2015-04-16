/***************************************************************************//**
 * @brief   Module with an example on how to use the ADC.
 *
 * @file    adc_example.h
 * @version 1.0
 * @date    12.12.2014
 * @author  Daniel Meer
 *
 * @remark  Last Modifications:
 *          12.12.2014 meerd1 created
 ******************************************************************************/

#ifndef TXW51_APPLICATION_ADC_EXAMPLE_H_
#define TXW51_APPLICATION_ADC_EXAMPLE_H_

/*----- Header-Files ---------------------------------------------------------*/
#include <stdint.h>

/*----- Macros ---------------------------------------------------------------*/
#define APPL_ADC_EXMPL_SAMPLING_INTERVAL    ( 1UL * RTC_FREQUENCY )     /**< ADC sampling interval in ((# of seconds)*(RTC_FREQUENCY)). */

/*----- Data types -----------------------------------------------------------*/

/*----- Function prototypes --------------------------------------------------*/

/***************************************************************************//**
 * @brief Initialize the ADC module and the example.
 *
 * @return ERR_NONE if no error occurred.
 *         ERR_ADC_EXMPL_INIT_FAILED if initialization failed.
 ******************************************************************************/
extern uint32_t APPL_ADC_EXMPL_Init(void);

/***************************************************************************//**
 * @brief Starts the ADC.
 *
 * @return ERR_NONE if no error occurred.
 *         ERR_ADC_EXMPL_START_FAILED if ADC timer could not be started.
 ******************************************************************************/
extern uint32_t APPL_ADC_EXMPL_Start(void);

/***************************************************************************//**
 * @brief Prints the value to the log.
 *
 * @param[in] value The ADC value to handle.
 *
 * @return Nothing.
 ******************************************************************************/
extern void APPL_ADC_EXMPL_HandleResult(uint16_t value);

/*----- Data -----------------------------------------------------------------*/

#endif /* TXW51_APPLICATION_ADC_EXAMPLE_H_ */
