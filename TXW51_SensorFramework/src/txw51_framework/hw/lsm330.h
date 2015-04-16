/***************************************************************************//**
 * @brief   Driver to the LSM330 sensor.
 *
 * The LSM330 contains of an accelerometer, a gyroscope and a temperature
 * sensor. The communication on the TXW51 is over SPI. Multiple interrupt lines
 * are connected with the LSM330 as well.
 *
 * @file    lsm330.h
 * @version 1.0
 * @date    26.11.2014
 * @author  Daniel Meer
 *
 * @remark  Last Modifications:
 *          26.11.2014 meerd1 created
 ******************************************************************************/

#ifndef TXW51_FRAMEWORK_HW_LSM330_H_
#define TXW51_FRAMEWORK_HW_LSM330_H_

/*----- Header-Files ---------------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>

#include "txw51_framework/hw/lsm330_registers.h"

/*----- Macros ---------------------------------------------------------------*/
#define TXW51_LSM330_FLAG_MULTI_RW  ( 1UL << 6 )    /**< Flag for SPI multi r/w (see SPI protocol in LSM330 datasheet). */
#define TXW51_LSM330_FLAG_READ      ( 1UL << 7 )    /**< Flag for SPI read (see SPI protocol in LSM330 datasheet). */
#define TXW51_LSM330_TEMP_REF       ( 40 )          /**< Referenz value to calculate true temperature value (40 - value). */

#define TXW51_LSM330_GPIO_SS_ACC    ( TXW51_GPIO_PIN_SPI0_SS_A )    /**< Slave Select pin for accelerometer. */
#define TXW51_LSM330_GPIO_SS_GYRO   ( TXW51_GPIO_PIN_SPI0_SS_G )    /**< Slave Select pin for gyroscope. */

#define TXW51_LSM330_GPIO_INT1_ACC              ( TXW51_GPIO_PIN_INT1_A )       /**< GPIO pin for INT1_A. */
#define TXW51_LSM330_GPIO_INT1_ACC_PULL_CONF    ( NRF_GPIO_PIN_PULLDOWN )       /**< Pull configuration for INT1_A. */
#define TXW51_LSM330_GPIO_INT1_ACC_POL_CONF     ( NRF_GPIOTE_POLARITY_LOTOHI )  /**< GPIOTE interrupt polarity for INT1_A. */
#define TXW51_LSM330_GPIO_INT1_ACC_CHANNEL      ( 0 )                           /**< GPIOTE interrupt channel for INT1_A. */

#define TXW51_LSM330_GPIO_INT2_ACC              ( TXW51_GPIO_PIN_INT2_A )       /**< GPIO pin for INT2_A. */
#define TXW51_LSM330_GPIO_INT2_ACC_PULL_CONF    ( NRF_GPIO_PIN_PULLDOWN )       /**< Pull configuration for INT2_A. */
#define TXW51_LSM330_GPIO_INT2_ACC_POL_CONF     ( NRF_GPIOTE_POLARITY_LOTOHI )  /**< GPIOTE interrupt polarity for INT2_A. */
#define TXW51_LSM330_GPIO_INT2_ACC_CHANNEL      ( 1 )                           /**< GPIOTE interrupt channel for INT2_A. */

#define TXW51_LSM330_GPIO_INT1_GYRO             ( TXW51_GPIO_PIN_INT1_G )       /**< GPIO pin for INT1_G. */
#define TXW51_LSM330_GPIO_INT1_GYRO_PULL_CONF   ( NRF_GPIO_PIN_NOPULL )         /**< Pull configuration for INT1_G. */
#define TXW51_LSM330_GPIO_INT1_GYRO_POL_CONF    ( NRF_GPIOTE_POLARITY_LOTOHI )  /**< GPIOTE interrupt polarity for INT1_G. */
#define TXW51_LSM330_GPIO_INT1_GYRO_CHANNEL     ( 2 )                           /**< GPIOTE interrupt channel for INT1_G. */

