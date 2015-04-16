/***************************************************************************//**
 * @brief   Driver to the LSM330 sensor.
 *
 * The LSM330 contains of an accelerometer, a gyroscope and a temperature
 * sensor. The communication on the TXW51 is over SPI. Multiple interrupt lines
 * are connected with the LSM330 as well.
 *
 * @file    lsm330.c
 * @version 1.0
 * @date    26.11.2014
 * @author  Daniel Meer
 *
 * @remark  Last Modifications:
 *          26.11.2014 meerd1 created
 ******************************************************************************/

/*----- Header-Files ---------------------------------------------------------*/
#include "lsm330.h"

#include <string.h>

#include "txw51_framework/hw/gpio.h"
#include "txw51_framework/hw/spi.h"
#include "txw51_framework/utils/log.h"
#include "txw51_framework/utils/txw51_errors.h"

/*----- Macros ---------------------------------------------------------------*/

/*----- Data types -----------------------------------------------------------*/
/**
 * @brief Enumeration to distinguish between the accelerometer and the
 *        gyroscope.
 */
enum LSM330_Sensor {
    TXW51_LSM330_ACC,   /**< The accelerometer. */
    TXW51_LSM330_GYRO   /**< The gyroscope. */
};

/*----- Function prototypes --------------------------------------------------*/
static uint32_t LSM330_CheckConnection(void);
static uint32_t LSM330_ReadSpi(enum LSM330_Sensor sensor,
                               uint8_t addr,
                               uint8_t *value);
static uint32_t LSM330_ReadMultiSpi(enum LSM330_Sensor sensor,
                                    uint8_t addr,
                                    uint8_t *values,
                                    uint32_t n);
static uint32_t LSM330_WriteSpi(enum LSM330_Sensor sensor,
                                uint8_t addr,
                                uint8_t value);
static uint32_t LSM330_UpdateRegister(enum LSM330_Sensor sensor,
                                      uint8_t addr,
                                      uint8_t mask,
                                      uint8_t newBits);
static uint32_t LSM330_ACC_ResetFifo(void);
static uint32_t LSM330_GYRO_ResetFifo(void);

/*----- Data -----------------------------------------------------------------*/

/*----- Implementation -------------------------------------------------------*/

uint32_t TXW51_LSM330_Init(void)
{
    uint32_t err;

    TXW51_GPIO_ConfigGpioAsOutput(TXW51_LSM330_GPIO_SS_ACC);
    TXW51_GPIO_ConfigGpioAsOutput(TXW51_LSM330_GPIO_SS_GYRO);
    TXW51_GPIO_SetGpio(TXW51_LSM330_GPIO_SS_ACC);
    TXW51_GPIO_SetGpio(TXW51_LSM330_GPIO_SS_GYRO);

    err = LSM330_CheckConnection();
    if (err != ERR_NONE) {
        TXW51_LOG_ERROR("[LSM330] Initialization failed. Could not read over SPI.");
        return err;
    }

    err = TXW51_LSM330_ResetDevice();
    if (err != ERR_NONE) {
        return err;
    }

    TXW51_LOG_DEBUG("[LSM330] Initialization successful.");
    return ERR_NONE;
}


void TXW51_LSM330_Deinit(void)
{
    TXW51_GPIO_ConfigGpioAsDisconnected(TXW51_LSM330_GPIO_SS_ACC);
    TXW51_GPIO_ConfigGpioAsDisconnected(TXW51_LSM330_GPIO_SS_GYRO);
    TXW51_GPIO_ConfigGpioAsDisconnected(TXW51_LSM330_GPIO_INT1_ACC);
    TXW51_GPIO_ConfigGpioAsDisconnected(TXW51_LSM330_GPIO_INT2_ACC);
    TXW51_GPIO_ConfigGpioAsDisconnected(TXW51_LSM330_GPIO_INT1_GYRO);
    TXW51_GPIO_ConfigGpioAsDisconnected(TXW51_LSM330_GPIO_INT2_GYRO);

    TXW51_LOG_DEBUG("[LSM330] Deinitialization successful.");
}


/***************************************************************************//**
 * @brief Checks the SPI connection to the sensor.
 *
 * @return ERR_NONE if no error occurred.
 *         ERR_LSM330_READ_FAILED if reading from the sensor failed.
 ******************************************************************************/
static uint32_t LSM330_CheckConnection(void)
{
    uint32_t err;
    uint8_t value = 0;

    err = LSM330_ReadSpi(TXW51_LSM330_ACC, TXW51_LSM330_REG_WHO_AM_I_A, &value);
    if (err != ERR_NONE) {
        return err;
    } else if (value != TXW51_LSM330_REG_WHO_AM_I_A_DEFAULT) {
        return ERR_LSM330_READ_FAILED;
    }

    err = LSM330_ReadSpi(TXW51_LSM330_GYRO, TXW51_LSM330_REG_WHO_AM_I_G, &value);
    if (err != ERR_NONE) {
        return err;
    } else if (value != TXW51_LSM330_REG_WHO_AM_I_G_DEFAULT) {
        return ERR_LSM330_READ_FAILED;
    }

    return ERR_NONE;
}


