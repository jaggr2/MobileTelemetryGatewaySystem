/***************************************************************************//**
 * @brief   Module that handles the Bluetooth Smart LSM330 service.
 *
 * It initializes and communicates with the Bluetooth Smart service.
 * Additionally, it initializes and sets the LSM330 sensor over SPI.
 *
 * @file    sensor.c
 * @version 1.0
 * @date    05.12.2014
 * @author  Daniel Meer
 *
 * @remark  Last Modifications:
 *          05.12.2014 meerd1 created
 ******************************************************************************/

/*----- Header-Files ---------------------------------------------------------*/
#include "sensor.h"

#include <string.h>
#include <stdio.h>

#include "txw51_framework/hw/lsm330.h"
#include "txw51_framework/utils/log.h"

#include "app/appl.h"
#include "app/error.h"
#include "app/fifo.h"

/*----- Macros ---------------------------------------------------------------*/

/*----- Data types -----------------------------------------------------------*/

/*----- Function prototypes --------------------------------------------------*/
static void SENSOR_StartAcc(void);
static void SENSOR_StopAcc(void);
static void SENSOR_StartGyro(void);
static void SENSOR_StopGyro(void);
static void SENSOR_ACC_ReadData(void *data, uint16_t size);
static void SENSOR_GYRO_ReadData(void *data, uint16_t size);
static void SENSOR_ACC_DebugInterrupt(void *data, uint16_t size);
static void SENSOR_GYRO_DebugInterrupt(void *data, uint16_t size);
static void SENSOR_BleEventHandler(struct TXW51_SERV_LSM330_Handle *handle,
                                   struct TXW51_SERV_LSM330_Event *evt);
static void SENSOR_EnableAcc(uint8_t enable);
static void SENSOR_EnableGyro(uint8_t enable);
static void SENSOR_GetTemperature(uint8_t *value);
static void SENSOR_SetFullscaleAcc(uint8_t value);
static void SENSOR_SetFullscaleGyro(uint8_t value);
static void SENSOR_SetOdrAcc(uint8_t value);
static void SENSOR_SetOdrGyro(uint8_t value);

/*----- Data -----------------------------------------------------------------*/
static bool isAccEnabled = false;       /**< Flag to indicate if the accelerometer has been enabled. */
static bool isGyroEnabled = false;      /**< Flag to indicate if the gyroscope has been enabled. */

/*----- Implementation -------------------------------------------------------*/

void APPL_SENSOR_Init(void)
{
    TXW51_LSM330_Init();
    TXW51_LSM330_EnableGyro(false);

    struct TXW51_LSM330_ACC_FifoInit accFifoConfig = {
        .FifoEnable      = true,
        .Mode            = TXW51_LSM330_ACC_FIFO_MODE_BYPASS,
        .Watermark       = 0,
        .WatermarkEnable = false
    };
    TXW51_LSM330_ACC_ConfigFifo(&accFifoConfig);

    struct TXW51_LSM330_GYRO_FifoInit gyroFifoConfig = {
        .FifoEnable      = true,
        .Mode            = TXW51_LSM330_GYRO_FIFO_MODE_BYPASS,
        .Watermark       = 0,
        .WatermarkEnable = false
    };
    TXW51_LSM330_GYRO_ConfigFifo(&gyroFifoConfig);

    struct TXW51_LSM330_Axis accAxisConfig = {
        .X_Enable = true,
        .Y_Enable = true,
        .Z_Enable = true
    };
    TXW51_LSM330_ACC_SetActiveAxis(&accAxisConfig);

    struct TXW51_LSM330_Axis gyroAxisConfig = {
        .X_Enable = true,
        .Y_Enable = true,
        .Z_Enable = true
    };
    TXW51_LSM330_GYRO_SetActiveAxis(&gyroAxisConfig);

    TXW51_LSM330_ACC_SetFullscale(TXW51_LSM330_ACC_FSCALE_2G);
    TXW51_LSM330_GYRO_SetFullscale(TXW51_LSM330_GYRO_FSCALE_250DPS);

    TXW51_LSM330_ACC_SetOdr(TXW51_LSM330_ACC_ODR_OFF);
    TXW51_LSM330_GYRO_SetOdr(TXW51_LSM330_GYRO_ODR_95);

    struct TXW51_LSM330_ACC_Interrupts accInterruptConfig = {
        .Int1A_Enable    = true,
        .Int1A_DataReady = false,
        .Int1A_Empty     = false,
        .Int1A_Watermark = true,
        .Int1A_Overrun   = false,
        .Int2A_Enable    = true
    };
    TXW51_LSM330_ACC_ConfigInterrupts(&accInterruptConfig);

    struct TXW51_LSM330_GYRO_Interrupts gyroInterruptConfig = {
        .Int1G_Enable    = false,
        .Int2G_Enable    = true,
        .Int2G_DataReady = false,
        .Int2G_Empty     = false,
        .Int2G_Watermark = true,
        .Int2G_Overrun   = false,
    };
    TXW51_LSM330_GYRO_ConfigInterrupts(&gyroInterruptConfig);
}