#define TXW51_LSM330_GPIO_INT2_GYRO             ( TXW51_GPIO_PIN_INT2_G )       /**< GPIO pin for INT2_G. */
#define TXW51_LSM330_GPIO_INT2_GYRO_PULL_CONF   ( NRF_GPIO_PIN_NOPULL )         /**< Pull configuration for INT2_G. */
#define TXW51_LSM330_GPIO_INT2_GYRO_POL_CONF    ( NRF_GPIOTE_POLARITY_LOTOHI )  /**< GPIOTE interrupt polarity for INT2_G. */
#define TXW51_LSM330_GPIO_INT2_GYRO_CHANNEL     ( 3 )                           /**< GPIOTE interrupt channel for INT2_G. */

#define TWX51_LSM330_CONFIG_INTERRUPT_POL       ( TXW51_LSM330_REG_CTRL_REG4_A_IEA_ActiveHigh ) /**< LSM330 interrupt polarity. */
#define TWX51_LSM330_CONFIG_INTERRUPT_LATCH     ( TXW51_LSM330_REG_CTRL_REG4_A_IEL_Pulsed )     /**< LSM330 interrupt latched/pulsed. */

#define TXW51_LSM330_SM1_THRESHOLD  ( 70 )      /**< Threshold for the Motion Wake-Up state machine. */

/*----- Data types -----------------------------------------------------------*/
/**
 * @brief List of the available full-scale settings for the accelerometer.
 */
enum TXW51_LSM330_ACC_Fullscale {
    TXW51_LSM330_ACC_FSCALE_2G  = 0x00,     /**< Full-scale 2g. */
    TXW51_LSM330_ACC_FSCALE_4G  = 0x01,     /**< Full-scale 4g. */
    TXW51_LSM330_ACC_FSCALE_6G  = 0x02,     /**< Full-scale 6g. */
    TXW51_LSM330_ACC_FSCALE_8G  = 0x03,     /**< Full-scale 8g. */
    TXW51_LSM330_ACC_FSCALE_16G = 0x04      /**< Full-scale 16g. */
};

/**
 * @brief List of the available full-scale settings for the gyroscope.
 */
enum TXW51_LSM330_GYRO_Fullscale {
    TXW51_LSM330_GYRO_FSCALE_250DPS  = 0x00,    /**< Full-scale 250dps. */
    TXW51_LSM330_GYRO_FSCALE_500DPS  = 0x01,    /**< Full-scale 500dps. */
    TXW51_LSM330_GYRO_FSCALE_2000DPS = 0x02     /**< Full-scale 2000dps. */
};

/**
 * @brief List of the available ODR settings for the accelerometer.
 */
enum TXW51_LSM330_ACC_Odr {
    TXW51_LSM330_ACC_ODR_OFF   = 0x00,      /**< Accelerometer turned off. */
    TXW51_LSM330_ACC_ODR_3_125 = 0x01,      /**< ODR 3.125Hz. */
    TXW51_LSM330_ACC_ODR_6_25  = 0x02,      /**< ODR 6.25Hz. */
    TXW51_LSM330_ACC_ODR_12_5  = 0x03,      /**< ODR 12.5Hz. */
    TXW51_LSM330_ACC_ODR_25    = 0x04,      /**< ODR 25Hz. */
    TXW51_LSM330_ACC_ODR_50    = 0x05,      /**< ODR 50Hz. */
    TXW51_LSM330_ACC_ODR_100   = 0x06,      /**< ODR 100Hz. */
    TXW51_LSM330_ACC_ODR_400   = 0x07,      /**< ODR 400Hz. */
    TXW51_LSM330_ACC_ODR_800   = 0x08,      /**< ODR 800Hz. */
    TXW51_LSM330_ACC_ODR_1600  = 0x09       /**< ODR 1600Hz. */
};

/**
 * @brief List of the available ODR settings for the gyroscope.
 */
