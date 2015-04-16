/***************************************************************************//**
 * @brief   Module with an example on how to use the ADC.
 *
 * @file    adc_example.c
 * @version 1.0
 * @date    12.12.2014
 * @author  Daniel Meer
 *
 * @remark  Last Modifications:
 *          12.12.2014 meerd1 created
 ******************************************************************************/

/*----- Header-Files ---------------------------------------------------------*/
#include "adc_example.h"

#include "txw51_framework/hw/adc.h"
#include "txw51_framework/utils/log.h"

#include "app/error.h"

/*----- Macros ---------------------------------------------------------------*/

/*----- Data types -----------------------------------------------------------*/

/*----- Function prototypes --------------------------------------------------*/
static void ACD_EXMPL_CallbackHandler(void *context);

/*----- Data -----------------------------------------------------------------*/
static app_timer_id_t timerHandle;  /**< Handle for the used timer. */

/*----- Implementation -------------------------------------------------------*/

uint32_t APPL_ADC_EXMPL_Init(void)
{
    uint32_t err;

    struct TXW51_ADC_InitTab init = {
        .RefSelection   = ADC_CONFIG_REFSEL_External,
        .InputSelection = ADC_CONFIG_INPSEL_AnalogInputOneThirdPrescaling,
        .Resolution     = ADC_CONFIG_RES_8bit
    };
    TXW51_ADC_Init(&init);

    err = app_timer_create(&timerHandle,
                           APP_TIMER_MODE_REPEATED,
                           &ACD_EXMPL_CallbackHandler);
    if (err != NRF_SUCCESS) {
        TXW51_LOG_ERROR("[ADC] Could not initialize ADC.");
        return ERR_ADC_EXMPL_INIT_FAILED;
    }

    TXW51_LOG_DEBUG("[ADC] Initialization successful.");
    return ERR_NONE;
}


uint32_t APPL_ADC_EXMPL_Start(void)
{
    uint32_t err;

    /* Start the timer. */
    err = app_timer_start(timerHandle, APPL_ADC_EXMPL_SAMPLING_INTERVAL, NULL);
    if (err != NRF_SUCCESS) {
        TXW51_LOG_ERROR("[ADC] Could not start ADC.");
        return ERR_ADC_EXMPL_START_FAILED;
    }

    TXW51_LOG_DEBUG("[ADC] Started.");
    return ERR_NONE;
}


/***************************************************************************//**
 * @brief Timer callback handler for the ADC example.
 *
 * This function gets called every time the timer expires.
 *
 * @param[in] context The context of the timer interrupt.
 *
 * @return Nothing.
 ******************************************************************************/
static void ACD_EXMPL_CallbackHandler(void *context)
{
    /* Start ADC sampling. */
    NRF_ADC->TASKS_START = 1U;
}


void APPL_ADC_EXMPL_HandleResult(uint16_t value)
{
    char buffer[30];
    sprintf(buffer, "[ADC Example] Value: %d", value);
    TXW51_LOG_INFO(buffer);
}