void APPL_SENSOR_StartToMeasure(void)
{
    if (isAccEnabled) {
        SENSOR_StartAcc();
    }
    if (isGyroEnabled) {
        SENSOR_StartGyro();
    }
}


void APPL_SENSOR_StopToMeasure(void)
{
    SENSOR_StopAcc();
    SENSOR_StopGyro();
}


/***************************************************************************//**
 * @brief Puts the accelerometer into measurement mode.
 *
 * This sets the FIFO of the accelerometer on the LSM330 to stream mode.
 *
 * @return Nothing.
 ******************************************************************************/
static void SENSOR_StartAcc(void)
{
    struct TXW51_LSM330_ACC_FifoInit fifoConfig = {
        .FifoEnable      = true,
        .Mode            = TXW51_LSM330_ACC_FIFO_MODE_STREAM,
        .Watermark       = APPL_SENSOR_VALUES_PER_FIFO_BLOCK,
        .WatermarkEnable = true
    };
    TXW51_LSM330_ACC_ConfigFifo(&fifoConfig);
}


/***************************************************************************//**
 * @brief Puts the accelerometer into non-measurement mode.
 *
 * This sets the FIFO of the accelerometer on the LSM330 to bypass mode.
 *
 * @return Nothing.
 ******************************************************************************/
static void SENSOR_StopAcc(void)
{
    struct TXW51_LSM330_ACC_FifoInit fifoConfig = {
        .FifoEnable      = true,
        .Mode            = TXW51_LSM330_ACC_FIFO_MODE_BYPASS,
        .Watermark       = 0,
        .WatermarkEnable = false
    };
    TXW51_LSM330_ACC_ConfigFifo(&fifoConfig);
}


/***************************************************************************//**
 * @brief Puts the gyroscope into measurement mode.
 *
 * This sets the FIFO of the gyroscope on the LSM330 to stream mode.
 *
 * @return Nothing.
 ******************************************************************************/
static void SENSOR_StartGyro(void)
{
    struct TXW51_LSM330_GYRO_FifoInit gyroFifoConfig = {
        .FifoEnable      = true,
        .Mode            = TXW51_LSM330_GYRO_FIFO_MODE_STREAM,
        .Watermark       = APPL_SENSOR_VALUES_PER_FIFO_BLOCK,
        .WatermarkEnable = true
    };
    TXW51_LSM330_GYRO_ConfigFifo(&gyroFifoConfig);
}


/***************************************************************************//**
 * @brief Puts the gyroscope into non-measurement mode.
 *
 * This sets the FIFO of the gyroscope on the LSM330 to bypass mode.
 *
 * @return Nothing.
 ******************************************************************************/
static void SENSOR_StopGyro(void)
{
    struct TXW51_LSM330_GYRO_FifoInit gyroFifoConfig = {
        .FifoEnable      = true,
        .Mode            = TXW51_LSM330_GYRO_FIFO_MODE_BYPASS,
        .Watermark       = 0,
        .WatermarkEnable = false
    };
    TXW51_LSM330_GYRO_ConfigFifo(&gyroFifoConfig);
}


void APPL_SENSOR_SetupMotionWakeup(void)
{
    TXW51_LSM330_ACC_SetOdr(TXW51_LSM330_ACC_ODR_3_125);
    TXW51_LSM330_EnableGyro(false);
    TXW51_LSM330_SetMotionWakeup();
}


void APPL_SENSOR_HandleInterrupt(int32_t channel)
{
    switch (channel) {
        case TXW51_LSM330_GPIO_INT1_ACC_CHANNEL:
            app_sched_event_put(NULL, 0, SENSOR_ACC_ReadData);
            break;

        case TXW51_LSM330_GPIO_INT2_GYRO_CHANNEL:
            app_sched_event_put(NULL, 0, SENSOR_GYRO_ReadData);
            break;
    }
}


/***************************************************************************//**
 * @brief Reads all values from the LSM330 accelerometer and puts them into the
 *        FIFO buffer.
 *
 * Reads all values of all axes from the FIFO on the LSM330 sensor until the
 * configured watermark. It then saves the values into the application FIFO.
 *
 * This function should be called when the sensor generated a watermark
 * interrupt.
 *
 * @param[in] data Not used.
 * @param[in] size Not used.
 *
 * @return Nothing.
 ******************************************************************************/