enum TXW51_LSM330_GYRO_Odr {
    TXW51_LSM330_GYRO_ODR_95  = 0x00,   /**< ODR 95Hz. */
    TXW51_LSM330_GYRO_ODR_190 = 0x01,   /**< ODR 190Hz. */
    TXW51_LSM330_GYRO_ODR_380 = 0x02,   /**< ODR 380Hz. */
    TXW51_LSM330_GYRO_ODR_760 = 0x03    /**< ODR 760Hz. */
};

/**
 * @brief List of the available FIFO modes for the accelerometer.
 */
enum TXW51_LSM330_ACC_FifoMode {
    TXW51_LSM330_ACC_FIFO_MODE_BYPASS           = 0x00,     /**< Set FIFO to bypass mode. */
    TXW51_LSM330_ACC_FIFO_MODE_FIFO             = 0x01,     /**< Set FIFO to fifo mode. */
    TXW51_LSM330_ACC_FIFO_MODE_STREAM           = 0x02,     /**< Set FIFO to stream mode. */
    TXW51_LSM330_ACC_FIFO_MODE_STREAM_TO_FIFO   = 0x03,     /**< Set FIFO to stream-to-fifo mode. */
    TXW51_LSM330_ACC_FIFO_MODE_BYPASS_TO_STREAM = 0x04,     /**< Set FIFO to bypass-to-stream mode. */
};

/**
 * @brief List of the available FIFO modes for the gyroscope.
 */
enum TXW51_LSM330_GYRO_FifoMode {
    TXW51_LSM330_GYRO_FIFO_MODE_BYPASS           = 0x00,    /**< Set FIFO to bypass mode. */
    TXW51_LSM330_GYRO_FIFO_MODE_FIFO             = 0x01,    /**< Set FIFO to fifo mode. */
    TXW51_LSM330_GYRO_FIFO_MODE_STREAM           = 0x02,    /**< Set FIFO to stream mode. */
    TXW51_LSM330_GYRO_FIFO_MODE_STREAM_TO_FIFO   = 0x03,    /**< Set FIFO to stream-to-fifo mode. */
    TXW51_LSM330_GYRO_FIFO_MODE_BYPASS_TO_STREAM = 0x04     /**< Set FIFO to bypass-to-stream mode. */
};

/**
 * @brief Structure to enable or disable specific axes.
 */
struct TXW51_LSM330_Axis {
    bool X_Enable;      /**< Enable or disable x axis. */
    bool Y_Enable;      /**< Enable or disable y axis. */
    bool Z_Enable;      /**< Enable or disable z axis. */
};

/**
 * @brief Structure to configure the accelerometer interrupts.
 */
struct TXW51_LSM330_ACC_Interrupts {
    bool Int1A_Enable;      /**< Enable INT1A interrupt. */
    bool Int1A_DataReady;   /**< Set data-ready interrupt of INT1A. */
    bool Int1A_Empty;       /**< Set empty interrupt of INT1A. */
    bool Int1A_Watermark;   /**< Set watermark interrupt of INT1A. */
    bool Int1A_Overrun;     /**< Set overrun interrupt of INT1A. */
    bool Int2A_Enable;      /**< Enable INT2A interrupt. */
};

/**
 * @brief Structure to configure the gyroscope interrupts.
 */
struct TXW51_LSM330_GYRO_Interrupts {
    bool Int1G_Enable;      /**< Enable INT1G interrupt. */
    bool Int2G_Enable;      /**< Enable INT2G interrupt. */
    bool Int2G_DataReady;   /**< Set dataready interrupt of INT2G. */
    bool Int2G_Empty;       /**< Set empty interrupt of INT2G. */
    bool Int2G_Watermark;   /**< Set watermark interrupt of INT2G. */
    bool Int2G_Overrun;     /**< Set overrun interrupt of INT2G. */
};

/**
 * @brief Structure to configure the accelerometer FIFO.
 */
struct TXW51_LSM330_ACC_FifoInit {
    bool    FifoEnable;                     /**< Enable FIFO. */
    enum TXW51_LSM330_ACC_FifoMode Mode;    /**< Set FIFO mode. */
    uint8_t Watermark;                      /**< Set watermark value. */
    bool    WatermarkEnable;                /**< Enable watermark. */
};