/***************************************************************************//**
 * @brief Reads from the SPI interface.
 *
 * @param[in]  sensor Specifies from which sensor to read.
 * @param[in]  addr   The register address of the value.
 * @param[out] value  Buffer to save the value.
 *
 * @return ERR_NONE if no error occurred.
 *         ERR_LSM330_READ_FAILED if reading from the sensor failed.
 ******************************************************************************/
static uint32_t LSM330_ReadSpi(enum LSM330_Sensor sensor,
                               uint8_t addr,
                               uint8_t *value)
{
    return LSM330_ReadMultiSpi(sensor, addr, value, 1);
}


/***************************************************************************//**
 * @brief Reads multiple bytes from the SPI interface.
 *
 * The address of the first byte needs to be specified. Then we read multiple
 * bytes with incrementing address.
 *
 * @param[in]  sensor Specifies from which sensor to read.
 * @param[in]  addr   The register address of the first byte.
 * @param[out] values Buffer to save the values.
 * @param[in]  n      The number of bytes to read.
 *
 * @return ERR_NONE if no error occurred.
 *         ERR_LSM330_READ_FAILED if reading from the sensor failed.
 ******************************************************************************/
static uint32_t LSM330_ReadMultiSpi(enum LSM330_Sensor sensor,
                                    uint8_t addr,
                                    uint8_t *values,
                                    uint32_t n)
{
    uint32_t err;

    enum TXW51_GPIO_Pin gpio;
    switch (sensor) {
        case TXW51_LSM330_ACC:
            gpio = TXW51_LSM330_GPIO_SS_ACC;
            break;

        case TXW51_LSM330_GYRO:
            gpio = TXW51_LSM330_GPIO_SS_GYRO;
            addr |= TXW51_LSM330_FLAG_MULTI_RW;
            break;
    }

    addr |= TXW51_LSM330_FLAG_READ;

    TXW51_GPIO_ClearGpio(gpio);
    err = TXW51_SPI_Read(TXW51_SPI_0, addr, values, n);
    TXW51_GPIO_SetGpio(gpio);

    if (err != ERR_NONE) {
        return ERR_LSM330_READ_FAILED;
    }
    return ERR_NONE;
}


/***************************************************************************//**
 * @brief Writes to the SPI interface.
 *
 * @param[in] sensor Specifies to which sensor to write.
 * @param[in] addr   The register address of the value.
 * @param[in] value  The value to write.
 *
 * @return ERR_NONE if no error occurred.
 *         ERR_LSM330_WRITE_FAILED if writing to the sensor failed.
 ******************************************************************************/
static uint32_t LSM330_WriteSpi(enum LSM330_Sensor sensor,
                                uint8_t addr,
                                uint8_t value)
{
    uint32_t err;

    enum TXW51_GPIO_Pin gpio;
    switch (sensor) {
        case TXW51_LSM330_ACC:
            gpio = TXW51_LSM330_GPIO_SS_ACC;
            break;

        case TXW51_LSM330_GYRO:
            gpio = TXW51_LSM330_GPIO_SS_GYRO;
            break;
    }

    TXW51_GPIO_ClearGpio(gpio);
    err = TXW51_SPI_Write(TXW51_SPI_0, addr, value);
    TXW51_GPIO_SetGpio(gpio);

    if (err != ERR_NONE) {
        return ERR_LSM330_WRITE_FAILED;
    }
    return ERR_NONE;
}


void TXW51_LSM330_Debug_StreamValues(void)
{
    uint8_t buffer[12];
    uint16_t temp[12];
    char outputString[100];
    uint8_t temperature;

    for (;;) {
        for (int i = 0; i < 100000; i++) {}

        LSM330_ReadMultiSpi(TXW51_LSM330_ACC, TXW51_LSM330_REG_OUT_X_L_A, buffer, 6);
        LSM330_ReadMultiSpi(TXW51_LSM330_GYRO, TXW51_LSM330_REG_OUT_X_L_G, buffer+6, 6);
        TXW51_LSM330_GetTemperature(&temperature);

        temp[0] = (buffer[1] << 8) + buffer[0];
        temp[1] = (buffer[3] << 8) + buffer[2];
        temp[2] = (buffer[5] << 8) + buffer[4];
        temp[3] = (buffer[7] << 8) + buffer[6];
        temp[4] = (buffer[9] << 8) + buffer[8];
        temp[5] = (buffer[11] << 8) + buffer[10];
        sprintf(outputString, "X: %5d  Y: %5d  Z:%5d  X: %5d  Y: %5d  Z:%5d  Temp:%d",
                temp[0], temp[1], temp[2], temp[3], temp[4], temp[5], temperature);
        TXW51_LOG_DEBUG(outputString);

        for (int i = 0; i < 100000; i++) {}
    }
}


void TXW51_LSM330_ACC_Debug_ShowRegister(uint32_t reg)
{
    uint8_t buffer;
    char outputString[50];

    LSM330_ReadSpi(TXW51_LSM330_ACC, reg, &buffer);
    sprintf(outputString, "0x%02X", buffer);
    TXW51_LOG_DEBUG(outputString);
}