static void SENSOR_ACC_ReadData(void *data, uint16_t size)
{
    uint8_t buffer[APPL_SENSOR_VALUES_PER_FIFO_BLOCK * 6];

    TXW51_LSM330_ACC_GetDataBlock(buffer, APPL_SENSOR_VALUES_PER_FIFO_BLOCK);
    APPL_FIFO_Put(APPL_FIFO_BUFFER_ACC, buffer,
                  APPL_SENSOR_VALUES_PER_FIFO_BLOCK * 6);
    gIsNewAccDataAvailable = true;
}


/***************************************************************************//**
 * @brief Reads all values from the LSM330 gyroscope and puts them into the
 *        FIFO buffer.
 *
 * Reads all values of all axes from the FIFO on the LSM330 sensor until the
 * configured watermark. It then saves the values into the application FIFO.
 *
 * This function should be called when the sensor generated a watermark
 * interrupt.
 *
 * @param[in] data Not used.
 * @param[in] size Not used.
 *
 * @return Nothing.
 ******************************************************************************/
static void SENSOR_GYRO_ReadData(void *data, uint16_t size)
{
    uint8_t buffer[APPL_SENSOR_VALUES_PER_FIFO_BLOCK * 6];

    TXW51_LSM330_GYRO_GetDataBlock(buffer, APPL_SENSOR_VALUES_PER_FIFO_BLOCK);
    APPL_FIFO_Put(APPL_FIFO_BUFFER_GYRO, buffer,
                  APPL_SENSOR_VALUES_PER_FIFO_BLOCK * 6);
    gIsNewGyroDataAvailable = true;
}


/***************************************************************************//**
 * @brief Reads all values from the LSM330 accelerometer and writes them to
 *        the log.
 *
 * Use function only for debugging purposes!
 *
 * Reads all values of all axes from the FIFO on the LSM330 sensor until the
 * configured watermark. It then prints the values to the log.
 *
 * This function should be called when the sensor generated a watermark
 * interrupt.
 *
 * @param[in] data Not used.
 * @param[in] size Not used.
 *
 * @return Nothing.
 ******************************************************************************/
static void SENSOR_ACC_DebugInterrupt(void *data, uint16_t size)
{
    char outputString[80];
    union TXW51_LSM330_FIFO_SRC_REG_A value;

    /* Print FIFO status at the beginning. */
    TXW51_LSM330_ACC_GetFifoStatus(&value);
    sprintf(outputString, "FIFO_SRC_REG_A: 0x%02X, WTM: %d, OVRN: %d, EMPTY: %d, FSS: %d",
            value.Byte, value.Bit.WTM, value.Bit.OVRN_FIFO, value.Bit.EMPTY, value.Bit.FSS);
    TXW51_LOG_INFO(outputString);

    /* Print FIFO values. */
    uint8_t buffer[500];
    int8_t n = APPL_SENSOR_VALUES_PER_FIFO_BLOCK;
    int8_t k = 6;
    uint16_t temp[k];

    TXW51_LSM330_ACC_GetDataBlock(buffer, n);
    for (int32_t i = 0; i < n; i++) {
        temp[0] = (buffer[1+i*k] << 8) + buffer[0+i*k];
        temp[1] = (buffer[3+i*k] << 8) + buffer[2+i*k];
        temp[2] = (buffer[5+i*k] << 8) + buffer[4+i*k];
        sprintf(outputString, "X: %5d  Y: %5d  Z:%5d", temp[0], temp[1], temp[2]);
        TXW51_LOG_INFO(outputString);
    }

    /* Print FIFO status at the end. */
    TXW51_LSM330_ACC_GetFifoStatus(&value);
    sprintf(outputString, "FIFO_SRC_REG_A: 0x%02X, WTM: %d, OVRN: %d, EMPTY: %d, FSS: %d",
            value.Byte, value.Bit.WTM, value.Bit.OVRN_FIFO, value.Bit.EMPTY, value.Bit.FSS);
    TXW51_LOG_INFO(outputString);
}


/***************************************************************************//**
 * @brief Reads all values from the LSM330 gyroscope and writes them to
 *        the log.
 *
 * Use function only for debugging purposes!
 *
 * Reads all values of all axes from the FIFO on the LSM330 sensor until the
 * configured watermark. It then prints the values to the log. It does this
 * only every 10th time, because the sampling rate is too high for the log.
 *
 * This function should be called when the sensor generated a watermark
 * interrupt.
 *
 * @param[in] data Not used.
 * @param[in] size Not used.
 *
 * @return Nothing.
 ******************************************************************************/
