/***************************************************************************//**
 * @brief   Main module of the application.
 *
 * First, the hardware and the Bluetooth Smart stack get set up. Afterwards,
 * the main loop gets started.
 *
 * @file    appl.c
 * @version 1.0
 * @date    10.11.2014
 * @author  Daniel Meer
 *
 * @remark  Last Modifications:
 *          10.11.2014 meerd1 created
 *          10.04.2015 bohnp1 add contactless temperature service.
 ******************************************************************************/

/*----- Header-Files ---------------------------------------------------------*/
#include "appl.h"

#include "txw51_framework/ble/btle.h"
#include "txw51_framework/ble/cb.h"
#include "txw51_framework/hw/gpio.h"
#include "txw51_framework/hw/spi.h"
#include "txw51_framework/hw/uart.h"
#include "txw51_framework/utils/log.h"
#include "txw51_framework/utils/setup.h"

#include "app/device_info.h"
#include "app/error.h"
#include "app/fifo.h"
#include "app/measurement.h"
#include "app/sensor.h"
#include "app/timer.h"
#include "app/contactless_temp.h"

/*----- Macros ---------------------------------------------------------------*/

/*----- Data types -----------------------------------------------------------*/

/*----- Function prototypes --------------------------------------------------*/
static void APPL_Sleep(void);
static void APPL_BleEventHandler(ble_evt_t *bleEvent);
static void APPL_Init(void);

/*----- Data -----------------------------------------------------------------*/
bool gIsNewAccDataAvailable = false;
bool gIsNewGyroDataAvailable = false;
bool gIsTimeout = false;

static struct TXW51_SERV_DIS_Handle serviceHandleDis;           /**< Handle for the DIS Bluetooth service. */
static struct TXW51_SERV_LSM330_Handle serviceHandleLsm330;     /**< Handle for the LSM330 Bluetooth service. */
static struct TXW51_SERV_MEASURE_Handle serviceHandleMeasure;   /**< Handle for the Measurement Bluetooth service. */
static struct TXW51_SERV_TEMP_CONTACTLESS_Handle serviceHandleContactlessTemp;	/**< Handle for the contactless temperature Bluetooth service. */

/*----- Implementation -------------------------------------------------------*/

/***************************************************************************//**
 * @brief Puts the device into a deep sleep mode.
 *
 * @return Nothing.
 ******************************************************************************/
static void APPL_Sleep(void)
{
    TXW51_LOG_INFO("Go to sleep.");

    /* Configure the sensor to wake up the device when it gets moved. */
    APPL_SENSOR_SetupMotionWakeup();

    while (APPL_DEVINFO_IsBusy()) { /* Wait for flash transactions to finish */ }

    /* Turn off function blocks and disconnect the GPIO pins. */
    TXW51_UART_Deinit();
    TXW51_SPI_Deinit(SPI_MASTER_0);
    TXW51_GPIO_DeinitLed();

    /* Turn off the HFCLK. */
    sd_clock_hfclk_release();

    sd_power_system_off();
}


/***************************************************************************//**
 * @brief Handles the events from the Bluetooth Smart module.
 *
 * @param[in,out] bleEvent The event, that occurred.
 *
 * @return Nothing.
 ******************************************************************************/
static void APPL_BleEventHandler(ble_evt_t *bleEvent)
{
    /* Call BLE event handler of the services. */
    TXW51_SERV_DIS_OnBleEvent(&serviceHandleDis, bleEvent);
    TXW51_SERV_LSM330_OnBleEvent(&serviceHandleLsm330, bleEvent);
    TXW51_SERV_MEASURE_OnBleEvent(&serviceHandleMeasure, bleEvent);
    TXW51_SERV_TEMP_CONTACTLESS_OnBleEvent(&serviceHandleContactlessTemp, bleEvent);

    /* Local BLE event handling. */
    switch (bleEvent->header.evt_id) {
        case BLE_GAP_EVT_CONNECTED:
            APPL_TIMER_Stop();
            TXW51_GPIO_ClearGpio(CONFIG_HW_LED_ADVERTISING);
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            APPL_TIMER_Start();
            TXW51_GPIO_SetGpio(CONFIG_HW_LED_ADVERTISING);
            break;

        case BLE_GAP_EVT_TIMEOUT:
            if (bleEvent->evt.gap_evt.params.timeout.src ==
                    BLE_GAP_TIMEOUT_SRC_ADVERTISEMENT) {
                NVIC_SystemReset();
            }
            break;

        default:
            /* No implementation needed. */
            break;
    }
}


/***************************************************************************//**
 * @brief Initializes all the modules of the application.
 *
 * @return Nothing.
 ******************************************************************************/
static void APPL_Init(void)
{
    TXW51_LOG_INFO("Starting initialization ...");

    TXW51_SETUP_InitSoftdevice();
    TXW51_SETUP_InitScheduler();
    TXW51_SETUP_InitHfClock();
    APPL_TIMER_Init();

    TXW51_GPIO_InitLed();
    TXW51_GPIO_SetGpio(CONFIG_HW_LED_ON);
    TXW51_SPI_Init(TXW51_SPI_0);
    APPL_SENSOR_Init();

    APPL_FIFO_Init();
    APPL_DEVINFO_Init();
    APPL_DEVINFO_Load();
    while (APPL_DEVINFO_IsBusy()) { /* Wait. */ }

    TXW51_BLE_Init();
    APPL_DEVINFO_InitService(&serviceHandleDis);
    APPL_SENSOR_InitService(&serviceHandleLsm330);
    APPL_MEASUREMENT_InitService(&serviceHandleMeasure);
    APPL_CONTACTLESS_TEMP_InitService(&serviceHandleContactlessTemp);
    TXW51_BLE_InitAdvertising();

    TXW51_LOG_INFO("Initialization successful!");
}


void APPL_Start(void)
{
    TXW51_LOG_Init();
    TXW51_LOG_INFO("");
    TXW51_LOG_INFO("-------------");
    TXW51_LOG_INFO("--- TXW51 ---");
    TXW51_LOG_INFO("-------------");

    /* Initialize device. */
    APPL_Init();

    // TODO: Make better.
    /* Functions for loading and saving default device information to flash. */
//    APPL_DEVINFO_SetDefaultValues();
//    APPL_DEVINFO_Save();

    /* Start execution. */
    APPL_DEVINFO_PrintValues();
    TXW51_CB_RegisterBleCallback(APPL_BleEventHandler);
    TXW51_BLE_StartAdvertising();
    TXW51_GPIO_SetGpio(CONFIG_HW_LED_ADVERTISING);
    APPL_TIMER_Start();

    uint32_t err = NRF_SUCCESS;
    while (true) {
        app_sched_execute();

        if (gIsTimeout) {
            APPL_Sleep();
        }

        if (gIsNewAccDataAvailable || gIsNewGyroDataAvailable) {
            APPL_MEASUREMENT_SendAllData(TXW51_SERV_MEASURE_TX_NOTIFICATION);
        }

        err = sd_app_evt_wait();
        APP_ERROR_CHECK(err);
    }
}