uint32_t TXW51_LSM330_ACC_ConfigInterrupts(struct TXW51_LSM330_ACC_Interrupts *config)
{
    uint32_t err;

    if (config->Int1A_Enable) {
        TXW51_GPIO_ConfigGpioAsInput(TXW51_LSM330_GPIO_INT1_ACC,
                                     TXW51_LSM330_GPIO_INT1_ACC_PULL_CONF);
        TXW51_GPIOTE_SetInterrupt(TXW51_LSM330_GPIO_INT1_ACC_CHANNEL,
                                  TXW51_LSM330_GPIO_INT1_ACC,
                                  TXW51_LSM330_GPIO_INT1_ACC_POL_CONF);
    } else {
        TXW51_GPIO_ConfigGpioAsDisconnected(TXW51_LSM330_GPIO_INT1_ACC);
        /* TODO: TXW51_GPIOTE_ClearInterrupt (not yet implemented). */
    }

    if (config->Int2A_Enable) {
        TXW51_GPIO_SetSenseInterrupt(TXW51_LSM330_GPIO_INT2_ACC,
                                     TXW51_LSM330_GPIO_INT2_ACC_PULL_CONF);
        /* TODO: Let user chose between GPIOTE interrupt and sense interrupt. */
        /*hw_cfg_gpio_input(TXW51_LSM330_GPIO_INT2_ACC,
                          TXW51_LSM330_GPIO_INT2_ACC_PULL_CONF);
        TXW51_GPIOTE_SetInterrupt(TXW51_LSM330_GPIO_INT2_ACC_CHANNEL,
                                  TXW51_LSM330_GPIO_INT2_ACC,
                                  TXW51_LSM330_GPIO_INT2_ACC_POL_CONF);*/
    } else {
        TXW51_GPIO_ConfigGpioAsDisconnected(TXW51_LSM330_GPIO_INT2_ACC);
        /* TODO: TXW51_GPIOTE_ClearInterrupt (not yet implemented). */
    }

    uint8_t mask = TXW51_LSM330_REG_CTRL_REG4_A_INT1_EN |
                   TXW51_LSM330_REG_CTRL_REG4_A_INT2_EN |
                   TXW51_LSM330_REG_CTRL_REG4_A_IEL |
                   TXW51_LSM330_REG_CTRL_REG4_A_IEA |
                   TXW51_LSM330_REG_CTRL_REG4_A_DR_EN;

    uint8_t bits = TWX51_LSM330_CONFIG_INTERRUPT_POL |
                   TWX51_LSM330_CONFIG_INTERRUPT_LATCH;
    if (config->Int1A_Enable)    { bits |= TXW51_LSM330_REG_CTRL_REG4_A_INT1_EN; }
    if (config->Int1A_DataReady) { bits |= TXW51_LSM330_REG_CTRL_REG4_A_DR_EN;   }
    if (config->Int2A_Enable)    { bits |= TXW51_LSM330_REG_CTRL_REG4_A_INT2_EN; }

    err = LSM330_UpdateRegister(TXW51_LSM330_ACC,
                                TXW51_LSM330_REG_CTRL_REG4_A,
                                mask,
                                bits);
    if (err != ERR_NONE) {
        TXW51_LOG_ERROR("[LSM330] Acc: Could not configure interrupts.");
        return err;
    }

    mask = TXW51_LSM330_REG_CTRL_REG7_A_P1_EMPTY |
           TXW51_LSM330_REG_CTRL_REG7_A_P1_WTM |
           TXW51_LSM330_REG_CTRL_REG7_A_P1_OVERRUN;

    bits = 0;
    if (config->Int1A_Empty)     { bits |= TXW51_LSM330_REG_CTRL_REG7_A_P1_EMPTY;   }
    if (config->Int1A_Watermark) { bits |= TXW51_LSM330_REG_CTRL_REG7_A_P1_WTM;     }
    if (config->Int1A_Overrun)   { bits |= TXW51_LSM330_REG_CTRL_REG7_A_P1_OVERRUN; }

    err = LSM330_UpdateRegister(TXW51_LSM330_ACC,
                                TXW51_LSM330_REG_CTRL_REG7_A,
                                mask,
                                bits);
    if (err != ERR_NONE) {
        TXW51_LOG_ERROR("[LSM330] Acc: Could not configure interrupts.");
        return err;
    }

    TXW51_LOG_DEBUG("[LSM330] Acc: Interrupts configured.");
    return ERR_NONE;
}


