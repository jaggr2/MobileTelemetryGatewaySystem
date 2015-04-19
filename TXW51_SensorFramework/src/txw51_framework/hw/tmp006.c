/*
 * tmp006.c
 *
 *  Created on: Apr 17, 2015
 *      Author: Pascal
 */


uint32_t TXW51_TMP006_GetTemperature(uint8_t *value)
{
    uint32_t err;
    uint8_t values[2];

    err = twi_master_transfer((uint8)((TMP006_I2CADDR << 1) & 0x01) , &values, 2, false);
    if (!err){
    //if (err != ERR_NONE) {
        TXW51_LOG_WARNING("[TMP006] Could not read temperature.");
        return err;
    }

    return ERR_NONE;
}
