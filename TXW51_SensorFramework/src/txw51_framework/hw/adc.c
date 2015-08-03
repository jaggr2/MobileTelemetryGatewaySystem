/***************************************************************************//**
 * @brief   Module to initialize and use the analog-digial-converter of the
 *          TXW51 board.
 *
 * @file    adc.c
 * @version 1.0
 * @date    12.12.2014
 * @author  Daniel Meer
 *
 * @remark  Last Modifications:
 *          12.12.2014 meerd1 created
 ******************************************************************************/
/*----- Header-Files ---------------------------------------------------------*/
#include "adc.h"

#include "txw51_framework/utils/log.h"
#include "txw51_framework/utils/txw51_errors.h"

/*----- Macros ---------------------------------------------------------------*/

/*----- Data types -----------------------------------------------------------*/

/*----- Function prototypes --------------------------------------------------*/

/*----- Data -----------------------------------------------------------------*/

/*----- Implementation -------------------------------------------------------*/

void TXW51_ADC_Init(const struct TXW51_ADC_InitTab *init)
{
    /* Enable interrupt on ADC sample ready event. */
    //NRF_ADC->INTENSET = ADC_INTENSET_END_Msk;
    //sd_nvic_SetPriority(ADC_IRQn, TXW51_ADC_IRQ_PRIORITY);
    //sd_nvic_EnableIRQ(ADC_IRQn);

    /* Configure ADC. */
    uint32_t bits = (TXW51_ADC_PIN_SELECTION << ADC_CONFIG_PSEL_Pos) |
                    (init->RefSelection << ADC_CONFIG_REFSEL_Pos) |
                    (init->InputSelection << ADC_CONFIG_INPSEL_Pos) |
                    (init->Resolution << ADC_CONFIG_RES_Pos);

    if (init->RefSelection == ADC_CONFIG_REFSEL_External) {
        bits |= (TXW51_ADC_REF_PIN_SELECTION << ADC_CONFIG_EXTREFSEL_Pos);
    } else {
        bits |= (ADC_CONFIG_EXTREFSEL_None << ADC_CONFIG_EXTREFSEL_Pos);
    }

    NRF_ADC->CONFIG = bits;

    /* Enable ADC. */
    NRF_ADC->ENABLE = ADC_ENABLE_ENABLE_Enabled;
}

