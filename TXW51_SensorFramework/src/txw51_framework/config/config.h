/***************************************************************************//**
 * @brief   Configuration file for the TXW51 sensor framework.
 *
 * @file    config.h
 * @version 1.0
 * @date    10.11.2014
 * @author  Daniel Meer
 *
 * @remark  Last Modifications:
 *          10.11.2014 meerd1 created
 ******************************************************************************/

#ifndef TXW51_FRAMEWORK_CONFIG_CONFIG_H_
#define TXW51_FRAMEWORK_CONFIG_CONFIG_H_

/*----- Header-Files ---------------------------------------------------------*/
#include "nrf/app_common/app_timer.h"

/*----- Macros ---------------------------------------------------------------*/

/******************************************************************************/
/* S110 configuration.
 ******************************************************************************/
#define CONFIG_S110_USE_SCHEDULER   ( true )    /**< Enable or disable the use of the scheduler. */


/******************************************************************************/
/* Clock configuration.
 ******************************************************************************/
#define CONFIG_CLOCK_LFCLK_SOURCE   ( NRF_CLOCK_LFCLKSRC_XTAL_20_PPM )  /**< Set the source for the low frequency clock. */


/******************************************************************************/
/* Scheduler configuration.
 ******************************************************************************/
#define CONFIG_SCHED_MAX_EVENT_DATA_SIZE   sizeof(app_timer_event_t)    /**< Maximum size of scheduler events. Note that scheduler BLE stack events do not contain any data, as the events are being pulled from the stack in the event handler. */
#define CONFIG_SCHED_QUEUE_SIZE            ( 10 )                       /**< Maximum number of events in the scheduler queue. */


/******************************************************************************/
/* Timer configuration.
 ******************************************************************************/
#define LFCLK_FREQUENCY                 ( 32768UL )  /**< LFCLK frequency in Hertz, constant. */
#define RTC_FREQUENCY                   ( 128UL )    /**< Required RTC working clock RTC_FREQUENCY Hertz. Changeable. */
#define CONFIG_TIMERS_PRESCALER         ((LFCLK_FREQUENCY / RTC_FREQUENCY) - 1UL)   /**< Prescaler of the timers. f = LFCLK/(prescaler + 1) */
#define CONFIG_TIMERS_MAX_TIMERS        ( 4 )  /**< Maximum number of simultaneously created timers. */
#define CONFIG_TIMERS_OP_QUEUE_SIZE     ( 4 )  /**< Size of timer operation queues. */


/******************************************************************************/
/* GAP configuration.
 ******************************************************************************/
#define CONFIG_GAP_DEVICE_NAME                      "TXW51" /**< Name of device. Will be included in the advertising data. */

#define CONFIG_GAP_IS_SRVC_CHANGED_CHARACT_PRESENT  ( 0 )   /**< Include or not the service_changed characteristic. if not enabled, the server's database cannot be changed for the lifetime of the device*/

#define CONFIG_GAP_BLE_APPEARANCE                   ( BLE_APPEARANCE_GENERIC_TAG )  /**< Value of the appearance field in the advertising packet. */

#define CONFIG_GAP_SEC_PARAM_TIMEOUT                ( 30 )  /**< Timeout for Pairing Request or Security Request (in seconds). */
#define CONFIG_GAP_SEC_PARAM_BOND                   ( 0 )   /**< Perform no bonding. */
#define CONFIG_GAP_SEC_PARAM_MITM                   ( 0 )   /**< Man In The Middle protection not required. */
#define CONFIG_GAP_SEC_PARAM_IO_CAPABILITIES        ( BLE_GAP_IO_CAPS_NONE ) /**< No I/O capabilities. */
#define CONFIG_GAP_SEC_PARAM_OOB                    ( 0 )   /**< Out Of Band data not available. */
#define CONFIG_GAP_SEC_PARAM_MIN_KEY_SIZE           ( 7 )   /**< Minimum encryption key size. */
#define CONFIG_GAP_SEC_PARAM_MAX_KEY_SIZE           ( 16 )  /**< Maximum encryption key size. */

