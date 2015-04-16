/***************************************************************************//**
 * @brief   Module that handles the Bluetooth Smart Measurement Service.
 *
 * It initializes and communicates with the Measurement service. Furthermore,
 * it starts the measurement and sends the data over the Bluetooth link.
 *
 * @file    measurement.c
 * @version 1.0
 * @date    09.12.2014
 * @author  Daniel Meer
 *
 * @remark  Last Modifications:
 *          09.12.2014 meerd1 created
 ******************************************************************************/

/*----- Header-Files ---------------------------------------------------------*/
#include "measurement.h"

#include "txw51_framework/utils/log.h"

#include "app/appl.h"
#include "app/error.h"
#include "app/fifo.h"
#include "app/sensor.h"

/*----- Macros ---------------------------------------------------------------*/

/*----- Data types -----------------------------------------------------------*/

/*----- Function prototypes --------------------------------------------------*/
static void MEASUREMENT_BleEventHandler(struct TXW51_SERV_MEASURE_Handle *handle,
                                        struct TXW51_SERV_MEASURE_Event *evt);

/*----- Data -----------------------------------------------------------------*/
static struct TXW51_SERV_MEASURE_Handle *measurementServiceHandle = NULL;   /**< Reference to the handle for the Bluetooth Smart Measurement Service. */

static bool isIndicationBusy = false;           /**< Flag to wait until an indication has been successfully received. */
static uint8_t sequenceNumber = 0;              /**< Sequence number of the packets to send. */
static uint8_t notificationPacketCount = 0;     /**< Number of notifications that we can send at a given time. */

/*----- Implementation -------------------------------------------------------*/

uint32_t APPL_MEASUREMENT_InitService(struct TXW51_SERV_MEASURE_Handle *serviceHandle)
{
    uint32_t err = ERR_NONE;

    sd_ble_tx_buffer_count_get(&notificationPacketCount);

    struct TXW51_SERV_MEASURE_Init measureInit;
    measureInit.EventHandler = MEASUREMENT_BleEventHandler;

    err = TXW51_SERV_MEASURE_Init(serviceHandle, &measureInit);
    if (err != ERR_NONE) {
        return ERR_BLE_SERVICE_INIT_FAILED;
    }

    measurementServiceHandle = serviceHandle;
    return ERR_NONE;
}


/***************************************************************************//**
 * @brief Handles the BLE events from the Measurement service.
 *
 * This function gets called from the BLE service as a callback.
 *
 * @param[in] handle The handle of the service.
 * @param[in] evt    The information to the event.
 *
 * @return Nothing.
 ******************************************************************************/
static void MEASUREMENT_BleEventHandler(struct TXW51_SERV_MEASURE_Handle *handle,
                                        struct TXW51_SERV_MEASURE_Event *evt)
{
    static bool isStarted = false;  /* Flag to remember if measurement has been started. */

    switch (evt->EventType) {
        case TXW51_SERV_MEASURE_EVT_START:
            if (isStarted) {
                TXW51_LOG_INFO("[Measure Service] Measurement already started!");
                return;
            }

            isStarted = true;
            sequenceNumber = 0;
            TXW51_LOG_INFO("[Measure Service] Start measurement");
            APPL_SENSOR_StartToMeasure();
            break;

        case TXW51_SERV_MEASURE_EVT_STOP:
            APPL_SENSOR_StopToMeasure();
            isStarted = false;
            TXW51_LOG_INFO("[Measure Service] Stop measurement");
            break;

        case TXW51_SERV_MEASURE_EVT_SET_DURATION:
            TXW51_LOG_INFO("[Measure Service] Set measurement duration");
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


void APPL_MEASUREMENT_SendAllData(enum TXW51_SERV_MEASURE_TxType txType)
{
    if (((txType == TXW51_SERV_MEASURE_TX_INDICATION)   && isIndicationBusy) ||
        ((txType == TXW51_SERV_MEASURE_TX_NOTIFICATION) && (notificationPacketCount == 0))) {
        return;
    }

    uint32_t bytesRead;
    struct TXW51_SERV_MEASURE_DataPacket packet;

    if (gIsNewAccDataAvailable) {
        bytesRead = APPL_FIFO_Get(APPL_FIFO_BUFFER_ACC, packet.Data, 3 * 6);
        if (bytesRead == 0) {
            gIsNewAccDataAvailable = false;
            return;
        }
        packet.Header.AccOrGyro = TXW51_SERV_MEASURE_DATA_SENSOR_ACC;
    }

    if (gIsNewGyroDataAvailable) {
        bytesRead = APPL_FIFO_Get(APPL_FIFO_BUFFER_GYRO, packet.Data, 3 * 6);
        if (bytesRead == 0) {
            gIsNewGyroDataAvailable = false;
            return;
        }
        packet.Header.AccOrGyro = TXW51_SERV_MEASURE_DATA_SENSOR_GYRO;
    }

    packet.Header.Axis = (TXW51_SERV_MEASURE_DATA_AXIS_X |
                          TXW51_SERV_MEASURE_DATA_AXIS_Y |
                          TXW51_SERV_MEASURE_DATA_AXIS_Z);
    packet.Header.NumberOfSamples = bytesRead / 6;
    packet.Number = sequenceNumber++;

    if (txType == TXW51_SERV_MEASURE_TX_INDICATION) {
        isIndicationBusy = true;
    } else {
        notificationPacketCount--;
    }
    TXW51_SERV_MEASURE_SendData(txType,
                                measurementServiceHandle,
                                &packet);
}