uint32_t TXW51_LSM330_GYRO_ConfigInterrupts(struct TXW51_LSM330_GYRO_Interrupts *config)
{
    uint32_t err;

    if (config->Int1G_Enable) {
        TXW51_GPIO_ConfigGpioAsInput(TXW51_LSM330_GPIO_INT1_GYRO,
                                     TXW51_LSM330_GPIO_INT1_GYRO_PULL_CONF);
        TXW51_GPIOTE_SetInterrupt(TXW51_LSM330_GPIO_INT1_GYRO_CHANNEL,
                                  TXW51_LSM330_GPIO_INT1_GYRO,
                                  TXW51_LSM330_GPIO_INT1_GYRO_POL_CONF);
    } else {
        TXW51_GPIO_ConfigGpioAsDisconnected(TXW51_LSM330_GPIO_INT1_GYRO);
        /* TODO: TXW51_GPIOTE_ClearInterrupt (not yet implemented). */
    }

    if (config->Int2G_Enable) {
        TXW51_GPIO_ConfigGpioAsInput(TXW51_LSM330_GPIO_INT2_GYRO,
                                     TXW51_LSM330_GPIO_INT2_GYRO_PULL_CONF);
        TXW51_GPIOTE_SetInterrupt(TXW51_LSM330_GPIO_INT2_GYRO_CHANNEL,
                                  TXW51_LSM330_GPIO_INT2_GYRO,
                                  TXW51_LSM330_GPIO_INT2_GYRO_POL_CONF);
    } else {
        TXW51_GPIO_ConfigGpioAsDisconnected(TXW51_LSM330_GPIO_INT2_GYRO);
        /* TODO: TXW51_GPIOTE_ClearInterrupt (not yet implemented). */
    }

    uint8_t reg = 0;
    if (config->Int1G_Enable)    { reg |= TXW51_LSM330_REG_CTRL_REG3_G_I1_INT1;  }
    if (config->Int2G_Enable)    { /* Nothing to do. */                           }
    if (config->Int2G_DataReady) { reg |= TXW51_LSM330_REG_CTRL_REG3_G_I2_DRDY;  }
    if (config->Int2G_Empty)     { reg |= TXW51_LSM330_REG_CTRL_REG3_G_I2_EMPTY; }
    if (config->Int2G_Watermark) { reg |= TXW51_LSM330_REG_CTRL_REG3_G_I2_WTM;   }
    if (config->Int2G_Overrun)   { reg |= TXW51_LSM330_REG_CTRL_REG3_G_I2_ORUN;  }

    /* We need to write 0 first. Or else it does not work. */
    err = LSM330_WriteSpi(TXW51_LSM330_GYRO, TXW51_LSM330_REG_CTRL_REG3_G, 0x00);
    if (err != ERR_NONE) {
        TXW51_LOG_ERROR("[LSM330] Gyro: Could not configure interrupts.");
        return err;
    }

    err = LSM330_WriteSpi(TXW51_LSM330_GYRO, TXW51_LSM330_REG_CTRL_REG3_G, reg);
    if (err != ERR_NONE) {
        TXW51_LOG_ERROR("[LSM330] Gyro: Could not configure interrupts.");
        return err;
    }

    TXW51_LOG_DEBUG("[LSM330] Gyro: Interrupts configured.");
    return ERR_NONE;
}


uint32_t TXW51_LSM330_ACC_SetActiveAxis(struct TXW51_LSM330_Axis *axis)
{
    uint32_t err = ERR_NONE;

    uint8_t mask = TXW51_LSM330_REG_CTRL_REG5_A_XEN |
                   TXW51_LSM330_REG_CTRL_REG5_A_YEN |
                   TXW51_LSM330_REG_CTRL_REG5_A_ZEN;

    uint8_t bits = 0;
    if (axis->X_Enable) { bits |= TXW51_LSM330_REG_CTRL_REG5_A_XEN; }
    if (axis->Y_Enable) { bits |= TXW51_LSM330_REG_CTRL_REG5_A_YEN; }
    if (axis->Z_Enable) { bits |= TXW51_LSM330_REG_CTRL_REG5_A_ZEN; }

    err = LSM330_UpdateRegister(TXW51_LSM330_ACC,
                                TXW51_LSM330_REG_CTRL_REG5_A,
                                mask,
                                bits);
    if (err != ERR_NONE) {
        TXW51_LOG_WARNING("[LSM330] Acc: Could not set/clear axis.");
    } else {
        TXW51_LOG_DEBUG("[LSM330] Acc: Set/clear x-, y- and z-axis.");
    }

    return err;
}


uint32_t TXW51_LSM330_GYRO_SetActiveAxis(struct TXW51_LSM330_Axis *axis)
{
    uint32_t err = ERR_NONE;

    uint8_t mask = TXW51_LSM330_REG_CTRL_REG1_G_XEN |
                   TXW51_LSM330_REG_CTRL_REG1_G_YEN |
                   TXW51_LSM330_REG_CTRL_REG1_G_ZEN;
    uint8_t bits = 0;
    if (axis->X_Enable) { bits |= TXW51_LSM330_REG_CTRL_REG1_G_XEN; }
    if (axis->Y_Enable) { bits |= TXW51_LSM330_REG_CTRL_REG1_G_YEN; }
    if (axis->Z_Enable) { bits |= TXW51_LSM330_REG_CTRL_REG1_G_ZEN; }

    err = LSM330_UpdateRegister(TXW51_LSM330_GYRO,
                                TXW51_LSM330_REG_CTRL_REG1_G,
                                mask,
                                bits);
    if (err != ERR_NONE) {
        TXW51_LOG_WARNING("[LSM330] Gyro: Could not set/clear axis.");
    } else {
        TXW51_LOG_DEBUG("[LSM330] Gyro: Set/clear x-, y- and z-axis.");
    }

    return err;
}