#define CONFIG_GAP_ADV_INTERVAL                     ( 64 )  /**< The advertising interval (in units of 0.625 ms. This value corresponds to 40 ms). */
#define CONFIG_GAP_ADV_TIMEOUT_IN_SECONDS           ( 180 ) /**< The advertising timeout (in units of seconds). */
#define CONFIG_GAP_ADV_FLAGS                        ( BLE_GAP_ADV_FLAGS_LE_ONLY_LIMITED_DISC_MODE ) /**< Flags that are set in the advertising packet. */

#define CONFIG_GAP_MIN_CONN_INTERVAL                MSEC_TO_UNITS(7.5, UNIT_1_25_MS)    /**< Minimum acceptable connection interval. */
#define CONFIG_GAP_MAX_CONN_INTERVAL                MSEC_TO_UNITS(7.5, UNIT_1_25_MS)    /**< Maximum acceptable connection interval. */
#define CONFIG_GAP_SLAVE_LATENCY                    ( 0 )                               /**< Slave latency. */
#define CONFIG_GAP_CONN_SUP_TIMEOUT                 MSEC_TO_UNITS(4000, UNIT_10_MS)     /**< Connection supervisory timeout (4 seconds). */
#define CONFIG_GAP_FIRST_CONN_PARAMS_UPDATE_DELAY   APP_TIMER_TICKS(5000, CONFIG_TIMERS_PRESCALER)  /**< Time from initiating event (connect or start of notification) to first time sd_ble_gap_conn_param_update is called (5 seconds). */
#define CONFIG_GAP_NEXT_CONN_PARAMS_UPDATE_DELAY    APP_TIMER_TICKS(30000, CONFIG_TIMERS_PRESCALER) /**< Time between each call to sd_ble_gap_conn_param_update after the first call (30 seconds). */
#define CONFIG_GAP_MAX_CONN_PARAMS_UPDATE_COUNT     ( 3 )                               /**< Number of attempts before giving up the connection parameter negotiation. */


/******************************************************************************/
/* GPIOTE configuration.
 ******************************************************************************/
#define CONFIG_GPIOTE_MAX_USERS         ( 1 )   /**< Maximum number of users of the GPIOTE handler. */


/******************************************************************************/
/* Hardware configuration.
 ******************************************************************************/
#define CONFIG_HW_LED_ADVERTISING       ( TXW51_GPIO_PIN_LED1 ) /**< LED pin to signal that the device is advertising. */
#define CONFIG_HW_LED_ON                ( TXW51_GPIO_PIN_LED0 ) /**< LED pin to signal that the device is on. */
#define CONFIG_HW_LED_ERROR             ( TXW51_GPIO_PIN_LED2 ) /**< LED pin to signal that the device has an error. */


/******************************************************************************/
/* Log configuration.
 ******************************************************************************/
#define CONFIG_LOG_UART_RX              ( TXW51_GPIO_PIN_UART0_RX )     /**< UART RX pin for the logging module. */
#define CONFIG_LOG_UART_TX              ( TXW51_GPIO_PIN_UART0_TX )     /**< UART TX pin for the logging module. */
#define CONFIG_LOG_UART_CTS             ( 0 )                           /**< UART CTS pin for the logging module. */
#define CONFIG_LOG_UART_RTS             ( 0 )                           /**< UART RTS pin for the logging module. */
#define CONFIG_LOG_UART_HWFC            ( false )                       /**< Enable or disable hardware flow control. */
#define CONFIG_LOG_UART_BAUDRATE        ( TXW51_UART_BAUDRATE_38400 )   /**< UART baudrate for the logging module. */

#define CONFIG_LOG_LEVEL_NONE           ( 0 )   /**< Log level "None". Do not change. */
#define CONFIG_LOG_LEVEL_ERROR          ( 1 )   /**< Log level "Error". Do not change. */
#define CONFIG_LOG_LEVEL_WARNING        ( 2 )   /**< Log level "Warning". Do not change. */
#define CONFIG_LOG_LEVEL_INFO           ( 3 )   /**< Log level "Info". Do not change. */
#define CONFIG_LOG_LEVEL_DEBUG          ( 4 )   /**< Log level "Debug". Do not change. */

#define CONFIG_LOG_LEVEL                ( CONFIG_LOG_LEVEL_DEBUG )  /**< Sets the level of logging output. */

/*----- Data types -----------------------------------------------------------*/

/*----- Function prototypes --------------------------------------------------*/

/*----- Data -----------------------------------------------------------------*/

#endif /* TXW51_FRAMEWORK_CONFIG_CONFIG_H_ */