/**
 * @brief Structure to configure the gyroscope FIFO.
 */
struct TXW51_LSM330_GYRO_FifoInit {
    bool    FifoEnable;                     /**< Enable FIFO. */
    enum TXW51_LSM330_GYRO_FifoMode Mode;   /**< Set FIFO mode. */
    uint8_t Watermark;                      /**< Set watermark value. */
    bool    WatermarkEnable;                /**< Enable watermark. */
};

/**
 * @brief Union of the FIFO_SRC_REG_A register.
 */
union TXW51_LSM330_FIFO_SRC_REG_A {
    struct {
        uint8_t FSS:5;          /**< bit: 0..4  FIFO stored data level. */
        uint8_t EMPTY:1;        /**< bit:    5  FIFO empty bit. */
        uint8_t OVRN_FIFO:1;    /**< bit:    6  Overrun bit status. */
        uint8_t WTM:1;          /**< bit:    7  Watermark status. */
    } Bit;                      /**< Representation of the register values. */
    uint8_t Byte;               /**< Representation of the whole register. */
};

///**
// * @brief Union of the CTRL_REG2_A register.
// */
//union TXW51_LSM330_CTRL_REG2_A {
//    struct {
//        uint8_t SM1_EN:1;       /**< bit:    0  SM1 enable/disable. */
//        uint8_t _reserved0:2;   /**< bit: 1..2  Reserved. */
//        uint8_t SM1_PIN:1;      /**< bit:    3  SM1 interrupt pin. */
//        uint8_t _reserved1:1;   /**< bit:    4  Reserved. */
//        uint8_t HYSTx_1:3;      /**< bit: 5..7  Hysteresis unsigned value. */
//    } Bit;                      /**< Representation of the register values. */
//    uint8_t Byte;               /**< Representation of the whole register. */
//};

/**
 * @brief Union of the FIFO_CTRL_REG_A register.
 */
union TXW51_LSM330_FIFO_CTRL_REG_A {
    struct {
        uint8_t WTMP:5;         /**< bit: 0..4  FIFO watermark level setting. */
        uint8_t FMODE:3;        /**< bit: 5..7  FIFO mode selection. */
    } Bit;                      /**< Representation of the register values. */
    uint8_t Byte;               /**< Representation of the whole register. */
};

/**
 * @brief Union of the FIFO_CTRL_REG_G register.
 */
union TXW51_LSM330_FIFO_CTRL_REG_G {
    struct {
        uint8_t WTM:5;          /**< bit: 0..4  FIFO watermark level setting. */
        uint8_t FM:3;           /**< bit: 5..7  FIFO mode selection. */
    } Bit;                      /**< Representation of the register values. */
    uint8_t Byte;               /**< Representation of the whole register. */
};

/**
 * @brief Union of the FIFO_SRC_REG_G register.
 */
union TXW51_LSM330_FIFO_SRC_REG_G {
    struct {
        uint8_t FSS:5;          /**< bit: 0..4  FIFO stored data level. */
        uint8_t EMPTY:1;        /**< bit:    5  FIFO empty bit. */
        uint8_t OVRN:1;         /**< bit:    6  Overrun bit status. */
        uint8_t WTM:1;          /**< bit:    7  Watermark status. */
    } Bit;                      /**< Representation of the register values. */
    uint8_t Byte;               /**< Representation of the whole register. */
};

/*----- Function prototypes --------------------------------------------------*/

/***************************************************************************//**
 * @brief Initializes the LSM330.
 *
 * @return ERR_NONE if no error occurred.
 *         ERR_LSM330_READ_FAILED if reading from the sensor failed.
 *         ERR_LSM330_WRITE_FAILED if writing to the sensor failed.
 ******************************************************************************/
extern uint32_t TXW51_LSM330_Init(void);

/***************************************************************************//**
 * @brief Deinitializes the LSM330.
 *
 * This can be used to save energy on the GPIO pins.
 *
 * @return Nothing.
 ******************************************************************************/
extern void TXW51_LSM330_Deinit(void);