/* TODO: Maybe use unions instead of shift operations. */
enum TXW51_LSM330_ACC_Fullscale TXW51_LSM330_ACC_GetFullscale(void)
{
    uint32_t err;
    uint8_t value = 0;

    err = LSM330_ReadSpi(TXW51_LSM330_ACC, TXW51_LSM330_REG_CTRL_REG6_A, &value);
    if (err != ERR_NONE) {
        return TXW51_LSM330_ACC_FSCALE_2G;
    }

    value &= TXW51_LSM330_REG_CTRL_REG6_A_FSCALE_Msk;
    value >>= TXW51_LSM330_REG_CTRL_REG6_A_FSCALE_Pos;
    return value;
}


/* TODO: Maybe use unions instead of shift operations. */
enum TXW51_LSM330_GYRO_Fullscale TXW51_LSM330_GYRO_GetFullscale(void)
{
    uint32_t err;
    uint8_t value = 0;

    err = LSM330_ReadSpi(TXW51_LSM330_GYRO, TXW51_LSM330_REG_CTRL_REG4_G, &value);
    if (err != ERR_NONE) {
        return TXW51_LSM330_GYRO_FSCALE_250DPS;
    }

    value &= TXW51_LSM330_REG_CTRL_REG4_G_FS_Msk;
    value >>= TXW51_LSM330_REG_CTRL_REG4_G_FS_Pos;
    return value;
}


/* TODO: Maybe use unions instead of shift operations. */
uint32_t TXW51_LSM330_ACC_SetFullscale(enum TXW51_LSM330_ACC_Fullscale fullscale)
{
    uint32_t err = ERR_NONE;

    err = LSM330_UpdateRegister(TXW51_LSM330_ACC,
                                TXW51_LSM330_REG_CTRL_REG6_A,
                                TXW51_LSM330_REG_CTRL_REG6_A_FSCALE_Msk,
                                (fullscale << TXW51_LSM330_REG_CTRL_REG6_A_FSCALE_Pos));
    if (err != ERR_NONE) {
        TXW51_LOG_WARNING("[LSM330] Acc: Could not set full scale.");
    } else {
        TXW51_LOG_DEBUG("[LSM330] Acc: Full scale set.");
    }

    return err;
}


/* TODO: Maybe use unions instead of shift operations. */
uint32_t TXW51_LSM330_GYRO_SetFullscale(enum TXW51_LSM330_GYRO_Fullscale fullscale)
{
    uint32_t err = ERR_NONE;

    err = LSM330_UpdateRegister(TXW51_LSM330_GYRO,
                                TXW51_LSM330_REG_CTRL_REG4_G,
                                TXW51_LSM330_REG_CTRL_REG4_G_FS_Msk,
                                (fullscale << TXW51_LSM330_REG_CTRL_REG4_G_FS_Pos));
    if (err != ERR_NONE) {
        TXW51_LOG_WARNING("[LSM330] Gyro: Could not set full scale.");
    } else {
        TXW51_LOG_DEBUG("[LSM330] Gyro: Full scale set.");
    }

    return err;
}


/* TODO: Maybe use unions instead of shift operations. */
enum TXW51_LSM330_ACC_Odr TXW51_LSM330_ACC_GetOdr(void)
{
    uint32_t err;
    uint8_t value = 0;

    err = LSM330_ReadSpi(TXW51_LSM330_ACC, TXW51_LSM330_REG_CTRL_REG5_A, &value);
    if (err != ERR_NONE) {
        return TXW51_LSM330_ACC_ODR_OFF;
    }

    value &= TXW51_LSM330_REG_CTRL_REG5_A_ODR_Msk;
    value >>= TXW51_LSM330_REG_CTRL_REG5_A_ODR_Pos;
    return value;
}


/* TODO: Maybe use unions instead of shift operations. */
enum TXW51_LSM330_GYRO_Odr TXW51_LSM330_GYRO_GetOdr(void)
{
    uint32_t err;
    uint8_t value = 0;

    err = LSM330_ReadSpi(TXW51_LSM330_GYRO, TXW51_LSM330_REG_CTRL_REG1_G, &value);
    if (err != ERR_NONE) {
        return TXW51_LSM330_GYRO_ODR_95;
    }

    value &= TXW51_LSM330_REG_CTRL_REG1_G_DR_Msk;
    value >>= TXW51_LSM330_REG_CTRL_REG1_G_DR_Pos;
    return value;
}


/* TODO: Maybe use unions instead of shift operations. */
uint32_t TXW51_LSM330_ACC_SetOdr(enum TXW51_LSM330_ACC_Odr odr)
{
    uint32_t err = ERR_NONE;

    err = LSM330_UpdateRegister(TXW51_LSM330_ACC,
                                TXW51_LSM330_REG_CTRL_REG5_A,
                                TXW51_LSM330_REG_CTRL_REG5_A_ODR_Msk,
                                (odr << TXW51_LSM330_REG_CTRL_REG5_A_ODR_Pos));
    if (err != ERR_NONE) {
        TXW51_LOG_WARNING("[LSM330] Acc: Could not set ODR.");
    } else {
        TXW51_LOG_DEBUG("[LSM330] Acc: ODR set.");
    }

    return err;
}


