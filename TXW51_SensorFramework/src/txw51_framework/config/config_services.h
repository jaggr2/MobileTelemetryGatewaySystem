/***************************************************************************//**
 * @brief   Configuration file for the Bluetooth Smart Services.
 *
 * @file    config_services.h
 * @version 1.0
 * @date    09.12.2014
 * @author  Daniel Meer
 *
 * @remark  Last Modifications:
 *          09.12.2014 meerd1 created
 *          10.04.2015 bohnp1 add contactless temperature service
 ******************************************************************************/

#ifndef TXW51_FRAMEWORK_CONFIG_CONFIG_SERVICES_H_
#define TXW51_FRAMEWORK_CONFIG_CONFIG_SERVICES_H_

/*----- Header-Files ---------------------------------------------------------*/

/*----- Macros ---------------------------------------------------------------*/

/******************************************************************************/
/* Common definitions.
 ******************************************************************************/
/**
 * @brief TXW51 base UUID address: 8EDFxxxx-67E5-DB83-F85B-A1E2AB1C9E7A
 */
#define TXW51_SERVICE_UUID_BASE	{{0x7A, 0x9E, 0x1C, 0xAB, 0xE2, 0xA1, 0x5B, 0xF8, 0x83, 0xDB, 0xE5, 0x67, 0x0, 0x0, 0xDF, 0x8E}}

#define TXW51_SERVICE_UUID_BLANK_SERVICE        ( 0x0000 )  /**< Blank service to represent all TXW51 services. Can be used in the advertising packet. */


/******************************************************************************/
/* Definitions for the Device Information Service.
 ******************************************************************************/
#define TXW51_SERV_DIS_UUID_SERVICE             ( 0x0100 )  /**< UUID address of the DIS service. */
#define TXW51_SERV_DIS_UUID_CHAR_MANUFACTURER   ( 0x0101 )  /**< UUID address of the manufacturer characteristic. */
#define TXW51_SERV_DIS_UUID_CHAR_MODEL          ( 0x0102 )  /**< UUID address of the model name characteristic. */
#define TXW51_SERV_DIS_UUID_CHAR_SERIAL         ( 0x0103 )  /**< UUID address of the serial number characteristic. */
#define TXW51_SERV_DIS_UUID_CHAR_HW_REV         ( 0x0104 )  /**< UUID address of the HW revision characteristic. */
#define TXW51_SERV_DIS_UUID_CHAR_FW_REV         ( 0x0105 )  /**< UUID address of the FW revision characteristic. */
#define TXW51_SERV_DIS_UUID_CHAR_DEVICE_NAME    ( 0x0106 )  /**< UUID address of the device name characteristic. */
#define TXW51_SERV_DIS_UUID_CHAR_SAVE_VALUES    ( 0x0107 )  /**< UUID address of the save values characteristic. */

#define TXW51_SERV_DIS_STRING_CHAR_MANUFACTURER "Manufacturer"      /**< User description string for the manufacturer characteristic. */
#define TXW51_SERV_DIS_STRING_CHAR_MODEL        "Model Name"        /**< User description string for the model name characteristic. */
#define TXW51_SERV_DIS_STRING_CHAR_SERIAL       "Serial Number"     /**< User description string for the serial number characteristic. */
#define TXW51_SERV_DIS_STRING_CHAR_HW_REV       "Hardware Version"  /**< User description string for the HW revision characteristic. */
#define TXW51_SERV_DIS_STRING_CHAR_FW_REV       "Firmware Version"  /**< User description string for the FW revision characteristic. */
#define TXW51_SERV_DIS_STRING_CHAR_DEVICE_NAME  "Device Name"       /**< User description string for the device name characteristic. */
#define TXW51_SERV_DIS_STRING_CHAR_SAVE_VALUES  "Save Values"       /**< User description string for the save values characteristic. */


/******************************************************************************/
/* Definitions for the LSM330 Service.
 ******************************************************************************/
#define SERVICE_LSM330_UUID_SERVICE	            ( 0x0200 )  /**< UUID address of the LSM330 service. */
#define SERVICE_LSM330_UUID_CHAR_ACC_EN         ( 0x0201 )  /**< UUID address of the acc enable characteristic. */
#define SERVICE_LSM330_UUID_CHAR_GYRO_EN        ( 0x0202 )  /**< UUID address of the gyro enable characteristic. */
#define SERVICE_LSM330_UUID_CHAR_TEMP_SAMPLE    ( 0x0203 )  /**< UUID address of the temperature sample characteristic. */
#define SERVICE_LSM330_UUID_CHAR_ACC_FSCALE     ( 0x0204 )  /**< UUID address of the acc full-scale characteristic. */
#define SERVICE_LSM330_UUID_CHAR_GYRO_FSCALE    ( 0x0205 )  /**< UUID address of the gyro full-scale characteristic. */
#define SERVICE_LSM330_UUID_CHAR_ACC_ODR        ( 0x0206 )  /**< UUID address of the acc ODR characteristic. */
#define SERVICE_LSM330_UUID_CHAR_GYRO_ODR       ( 0x0207 )  /**< UUID address of the gyro ODR characteristic. */
#define SERVICE_LSM330_UUID_CHAR_TRIGGER_VAL    ( 0x0208 )  /**< UUID address of the trigger value characteristic. */
#define SERVICE_LSM330_UUID_CHAR_TRIGGER_AXIS   ( 0x0209 )  /**< UUID address of the trigger axis characteristic. */