/***************************************************************************//**
* @brief Enables the gyroscope.
*
* @param[in] enable True to enable and false to disable the gyroscope.
*
* @return ERR_NONE if no error occurred.
*         ERR_LSM330_READ_FAILED if reading from the sensor failed.
*         ERR_LSM330_WRITE_FAILED if writing to the sensor failed.
******************************************************************************/
extern uint32_t TXW51_LSM330_EnableGyro(bool enable);

/***************************************************************************//**
* @brief Resets the LSM330.
*
* @return ERR_NONE if no error occurred.
*         ERR_LSM330_WRITE_FAILED if writing to the sensor failed.
******************************************************************************/
extern uint32_t TXW51_LSM330_ResetDevice(void);

/***************************************************************************//**
* @brief Sets the state machine of the LSM330 to generate an interrupt when
*        someone shakes the sensor.
*
* @return ERR_NONE if no error occurred.
*         ERR_LSM330_WRITE_FAILED if writing to the sensor failed.
******************************************************************************/
extern uint32_t TXW51_LSM330_SetMotionWakeup(void);

/***************************************************************************//**
 * @brief Configures the interrupts of the accelerometer.
 *
 * @param[in] config The configuration values for the interrupts.
 *
 * @return ERR_NONE if no error occurred.
 *         ERR_LSM330_READ_FAILED if reading from the sensor failed.
 *         ERR_LSM330_WRITE_FAILED if writing to the sensor failed.
 ******************************************************************************/
extern uint32_t TXW51_LSM330_ACC_ConfigInterrupts(struct TXW51_LSM330_ACC_Interrupts *config);

/***************************************************************************//**
 * @brief Configures which axes of the accelerometer to enable.
 *
 * @param[in] axis The configuration for the axes.
 *
 * @return ERR_NONE if no error occurred.
 *         ERR_LSM330_READ_FAILED if reading from the sensor failed.
 *         ERR_LSM330_WRITE_FAILED if writing to the sensor failed.
 ******************************************************************************/
extern uint32_t TXW51_LSM330_ACC_SetActiveAxis(struct TXW51_LSM330_Axis *axis);

/***************************************************************************//**
 * @brief Reads the full-scale value of the accelerometer.
 *
 * @return TXW51_LSM330_ACC_FSCALE_2G
 *         TXW51_LSM330_ACC_FSCALE_4G
 *         TXW51_LSM330_ACC_FSCALE_6G
 *         TXW51_LSM330_ACC_FSCALE_8G
 *         TXW51_LSM330_ACC_FSCALE_16G
 *         TXW51_LSM330_ACC_FSCALE_2G if an error occurred.
 ******************************************************************************/
extern enum TXW51_LSM330_ACC_Fullscale TXW51_LSM330_ACC_GetFullscale(void);

/***************************************************************************//**
 * @brief Sets the full-scale value for the accelerometer.
 *
 * @param[in] fullscale The full-scale value to set.
 *
 * @return ERR_NONE if no error occurred.
 *         ERR_LSM330_READ_FAILED if reading from the sensor failed.
 *         ERR_LSM330_WRITE_FAILED if writing to the sensor failed.
 ******************************************************************************/
extern uint32_t TXW51_LSM330_ACC_SetFullscale(enum TXW51_LSM330_ACC_Fullscale fullscale);

/***************************************************************************//**
 * @brief Reads the ODR value of the accelerometer.
 *
 * @return TXW51_LSM330_ACC_ODR_OFF
 *         TXW51_LSM330_ACC_ODR_3_125
 *         TXW51_LSM330_ACC_ODR_6_25
 *         TXW51_LSM330_ACC_ODR_12_5
 *         TXW51_LSM330_ACC_ODR_25
 *         TXW51_LSM330_ACC_ODR_50
 *         TXW51_LSM330_ACC_ODR_100
 *         TXW51_LSM330_ACC_ODR_400
 *         TXW51_LSM330_ACC_ODR_800
 *         TXW51_LSM330_ACC_ODR_1600
 *         TXW51_LSM330_ACC_ODR_OFF if an error occurred.
 ******************************************************************************/