/* TODO: Maybe use unions instead of shift operations. */
uint32_t TXW51_LSM330_GYRO_SetOdr(enum TXW51_LSM330_GYRO_Odr odr)
{
    uint32_t err = ERR_NONE;

    err = LSM330_UpdateRegister(TXW51_LSM330_GYRO,
                                TXW51_LSM330_REG_CTRL_REG1_G,
                                TXW51_LSM330_REG_CTRL_REG1_G_DR_Msk,
                                (odr << TXW51_LSM330_REG_CTRL_REG1_G_DR_Pos));
    if (err != ERR_NONE) {
        TXW51_LOG_WARNING("[LSM330] Gyro: Could not set ODR.");
    } else {
        TXW51_LOG_DEBUG("[LSM330] Gyro: ODR set.");
    }

    return err;
}


uint32_t TXW51_LSM330_GetTemperature(uint8_t *value)
{
    uint32_t err;

    err = LSM330_ReadSpi(TXW51_LSM330_GYRO,
                         TXW51_LSM330_REG_OUT_TEMP_G,
                         value);
    if (err != ERR_NONE) {
        TXW51_LOG_WARNING("[LSM330] Could not read temperature.");
        return err;
    }

    return ERR_NONE;
}


uint32_t TXW51_LSM330_EnableGyro(bool enable)
{
    uint32_t err = ERR_NONE;

    uint8_t newBit = 0;
    if (enable) {
        newBit = TXW51_LSM330_REG_CTRL_REG1_G_PD;
    }

    err = LSM330_UpdateRegister(TXW51_LSM330_GYRO,
                                TXW51_LSM330_REG_CTRL_REG1_G,
                                TXW51_LSM330_REG_CTRL_REG1_G_PD,
                                newBit);
    if (err != ERR_NONE) {
        TXW51_LOG_ERROR("[LSM330] Could not enable/disable gyroscope.");
    } else {
        TXW51_LOG_DEBUG("[LSM330] Gyroscope enabled/disabled.");
    }

    return err;
}


/***************************************************************************//**
* @brief Updates the value of a register.
*
* This function simplifies the process when only some bits need to change. It
* reads the old value and changes only the specified bits. Then it writes the
* new value to the register.
*
* @param[in] sensor  Specifies from which sensor to read.
* @param[in] addr    The register address of the value.
* @param[in] mask    Specifies which bits need to change.
* @param[in] newBits Specifies the value of the bits that need to change.
*
* @return ERR_NONE if no error occurred.
*         ERR_LSM330_READ_FAILED if reading from the sensor failed.
*         ERR_LSM330_WRITE_FAILED if writing to the sensor failed.
******************************************************************************/
static uint32_t LSM330_UpdateRegister(enum LSM330_Sensor sensor,
                                      uint8_t addr,
                                      uint8_t mask,
                                      uint8_t newBits)
{
    uint32_t err;
    uint8_t value = 0;

    err = LSM330_ReadSpi(sensor, addr, &value);
    if (err != ERR_NONE) {
        return err;
    }

    value &= ~mask;
    value |= (newBits & mask);

    err = LSM330_WriteSpi(sensor, addr, value);
    if (err != ERR_NONE) {
        return err;
    }

    return ERR_NONE;
}


uint32_t TXW51_LSM330_ResetDevice(void)
{
    uint32_t err;
    uint8_t reg = TXW51_LSM330_REG_CTRL_REG4_A_STRT;

    err = LSM330_WriteSpi(TXW51_LSM330_ACC, TXW51_LSM330_REG_CTRL_REG4_A, reg);
    if (err != ERR_NONE) {
        TXW51_LOG_WARNING("[LSM330] Could not reset device. Writing register failed.");
        return err;
    }

    TXW51_LOG_DEBUG("[LSM330] Device reset.");
    return ERR_NONE;
}


uint32_t TXW51_LSM330_ACC_ConfigFifo(struct TXW51_LSM330_ACC_FifoInit *config)
{
    uint32_t err = ERR_NONE;

    if (config->Mode != TXW51_LSM330_ACC_FIFO_MODE_BYPASS) {
        err = LSM330_ACC_ResetFifo();
        if (err != ERR_NONE) {
            TXW51_LOG_WARNING("[LSM330] Acc: Could not reset FIFO.");
            return err;
        }
    }

    union TXW51_LSM330_FIFO_CTRL_REG_A reg = {
        .Bit.WTMP  = config->Watermark,
        .Bit.FMODE = config->Mode
    };

    err = LSM330_WriteSpi(TXW51_LSM330_ACC, TXW51_LSM330_REG_FIFO_CTRL_REG_A, reg.Byte);
    if (err != ERR_NONE) {
        TXW51_LOG_WARNING("[LSM330] Could not initialize FIFO.");
        return err;
    }

    uint8_t mask = TXW51_LSM330_REG_CTRL_REG7_A_FIFO_EN |
                   TXW51_LSM330_REG_CTRL_REG7_A_WTM_EN;
    uint8_t bits = 0;
    if (config->FifoEnable)      { bits |= TXW51_LSM330_REG_CTRL_REG7_A_FIFO_EN; }
    if (config->WatermarkEnable) { bits |= TXW51_LSM330_REG_CTRL_REG7_A_WTM_EN;  }

    err = LSM330_UpdateRegister(TXW51_LSM330_ACC,
                                TXW51_LSM330_REG_CTRL_REG7_A,
                                mask,
                                bits);
    if (err != ERR_NONE) {
        TXW51_LOG_WARNING("[LSM330] Could not initialize FIFO.");
    } else {
        TXW51_LOG_DEBUG("[LSM330] FIFO initialized.");
    }

    return err;
}