#define SERVICE_LSM330_STRING_CHAR_ACC_EN       "Turn on Accel"         /**< User description string for the acc enable characteristic. */
#define SERVICE_LSM330_STRING_CHAR_GYRO_EN      "Turn on Gyro"          /**< User description string for the gyro enable characteristic. */
#define SERVICE_LSM330_STRING_CHAR_TEMP_SAMPLE  "Temperature Sample"    /**< User description string for the temperature sample characteristic. */
#define SERVICE_LSM330_STRING_CHAR_ACC_FSCALE   "Accel Full-Scale"      /**< User description string for the acc full-scale characteristic. */
#define SERVICE_LSM330_STRING_CHAR_GYRO_FSCALE  "Gyro Full-Scale"       /**< User description string for the gyro full-scale characteristic. */
#define SERVICE_LSM330_STRING_CHAR_ACC_ODR      "Accel ODR"             /**< User description string for the acc ODR characteristic. */
#define SERVICE_LSM330_STRING_CHAR_GYRO_ODR     "Gyro ODR"              /**< User description string for the gyro ODR characteristic. */
#define SERVICE_LSM330_STRING_CHAR_TRIGGER_VAL  "Trigger Value"         /**< User description string for the trigger value characteristic. */
#define SERVICE_LSM330_STRING_CHAR_TRIGGER_AXIS "Trigger Axis"          /**< User description string for the trigger axis characteristic. */


/******************************************************************************/
/* Definitions for the Measurement Service.
 ******************************************************************************/
#define TXW51_SERV_MEASURE_UUID_SERVICE         ( 0x0300 )  /**< UUID address of the Measurement service. */
#define TXW51_SERV_MEASURE_UUID_CHAR_START      ( 0x0301 )  /**< UUID address of the start characteristic. */
#define TXW51_SERV_MEASURE_UUID_CHAR_STOP       ( 0x0302 )  /**< UUID address of the stop characteristic. */
#define TXW51_SERV_MEASURE_UUID_CHAR_DURATION   ( 0x0303 )  /**< UUID address of the duration characteristic. */
#define TXW51_SERV_MEASURE_UUID_CHAR_DATASTRAM  ( 0x0304 )  /**< UUID address of the data stream characteristic. */

#define TXW51_SERV_MEASURE_STRING_CHAR_START        "Start Measurement"     /**< User description string for the start characteristic. */
#define TXW51_SERV_MEASURE_STRING_CHAR_STOP         "Stop Measurement"      /**< User description string for the stop characteristic. */
#define TXW51_SERV_MEASURE_STRING_CHAR_DURATION     "Set Measur. Duration"  /**< User description string for the duration characteristic. */
#define TXW51_SERV_MEASURE_STRING_CHAR_DATASTREAM   "Read Data from Sensor" /**< User description string for the data stream characteristic. */

/******************************************************************************/
/* Definitions for the contactless temperature Service.
 ******************************************************************************/
#define SERVICE_TEMP_CONTACTLESS_UUID_SERVICE         	( 0x0400 )  /**< UUID address of the Measurement service. */
#define SERVICE_TEMP_CONTACTLESS_UUID_CHAR_TEMP_SAMPLE  ( 0x0401 )  /**< UUID address of the temperature sample characteristic. */

#define SERVICE_TEMP_CONTACTLESS_STRING_CHAR_TEMP_SAMPLE  "Temperature Sample"    /**< User description string for the temperature sample characteristic. */

/******************************************************************************/
/* Definitions for the I2C Service.
 ******************************************************************************/
#define SERVICE_I2C_UUID_SERVICE         	  	( 0x0500 )  /**< UUID address of the Measurement service. */
#define SERVICE_I2C_UUID_CHAR_ADDRESS			( 0x0501 )  /**< UUID address of the I2C Address characteristic. */
#define SERVICE_I2C_UUID_CHAR_REGISTER			( 0x0502 )  /**< UUID address of the I2C Register characteristic. */
#define SERVICE_I2C_UUID_CHAR_LENGTH			( 0x0503 )  /**< UUID address of the I2C Register characteristic. */
#define SERVICE_I2C_UUID_CHAR_REGISTER_VALUE  	( 0x0504 )  /**< UUID address of the I2C Value characteristic. */

#define SERVICE_I2C_STRING_CHAR_ADDRESS  		"address"    		/**< User description string for the I2C address characteristic. */
#define SERVICE_I2C_STRING_CHAR_REGISTER  		"register"    		/**< User description string for the I2C register characteristic. */
#define SERVICE_I2C_STRING_CHAR_LENGTH  		"length"    		/**< User description string for the I2C register characteristic. */
#define SERVICE_I2C_STRING_CHAR_REGISTER_VALUE  "Register Value"    /**< User description string for the I2C Register Value characteristic. */

/*----- Data types -----------------------------------------------------------*/

/*----- Function prototypes --------------------------------------------------*/

/*----- Data -----------------------------------------------------------------*/

#endif /* TXW51_FRAMEWORK_CONFIG_CONFIG_SERVICES_H_ */