extern enum TXW51_LSM330_ACC_Odr TXW51_LSM330_ACC_GetOdr(void);

/***************************************************************************//**
 * @brief Sets the ODR value for the accelerometer.
 *
 * @param[in] odr The odr value to set.
 *
 * @return ERR_NONE if no error occurred.
 *         ERR_LSM330_READ_FAILED if reading from the sensor failed.
 *         ERR_LSM330_WRITE_FAILED if writing to the sensor failed.
 ******************************************************************************/
extern uint32_t TXW51_LSM330_ACC_SetOdr(enum TXW51_LSM330_ACC_Odr odr);

/***************************************************************************//**
 * @brief Configures the FIFO of the accelerometer.
 *
 * @param[in] config The configuration values for the FIFO.
 *
 * @return ERR_NONE if no error occurred.
 *         ERR_LSM330_READ_FAILED if reading from the sensor failed.
 *         ERR_LSM330_WRITE_FAILED if writing to the sensor failed.
 ******************************************************************************/
extern uint32_t TXW51_LSM330_ACC_ConfigFifo(struct TXW51_LSM330_ACC_FifoInit *config);

/***************************************************************************//**
* @brief Reads the current status of the FIFO for the accelerometer.
*
* @param[out] value Buffer for the status of the FIFO.
*
* @return ERR_NONE if no error occurred.
*         ERR_LSM330_READ_FAILED if reading from the sensor failed.
******************************************************************************/
extern uint32_t TXW51_LSM330_ACC_GetFifoStatus(union TXW51_LSM330_FIFO_SRC_REG_A *value);

/***************************************************************************//**
* @brief Reads multiple blocks of the value of all three accelerometer axes.
*
* @param[out] buffer         Buffer to save the values.
* @param[in]  numberOfBlocks Number of blocks to read.
*
* @return ERR_NONE if no error occurred.
*         ERR_LSM330_READ_FAILED if reading from the sensor failed.
******************************************************************************/
extern uint32_t TXW51_LSM330_ACC_GetDataBlock(uint8_t *buffer, uint32_t numberOfBlocks);

/***************************************************************************//**
 * @brief Configures the interrupts of the gyroscope.
 *
 * @param[in] config The configuration values for the interrupts.
 *
 * @return ERR_NONE if no error occurred.
 *         ERR_LSM330_READ_FAILED if reading from the sensor failed.
 *         ERR_LSM330_WRITE_FAILED if writing to the sensor failed.
 ******************************************************************************/
extern uint32_t TXW51_LSM330_GYRO_ConfigInterrupts(struct TXW51_LSM330_GYRO_Interrupts *config);

/***************************************************************************//**
 * @brief Configures which axes of the gyroscope to enable.
 *
 * @param[in] axis The configuration for the axes.
 *
 * @return ERR_NONE if no error occurred.
 *         ERR_LSM330_READ_FAILED if reading from the sensor failed.
 *         ERR_LSM330_WRITE_FAILED if writing to the sensor failed.
 ******************************************************************************/
extern uint32_t TXW51_LSM330_GYRO_SetActiveAxis(struct TXW51_LSM330_Axis *axis);

/***************************************************************************//**
 * @brief Reads the full-scale value of the gyroscope.
 *
 * @return TXW51_LSM330_GYRO_FSCALE_250DPS
 *         TXW51_LSM330_GYRO_FSCALE_500DPS
 *         TXW51_LSM330_GYRO_FSCALE_2000DPS
 *         TXW51_LSM330_GYRO_FSCALE_250DPS if an error occurred.
 ******************************************************************************/
extern enum TXW51_LSM330_GYRO_Fullscale TXW51_LSM330_GYRO_GetFullscale(void);

/***************************************************************************//**
 * @brief Sets the full-scale value for the gyroscope.
 *
 * @param[in] fullscale The full-scale value to set.
 *
 * @return ERR_NONE if no error occurred.
 *         ERR_LSM330_READ_FAILED if reading from the sensor failed.
 *         ERR_LSM330_WRITE_FAILED if writing to the sensor failed.
 ******************************************************************************/