static void SENSOR_GYRO_DebugInterrupt(void *data, uint16_t size)
{
    static int skipIndex = 0;
    char outputString[80];
    union TXW51_LSM330_FIFO_SRC_REG_G value;

    /* Only print every 10th time because the sampling rate is too fast. */
    if (skipIndex >= 10) {
        skipIndex = 0;

        /* Print FIFO status at the beginning. */
        TXW51_LSM330_GYRO_GetFifoStatus(&value);
        sprintf(outputString, "FIFO_SRC_REG_G: 0x%2X, WTM: %d, OVRN: %d, EMPTY: %d, FSS: %d",
                value.Byte, value.Bit.WTM, value.Bit.OVRN, value.Bit.EMPTY, value.Bit.FSS);
        TXW51_LOG_INFO(outputString);

        /* Print FIFO values. */
        uint8_t buffer[500];
        int8_t n = APPL_SENSOR_VALUES_PER_FIFO_BLOCK;
        int8_t k = 6;
        uint16_t temp[k];

        TXW51_LSM330_GYRO_GetDataBlock(buffer, n);
        for (int32_t i = 0; i < n; i++) {
            temp[0] = (buffer[1+i*k] << 8) + buffer[0+i*k];
            temp[1] = (buffer[3+i*k] << 8) + buffer[2+i*k];
            temp[2] = (buffer[5+i*k] << 8) + buffer[4+i*k];
            sprintf(outputString, "X: %5d  Y: %5d  Z:%5d", temp[0], temp[1], temp[2]);
            TXW51_LOG_INFO(outputString);
        }

        /* Print FIFO status at the end. */
        TXW51_LSM330_GYRO_GetFifoStatus(&value);
        sprintf(outputString, "FIFO_SRC_REG_G: 0x%2X, WTM: %d, OVRN: %d, EMPTY: %d, FSS: %d",
                value.Byte, value.Bit.WTM, value.Bit.OVRN, value.Bit.EMPTY, value.Bit.FSS);
        TXW51_LOG_INFO(outputString);
    }
    skipIndex++;
}


/* -------------------------------------------------------------------------- */


uint32_t APPL_SENSOR_InitService(struct TXW51_SERV_LSM330_Handle *serviceHandle)
{
    uint32_t err = ERR_NONE;

    struct TXW51_SERV_LSM330_Init init;
    init.EventHandler = SENSOR_BleEventHandler;

    err = TXW51_SERV_LSM330_Init(serviceHandle, &init);
    if (err != ERR_NONE) {
        return ERR_BLE_SERVICE_INIT_FAILED;
    }
    return ERR_NONE;
}


/***************************************************************************//**
 * @brief Handles the BLE events from the LSM330 service.
 *
 * This function gets called from the BLE service as a callback.
 *
 * @param[in] handle The handle of the service.
 * @param[in] evt    The information to the event.
 *
 * @return Nothing.
 ******************************************************************************/
static void SENSOR_BleEventHandler(struct TXW51_SERV_LSM330_Handle *handle,
                                   struct TXW51_SERV_LSM330_Event *evt)
{
    switch (evt->EventType) {
        case TXW51_SERV_LSM330_EVT_ACC_EN:
            SENSOR_EnableAcc(*evt->Value);
            break;
        case TXW51_SERV_LSM330_EVT_GYRO_EN:
            SENSOR_EnableGyro(*evt->Value);
            break;
        case TXW51_SERV_LSM330_EVT_TEMP_SAMPLE:
            SENSOR_GetTemperature(evt->Value);
            break;
        case TXW51_SERV_LSM330_EVT_ACC_FSCALE:
            SENSOR_SetFullscaleAcc(*evt->Value);
            break;
        case TXW51_SERV_LSM330_EVT_GYRO_FSCALE:
            SENSOR_SetFullscaleGyro(*evt->Value);
            break;
        case TXW51_SERV_LSM330_EVT_ACC_ODR:
            SENSOR_SetOdrAcc(*evt->Value);
            break;
        case TXW51_SERV_LSM330_EVT_GYRO_ODR:
            SENSOR_SetOdrGyro(*evt->Value);
            break;
        case TXW51_SERV_LSM330_EVT_TRIGGER_VAL:
            TXW51_LOG_INFO("[LSM330 Sensor] Trigger value not yet implemented.");
            break;
        case TXW51_SERV_LSM330_EVT_TRIGGER_AXIS:
            TXW51_LOG_INFO("[LSM330 Sensor] Trigger axis not yet implemented.");
            break;
        default:
            break;
    }
}


