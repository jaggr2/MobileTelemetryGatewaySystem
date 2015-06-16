/***************************************************************************//**
 * @brief   Contains all the IRQ handler.
 *
 * @file    irq_handler.c
 * @version 1.0
 * @date    18.01.2015
 * @author  Daniel Meer
 *
 * @remark  Last Modifications:
 *          18.01.2015 meerd1 created
 ******************************************************************************/

/*----- Header-Files ---------------------------------------------------------*/
#include "nrf/nrf.h"

#include "txw51_framework/hw/lsm330.h"

#include "app/adc_example.h"
#include "app/sensor.h"

/*----- Macros ---------------------------------------------------------------*/

/*----- Data types -----------------------------------------------------------*/

/*----- Function prototypes --------------------------------------------------*/

/*----- Data -----------------------------------------------------------------*/

/*----- Implementation -------------------------------------------------------*/

/***************************************************************************//**
 * @brief Handles the interrupt events from the GPIOTE module.
 *
 * @return Nothing.
 ******************************************************************************/
void GPIOTE_IRQHandler(void)
{
    if (NRF_GPIOTE->EVENTS_IN[TXW51_LSM330_GPIO_INT1_ACC_CHANNEL]) {
        APPL_SENSOR_HandleInterrupt(TXW51_LSM330_GPIO_INT1_ACC_CHANNEL);
        /* Event causing the interrupt must be cleared. */
        NRF_GPIOTE->EVENTS_IN[TXW51_LSM330_GPIO_INT1_ACC_CHANNEL] = 0;
    }

    if (NRF_GPIOTE->EVENTS_IN[TXW51_LSM330_GPIO_INT2_GYRO_CHANNEL]) {
        APPL_SENSOR_HandleInterrupt(TXW51_LSM330_GPIO_INT2_GYRO_CHANNEL);
        /* Event causing the interrupt must be cleared. */
        NRF_GPIOTE->EVENTS_IN[TXW51_LSM330_GPIO_INT2_GYRO_CHANNEL] = 0;
    }
}


/***************************************************************************//**
 * @brief Handles the interrupt events from the ADC module.
 *
 * @return Nothing.
 ******************************************************************************/
void ADC_IRQHandler(void)
{
    /* Clear data-ready event. */
    NRF_ADC->EVENTS_END = 0;

    APPL_ADC_EXMPL_HandleResult(NRF_ADC->RESULT);

    /* Use the STOP task to save current. Workaround for PAN_028 rev1.5 anomaly 1. */
    NRF_ADC->TASKS_STOP = 1;
}