/***************************************************************************//**
 * @brief Resets the FIFO of the accelerometer.
 *
 * Before the FIFO mode can change, it has to be reset. This can be done by
 * setting it to bypass mode.
 *
 * @return ERR_NONE if no error occurred.
 *         ERR_LSM330_READ_FAILED if reading from the sensor failed.
 *         ERR_LSM330_WRITE_FAILED if writing to the sensor failed.
 ******************************************************************************/
static uint32_t LSM330_ACC_ResetFifo(void)
{
    uint32_t err = ERR_NONE;

    union TXW51_LSM330_FIFO_CTRL_REG_A reg = {
        .Bit.WTMP  = 0,
        .Bit.FMODE = TXW51_LSM330_ACC_FIFO_MODE_BYPASS
    };

    err = LSM330_WriteSpi(TXW51_LSM330_ACC,
                          TXW51_LSM330_REG_FIFO_CTRL_REG_A,
                          reg.Byte);
    if (err != ERR_NONE) {
        return err;
    }

    err = LSM330_UpdateRegister(TXW51_LSM330_ACC,
                                TXW51_LSM330_REG_CTRL_REG7_A,
                                TXW51_LSM330_REG_CTRL_REG7_A_FIFO_EN,
                                0xFF);
    if (err != ERR_NONE) {
        return err;
    }

    return err;
}


uint32_t TXW51_LSM330_GYRO_ConfigFifo(struct TXW51_LSM330_GYRO_FifoInit *config)
{
    uint32_t err = ERR_NONE;

    if (config->Mode != TXW51_LSM330_GYRO_FIFO_MODE_BYPASS) {
        err = LSM330_GYRO_ResetFifo();
        if (err != ERR_NONE) {
            TXW51_LOG_WARNING("[LSM330] Gyro: Could not reset FIFO.");
            return err;
        }
    }

    union TXW51_LSM330_FIFO_CTRL_REG_G reg = {
        .Bit.WTM = config->Watermark,
        .Bit.FM  = config->Mode
    };

    err = LSM330_WriteSpi(TXW51_LSM330_GYRO, TXW51_LSM330_REG_FIFO_CTRL_REG_G, reg.Byte);
    if (err != ERR_NONE) {
        TXW51_LOG_WARNING("[LSM330] Gyro: Could not initialize FIFO.");
        return err;
    }

    uint8_t bits = 0;
    if (config->FifoEnable) {
        bits |= TXW51_LSM330_REG_CTRL_REG5_G_FIFO_EN;
    }
    err = LSM330_UpdateRegister(TXW51_LSM330_GYRO,
                                TXW51_LSM330_REG_CTRL_REG5_G,
                                TXW51_LSM330_REG_CTRL_REG5_G_FIFO_EN,
                                bits);
    if (err != ERR_NONE) {
        TXW51_LOG_WARNING("[LSM330] Gyro: Could not initialize FIFO.");
    } else {
        TXW51_LOG_DEBUG("[LSM330] Gyro: FIFO initialized.");
    }

    return err;
}


/***************************************************************************//**
 * @brief Resets the FIFO of the gyroscope.
 *
 * Before the FIFO mode can change, it has to be reset. This can be done by
 * setting it to bypass mode.
 *
 * @return ERR_NONE if no error occurred.
 *         ERR_LSM330_READ_FAILED if reading from the sensor failed.
 *         ERR_LSM330_WRITE_FAILED if writing to the sensor failed.
 ******************************************************************************/
static uint32_t LSM330_GYRO_ResetFifo(void)
{
    uint32_t err = ERR_NONE;

    union TXW51_LSM330_FIFO_CTRL_REG_G reg = {
        .Bit.WTM = 0,
        .Bit.FM  = TXW51_LSM330_GYRO_FIFO_MODE_BYPASS
    };

    err = LSM330_WriteSpi(TXW51_LSM330_GYRO,
                          TXW51_LSM330_REG_FIFO_CTRL_REG_G,
                          reg.Byte);
    if (err != ERR_NONE) {
        return err;
    }

    err = LSM330_UpdateRegister(TXW51_LSM330_GYRO,
                                TXW51_LSM330_REG_CTRL_REG5_G,
                                TXW51_LSM330_REG_CTRL_REG5_G_FIFO_EN,
                                0xFF);
    if (err != ERR_NONE) {
        return err;
    }

    return err;
}


uint32_t TXW51_LSM330_ACC_GetDataBlock(uint8_t *buffer, uint32_t numberOfBlocks)
{
    uint32_t err;

    for (int32_t i = 0; i < numberOfBlocks; i++) {
        err = LSM330_ReadMultiSpi(TXW51_LSM330_ACC,
                                  TXW51_LSM330_REG_OUT_X_L_A,
                                  &buffer[i*6],
                                  6);
        if (err != ERR_NONE) {
            return err;
        }
    }
    return ERR_NONE;
}