/***************************************************************************//**
 * @brief Enables or disables the accelerometer.
 *
 * This function gets called when the peer device enables the accelerometer.
 *
 * @param[in] enable If not 0 -> enable the sensor; if 0 -> disable the sensor.
 *
 * @return Nothing.
 ******************************************************************************/
static void SENSOR_EnableAcc(uint8_t enable)
{
    if (enable) {
        isAccEnabled = true;
        TXW51_LOG_DEBUG("[LSM330 Sensor] Accelerometer enabled.");
    } else {
        isAccEnabled = false;
        TXW51_LOG_DEBUG("[LSM330 Sensor] Accelerometer disabled.");
    }
}


/***************************************************************************//**
 * @brief Enables or disables the gyroscope.
 *
 * This function gets called when the peer device enables the gyroscope.
 *
 * @param[in] enable If not 0 -> enable the sensor; if 0 -> disable the sensor.
 *
 * @return Nothing.
 ******************************************************************************/
static void SENSOR_EnableGyro(uint8_t enable)
{
    if (enable) {
        TXW51_LSM330_EnableGyro(true);
        isGyroEnabled = true;
        TXW51_LOG_DEBUG("[LSM330 Sensor] Gyro enabled.");
    } else {
        TXW51_LSM330_EnableGyro(false);
        isGyroEnabled = false;
        TXW51_LOG_DEBUG("[LSM330 Sensor] Gyro disabled.");
    }
}


/***************************************************************************//**
 * @brief Reads the temperature sensor.
 *
 * @param[out] value The temperature value.
 *
 * @return Nothing.
 ******************************************************************************/
static void SENSOR_GetTemperature(uint8_t *value)
{
    TXW51_LSM330_GetTemperature(value);

    /* Convert sensor value to real temperature. */
    *value = TXW51_LSM330_TEMP_REF - *value;

    TXW51_LOG_DEBUG("[LSM330 Sensor] Temperature read.");
}


/***************************************************************************//**
 * @brief Sets the full-scale of the accelerometer.
 *
 * @param[in] value The new full-scale value.
 *
 * @return Nothing.
 ******************************************************************************/
static void SENSOR_SetFullscaleAcc(uint8_t value)
{
    if (value > TXW51_LSM330_ACC_FSCALE_16G) {
        TXW51_LOG_WARNING("[LSM330 Sensor] Acc: Could not set full-scale. Wrong value.");
        return;
    }

    TXW51_LSM330_ACC_SetFullscale(value);
    TXW51_LOG_DEBUG("[LSM330 Sensor] Acc: Full-scale set.");
}


/***************************************************************************//**
 * @brief Sets the full-scale of the gyroscope.
 *
 * @param[in] value The new full-scale value.
 *
 * @return Nothing.
 ******************************************************************************/
static void SENSOR_SetFullscaleGyro(uint8_t value)
{
    if (value > TXW51_LSM330_GYRO_FSCALE_2000DPS) {
        TXW51_LOG_WARNING("[LSM330 Sensor] Gyro: Could not set full-scale. Wrong value.");
        return;
    }

    TXW51_LSM330_GYRO_SetFullscale(value);
    TXW51_LOG_DEBUG("[LSM330 Sensor] Gyro: Full-scale set.");
}


/***************************************************************************//**
 * @brief Sets the ODR of the accelerometer.
 *
 * @param[in] value The new ODR value.
 *
 * @return Nothing.
 ******************************************************************************/
static void SENSOR_SetOdrAcc(uint8_t value)
{
    if (value > TXW51_LSM330_ACC_ODR_1600) {
        TXW51_LOG_WARNING("[LSM330 Sensor] Acc: Could not set ODR. Wrong value.");
        return;
    }

    TXW51_LSM330_ACC_SetOdr(value);
    TXW51_LOG_DEBUG("[LSM330 Sensor] Acc: ODR set.");
}


/***************************************************************************//**
 * @brief Sets the ODR of the gyroscope.
 *
 * @param[in] value The new ODR value.
 *
 * @return Nothing.
 ******************************************************************************/
static void SENSOR_SetOdrGyro(uint8_t value)
{
    if (value > TXW51_LSM330_GYRO_ODR_760) {
        TXW51_LOG_WARNING("[LSM330 Sensor] Gyro: Could not set ODR. Wrong value.");
        return;
    }

    TXW51_LSM330_GYRO_SetOdr(value);
    TXW51_LOG_DEBUG("[LSM330 Sensor] Gyro: ODR set.");
}