extern uint32_t TXW51_LSM330_GYRO_SetFullscale(enum TXW51_LSM330_GYRO_Fullscale fullscale);

/***************************************************************************//**
 * @brief Reads the ODR value of the gyroscope.
 *
 * @return TXW51_LSM330_GYRO_ODR_95
 *         TXW51_LSM330_GYRO_ODR_190
 *         TXW51_LSM330_GYRO_ODR_380
 *         TXW51_LSM330_GYRO_ODR_760
 *         TXW51_LSM330_GYRO_ODR_95 if an error occurred.
 ******************************************************************************/
extern enum TXW51_LSM330_GYRO_Odr TXW51_LSM330_GYRO_GetOdr(void);

/***************************************************************************//**
 * @brief Sets the ODR value for the gyroscope.
 *
 * @param[in] odr The odr value to set.
 *
 * @return ERR_NONE if no error occurred.
 *         ERR_LSM330_READ_FAILED if reading from the sensor failed.
 *         ERR_LSM330_WRITE_FAILED if writing to the sensor failed.
 ******************************************************************************/
extern uint32_t TXW51_LSM330_GYRO_SetOdr(enum TXW51_LSM330_GYRO_Odr odr);

/***************************************************************************//**
 * @brief Configures the FIFO of the gyroscope.
 *
 * @param[in] config The configuration values for the FIFO.
 *
 * @return ERR_NONE if no error occurred.
 *         ERR_LSM330_READ_FAILED if reading from the sensor failed.
 *         ERR_LSM330_WRITE_FAILED if writing to the sensor failed.
 ******************************************************************************/
extern uint32_t TXW51_LSM330_GYRO_ConfigFifo(struct TXW51_LSM330_GYRO_FifoInit *config);

/***************************************************************************//**
* @brief Reads the current status of the FIFO for the gyroscope.
*
* @param[out] value Buffer for the status of the FIFO.
*
* @return ERR_NONE if no error occurred.
*         ERR_LSM330_READ_FAILED if reading from the sensor failed.
******************************************************************************/
extern uint32_t TXW51_LSM330_GYRO_GetFifoStatus(union TXW51_LSM330_FIFO_SRC_REG_G *value);

/***************************************************************************//**
* @brief Reads multiple blocks of the value of all three gyroscope axes.
*
* @param[out] buffer         Buffer to save the values.
* @param[in]  numberOfBlocks Number of blocks to read.
*
* @return ERR_NONE if no error occurred.
*         ERR_LSM330_READ_FAILED if reading from the sensor failed.
******************************************************************************/
extern uint32_t TXW51_LSM330_GYRO_GetDataBlock(uint8_t *buffer, uint32_t numberOfBlocks);

/***************************************************************************//**
 * @brief Reads the temperature value.
 *
 * The gyroscope needs to be turned on for the temperature sensor to function.
 *
 * @param[out] value Buffer to save the temperature.
 *
 * @return ERR_NONE if no error occurred.
 *         ERR_LSM330_READ_FAILED if reading from the sensor failed.
 ******************************************************************************/
extern uint32_t TXW51_LSM330_GetTemperature(uint8_t *value);

/***************************************************************************//**
 * @brief Reads periodically all sensor values and outputs them on the log.
 *
 * This function is only for debugging purposes. It does not return.
 *
 * @return Function does not return.
 ******************************************************************************/
extern void TXW51_LSM330_Debug_StreamValues(void);

/***************************************************************************//**
 * @brief Reads a register from the accelerometer and show the value on the log.
 *
 * This function is mainly for debugging purposes.
 *
 * @param[in] reg The register address.
 *
 * @return Nothing.
 ******************************************************************************/
extern void TXW51_LSM330_ACC_Debug_ShowRegister(uint32_t reg);

/*----- Data -----------------------------------------------------------------*/

#endif /* TXW51_FRAMEWORK_HW_LSM330_H_ */
