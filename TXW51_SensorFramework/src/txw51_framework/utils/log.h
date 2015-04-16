/***************************************************************************//**
 * @brief   This module can be used to log messages.
 *
 * It uses the UART to send static logging messages used for debugging. The
 * amount of logging messages can be configured.
 *
 * @file    log.h
 * @version 1.0
 * @date    17.11.2014
 * @author  Daniel Meer
 *
 * @remark  Last Modifications:
 *          17.11.2014 meerd1 created
 ******************************************************************************/

#ifndef TXW51_FRAMEWORK_UTILS_LOG_H_
#define TXW51_FRAMEWORK_UTILS_LOG_H_

/*----- Header-Files ---------------------------------------------------------*/
#include "txw51_framework/config/config.h"

/*----- Macros ---------------------------------------------------------------*/

/***************************************************************************//**
 * @brief Function: TXW51_LOG_DEBUG()
 *        Logs a debug message.
 *
 * @param[in] msg String that contains the text to be written.
 * @return Nothing.
 ******************************************************************************/
#if (CONFIG_LOG_LEVEL >= CONFIG_LOG_LEVEL_DEBUG)
#define TXW51_LOG_DEBUG(msg) \
    (TXW51_LOG_Print(msg, TXW51_LOG_LEVEL_DEBUG))
#else
#define TXW51_LOG_DEBUG(...)
#endif /* CONFIG_LOG_LEVEL >= CONFIG_LOG_LEVEL_DEBUG */


/***************************************************************************//**
 * @brief Function: TXW51_LOG_INFO()
 *        Logs an info message.
 *
 * @param[in] msg String that contains the text to be written.
 * @return Nothing.
 ******************************************************************************/
#if (CONFIG_LOG_LEVEL >= CONFIG_LOG_LEVEL_INFO)
#define TXW51_LOG_INFO(msg) \
    (TXW51_LOG_Print(msg, TXW51_LOG_LEVEL_INFO))
#else
#define TXW51_LOG_INFO(...)
#endif /* CONFIG_LOG_LEVEL >= CONFIG_LOG_LEVEL_INFO */


/***************************************************************************//**
 * @brief Function: TXW51_LOG_WARNING()
 *        Logs a warning message.
 *
 * @param[in] msg String that contains the text to be written.
 * @return Nothing.
 ******************************************************************************/
#if (CONFIG_LOG_LEVEL >= CONFIG_LOG_LEVEL_WARNING)
#define TXW51_LOG_WARNING(msg) \
    (TXW51_LOG_Print(msg, TXW51_LOG_LEVEL_WARNING))
#else
#define TXW51_LOG_WARNING(...)
#endif /* CONFIG_LOG_LEVEL >= CONFIG_LOG_LEVEL_WARNING */


/***************************************************************************//**
 * @brief Function: TXW51_LOG_ERROR()
 *        Logs an error message.
 *
 * @param[in] msg String that contains the text to be written.
 * @return Nothing.
 ******************************************************************************/
#if (CONFIG_LOG_LEVEL >= CONFIG_LOG_LEVEL_ERROR)
#define TXW51_LOG_ERROR(msg) \
    (TXW51_LOG_Print(msg, TXW51_LOG_LEVEL_ERROR))
#else
#define TXW51_LOG_ERROR(...)
#endif /* CONFIG_LOG_LEVEL >= CONFIG_LOG_LEVEL_ERROR */


/*----- Data types -----------------------------------------------------------*/
/**
 * @brief Defines the various log levels which can be used to post a debug
 * message. The level defines the severity of the message.
 */
enum TXW51_LOG_Level {
    TXW51_LOG_LEVEL_NONE    = CONFIG_LOG_LEVEL_NONE,
    TXW51_LOG_LEVEL_ERROR   = CONFIG_LOG_LEVEL_ERROR,
    TXW51_LOG_LEVEL_WARNING = CONFIG_LOG_LEVEL_WARNING,
    TXW51_LOG_LEVEL_INFO    = CONFIG_LOG_LEVEL_INFO,
    TXW51_LOG_LEVEL_DEBUG   = CONFIG_LOG_LEVEL_DEBUG,
};

/*----- Function prototypes --------------------------------------------------*/

/***************************************************************************//**
 * @brief Initializes the logging module.
 *
 * @return Nothing.
 ******************************************************************************/
extern void TXW51_LOG_Init(void);

/***************************************************************************//**
 * @brief Prints a logging message over the UART.
 *
 * @param[in] msg   The message to print.
 * @param[in] level The logging level.
 * @return Nothing.
 ******************************************************************************/
extern void TXW51_LOG_Print(const char *msg, enum TXW51_LOG_Level level);

/*----- Data -----------------------------------------------------------------*/

#endif /* TXW51_FRAMEWORK_UTILS_LOG_H_ */
