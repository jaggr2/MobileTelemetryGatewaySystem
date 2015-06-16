/***************************************************************************//**
 * @brief   This module can be used to test the throughput of the Bluetooth
 *          Smart connection according to the Test Specification entry T143.
 *
 * You have to include this file to the build and exclude the file with the
 * real main function.
 *
 * After the peer device has set the CCCD descriptor and the start
 * characteristic, it writes 200 kBit (= 25 kByte) to the datastream
 * characteristic.
 *
 * @file    test_throughput.c
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

#include "txw51_framework/ble/btle.h"
#include "txw51_framework/ble/cb.h"
#include "txw51_framework/utils/log.h"

#include "app/error.h"
#include "app/measurement.h"
#include "app/timer.h"

/*----- Macros ---------------------------------------------------------------*/

/*----- Data types -----------------------------------------------------------*/

/*----- Function prototypes --------------------------------------------------*/
static void initClocks(void);
static uint32_t initService();
static void measurementBleEventHandler(struct TXW51_SERV_MEASURE_Handle *handle,
                                       struct TXW51_SERV_MEASURE_Event *evt);
static void sendAllData(enum TXW51_SERV_MEASURE_TxType txType);
static void bleEventHandler(ble_evt_t *bleEvent);
static void initSoftdevice();

/*----- Data -----------------------------------------------------------------*/
static bool isIndicationBusy = false;           /**< Flag to wait until an indication has been successfully received. */
static uint8_t notificationPacketCount = 0;     /**< Number of notifications that we can send at a given time. */
static bool isMeasurementStarted = false;   /**< Flag to indicate when measurement has started. */

static struct TXW51_SERV_MEASURE_Handle serviceHandleMeasure;   /**< Handle for the Measurement Bluetooth service. */

static int8_t dataBlock[1000];              /**< Buffer with 1000 Byte. Send this 25 times. */

/*----- Implementation -------------------------------------------------------*/

static void initClocks(void)
{
    uint32_t isClockRunning = 0;

    sd_clock_hfclk_request();
    while (!isClockRunning) {
        sd_clock_hfclk_is_running(&isClockRunning);
    }
}


static uint32_t initService()
{
    sd_ble_tx_buffer_count_get(&notificationPacketCount);

    struct TXW51_SERV_MEASURE_Init measureInit;
    measureInit.EventHandler = measurementBleEventHandler;

    TXW51_SERV_MEASURE_Init(&serviceHandleMeasure, &measureInit);
}


static void measurementBleEventHandler(struct TXW51_SERV_MEASURE_Handle *handle,
                                       struct TXW51_SERV_MEASURE_Event *evt)
{
    switch (evt->EventType) {
        case TXW51_SERV_MEASURE_EVT_START:
            if (isMeasurementStarted) {
                TXW51_LOG_INFO("[Measure Service] Measurement already started!");
                return;
            }

            isMeasurementStarted = true;
            TXW51_LOG_INFO("[Measure Service] Start measurement");
            break;

        case TXW51_SERV_MEASURE_EVT_STOP:
            isMeasurementStarted = false;
            TXW51_LOG_INFO("[Measure Service] Stop measurement");
            break;

        case TXW51_SERV_MEASURE_EVT_ENABLE_DATASTREAM:
            TXW51_LOG_INFO("[Measure Service] Enable data stream");
            break;

        case TXW51_SERV_MEASURE_EVT_DISABLE_DATASTREAM:
            TXW51_LOG_INFO("[Measure Service] Disable data stream");
            break;

        case TXW51_SERV_MEASURE_EVT_INDICATION_RECEIVED:
            isIndicationBusy = false;
            break;

        case TXW51_SERV_MEASURE_EVT_NOTIFICATIONS_SENT:
            notificationPacketCount += *evt->Value;
            break;

        default:
            break;
    }
}


static void sendAllData(enum TXW51_SERV_MEASURE_TxType txType)
{
    if (((txType == TXW51_SERV_MEASURE_TX_INDICATION)   && isIndicationBusy) ||
        ((txType == TXW51_SERV_MEASURE_TX_NOTIFICATION) && (notificationPacketCount == 0))) {
        return;
    }

    static int32_t index = 0;
    static int32_t iteration = 0;
    struct TXW51_SERV_MEASURE_DataPacket *packet = (struct TXW51_SERV_MEASURE_DataPacket *) (dataBlock + index);
    index += 20;

    if (index >= 1000) {
        index = 0;
        iteration++;

        if (iteration >= 25) {
            isMeasurementStarted = false;
        }
    }


    if (txType == TXW51_SERV_MEASURE_TX_INDICATION) {
        isIndicationBusy = true;
    } else {
        notificationPacketCount--;
    }
    TXW51_SERV_MEASURE_SendData(txType,
                                &serviceHandleMeasure,
                                packet);
}


static void bleEventHandler(ble_evt_t *bleEvent)
{
    /* Call BLE event handler of the services. */
    TXW51_SERV_MEASURE_OnBleEvent(&serviceHandleMeasure, bleEvent);

    /* Local BLE event handling. */
    switch (bleEvent->header.evt_id) {
        case BLE_GAP_EVT_CONNECTED:
            break;

        case BLE_GAP_EVT_DISCONNECTED:
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


static void initSoftdevice()
{
    SOFTDEVICE_HANDLER_INIT(CONFIG_CLOCK_LFCLK_SOURCE, true);
    APP_SCHED_INIT(CONFIG_SCHED_MAX_EVENT_DATA_SIZE, CONFIG_SCHED_QUEUE_SIZE);
    APP_TIMER_INIT(CONFIG_TIMERS_PRESCALER,
                       CONFIG_TIMERS_MAX_TIMERS,
                       CONFIG_TIMERS_OP_QUEUE_SIZE,
                       true);
}


int main(void)
{
    for (int32_t i = 0; i < 1000; i++) {
        dataBlock[i] = i;
    }

    TXW51_LOG_Init();
    initSoftdevice();
    initClocks();
    TXW51_BLE_Init();
    initService();
    TXW51_BLE_InitAdvertising();

    TXW51_CB_RegisterBleCallback(bleEventHandler);
    TXW51_BLE_StartAdvertising();

    while (true) {
        app_sched_execute();

        if (isMeasurementStarted) {
            sendAllData(TXW51_SERV_MEASURE_TX_NOTIFICATION);
        }

        sd_app_evt_wait();
    }

    return 0;
}