uint32_t TXW51_LSM330_GYRO_GetDataBlock(uint8_t *buffer, uint32_t numberOfBlocks)
{
    uint32_t err;

    for (int32_t i = 0; i < numberOfBlocks; i++) {
        err = LSM330_ReadMultiSpi(TXW51_LSM330_GYRO,
                                  TXW51_LSM330_REG_OUT_X_L_G,
                                  &buffer[i*6],
                                  6);
        if (err != ERR_NONE) {
            return err;
        }
    }
    return ERR_NONE;
}


uint32_t TXW51_LSM330_ACC_GetFifoStatus(union TXW51_LSM330_FIFO_SRC_REG_A *value)
{
    uint32_t err = LSM330_ReadSpi(TXW51_LSM330_ACC,
                                  TXW51_LSM330_REG_FIFO_SRC_REG_A,
                                  &value->Byte);
    if (err != ERR_NONE) {
        TXW51_LOG_WARNING("[LSM330] Could not get FIFO status. Reading register failed.");
    }

    return err;
}


uint32_t TXW51_LSM330_GYRO_GetFifoStatus(union TXW51_LSM330_FIFO_SRC_REG_G *value)
{
    uint32_t err = LSM330_ReadSpi(TXW51_LSM330_GYRO,
                                  TXW51_LSM330_REG_FIFO_SRC_REG_G,
                                  &value->Byte);
    if (err != ERR_NONE) {
        TXW51_LOG_WARNING("[LSM330] Could not get FIFO status. Reading register failed.");
    }

    return err;
}


uint32_t TXW51_LSM330_SetMotionWakeup(void)
{
    uint32_t err;
    uint8_t reg = 0;

    reg = TXW51_LSM330_REG_CTRL_REG2_A_SM1_EN |
          TXW51_LSM330_REG_CTRL_REG2_A_SM1_PIN;
    err = LSM330_WriteSpi(TXW51_LSM330_ACC, TXW51_LSM330_REG_CTRL_REG2_A, reg);
    if (err != ERR_NONE) {
        TXW51_LOG_WARNING("[LSM330] Could not reset device. Writing register failed.");
        return err;
    }

    reg = TXW51_LSM330_SM1_THRESHOLD;
    err = LSM330_WriteSpi(TXW51_LSM330_ACC, TXW51_LSM330_REG_THRS1_1, reg);
    if (err != ERR_NONE) {
        TXW51_LOG_WARNING("[LSM330] Could not reset device. Writing register failed.");
        return err;
    }

    reg = TXW51_LSM330_SM_OPCODE_GNTH1;
    err = LSM330_WriteSpi(TXW51_LSM330_ACC, TXW51_LSM330_REG_ST1_1, reg);
    if (err != ERR_NONE) {
        TXW51_LOG_WARNING("[LSM330] Could not reset device. Writing register failed.");
        return err;
    }

    reg = TXW51_LSM330_SM_OPCODE_CONT;
    err = LSM330_WriteSpi(TXW51_LSM330_ACC, TXW51_LSM330_REG_ST2_1, reg);
    if (err != ERR_NONE) {
        TXW51_LOG_WARNING("[LSM330] Could not reset device. Writing register failed.");
        return err;
    }

    reg = TXW51_LSM330_REG_MASKB_1_P_X |
          TXW51_LSM330_REG_MASKB_1_N_X |
          TXW51_LSM330_REG_MASKB_1_P_Y |
          TXW51_LSM330_REG_MASKB_1_N_Y |
          TXW51_LSM330_REG_MASKB_1_P_Z |
          TXW51_LSM330_REG_MASKB_1_N_Z;
    err = LSM330_WriteSpi(TXW51_LSM330_ACC, TXW51_LSM330_REG_MASKB_1, reg);
    if (err != ERR_NONE) {
        TXW51_LOG_WARNING("[LSM330] Could not reset device. Writing register failed.");
        return err;
    }

    reg = TXW51_LSM330_REG_MASKA_1_P_X |
          TXW51_LSM330_REG_MASKA_1_N_X |
          TXW51_LSM330_REG_MASKA_1_P_Y |
          TXW51_LSM330_REG_MASKA_1_N_Y |
          TXW51_LSM330_REG_MASKA_1_P_Z |
          TXW51_LSM330_REG_MASKA_1_N_Z;
    err = LSM330_WriteSpi(TXW51_LSM330_ACC, TXW51_LSM330_REG_MASKA_1, reg);
    if (err != ERR_NONE) {
        TXW51_LOG_WARNING("[LSM330] Could not reset device. Writing register failed.");
        return err;
    }

    reg = TXW51_LSM330_REG_SETT1_SITR;
    err = LSM330_WriteSpi(TXW51_LSM330_ACC, TXW51_LSM330_REG_SETT1, reg);
    if (err != ERR_NONE) {
        TXW51_LOG_WARNING("[LSM330] Could not reset device. Writing register failed.");
        return err;
    }

    return err;
}

