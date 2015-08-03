/**
 * Created by roger on 6/16/15.
 */


var S = require('string'),
    events = require('events'),
    VError = require('verror');

var tmp006Constants = {

    TMP006_B0: -0.0000294,
    TMP006_B1: -0.00000057,
    TMP006_B2: 0.00000000463,
    TMP006_C2: 13.4,
    TMP006_TREF: 298.15,
    TMP006_A2: -0.00001678,
    TMP006_A1: 0.00175,
    TMP006_S0: 6.4,  // * 10^-14

    TMP006_CONFIG:     0x02,

    TMP006_CFG_RESET:    0x8000,
    TMP006_CFG_MODEON:   0x7000,
    TMP006_CFG_1SAMPLE:  0x0000,
    TMP006_CFG_2SAMPLE:  0x0200,
    TMP006_CFG_4SAMPLE:  0x0400,
    TMP006_CFG_8SAMPLE:  0x0600,
    TMP006_CFG_16SAMPLE: 0x0800,
    TMP006_CFG_DRDYEN:   0x0100,
    TMP006_CFG_DRDY:     0x0080,

    TMP006_I2CADDR: 0x40,
    TMP006_MANID: 0xFE,
    TMP006_DEVID: 0xFF,

    TMP006_VOBJ:  0x0,
    TMP006_TAMB: 0x01
};

var Adafruit_TMP006 = function(i2cAddr, SampleRate) {

    var self = this;
    events.EventEmitter.call(this);

    self.i2cAddr = i2cAddr || tmp006Constants.TMP006_I2CADDR;
    self.sampleRate = SampleRate || tmp006Constants.TMP006_CFG_1SAMPLE;

    self.writeI2C = function(register, value, callback) {
        self.emit('writeI2C', self.i2cAddr, register, 16, value, callback);
    };

    self.readI2C = function(register, callback) {
        self.emit('readI2C', self.i2cAddr, register, 16, callback);
    };

    self.initTMP006 = function(callback) {

        self.writeI2C(tmp006Constants.TMP006_CONFIG, tmp006Constants.TMP006_CFG_MODEON | tmp006Constants.TMP006_CFG_DRDYEN | self.sampleRate, function(err) {
            if(err) return callback(new VError(err, "error while setting tmp06 config"));

            self.readI2C(tmp006Constants.TMP006_MANID, function(err, valueManId) {
                if(err) return callback(new VError(err, "error while reading TMP006_MANID"));

                console.log("TMP006_MANID", valueManId);
                //if (value != 0x5449) return false;

                self.readI2C(tmp006Constants.TMP006_DEVID, function(err, valueDevId) {
                    if(err) return callback(new VError(err, "error while reading TMP006_DEVID"));

                    console.log("TMP006_DEVID", valueDevId);
                    //if (did != 0x67) return false;

                    return callback(null);
                })
            })

        });
    };

    self.sleep = function(callback) {
        // Read the control register and update it so bits 12-14 are zero to enter sleep mode.
        self.readI2C(tmp006Constants.TMP006_CONFIG, function(err, readValue) {
            if(err) return callback(new VError(err, "error while reading tmp06 config"));

            readValue &= ~(tmp006Constants.TMP006_CFG_MODEON);

            self.writeI2C(tmp006Constants.TMP006_CONFIG, readValue, callback);
        });
    };

    self.wake = function(callback) {
        // Read the control register and update it so bits 12-14 are zero to enter sleep mode.
        self.readI2C(tmp006Constants.TMP006_CONFIG, function(err, readValue) {
            if(err) return callback(new VError(err, "error while reading tmp06 config"));

            readValue |= tmp006Constants.TMP006_CFG_MODEON;

            self.writeI2C(tmp006Constants.TMP006_CONFIG, readValue, callback);
        });
    };

    self.readDieTempC = function(callback) {

        self.readI2C(tmp006Constants.TMP006_TAMB, function(err, readValue) {
            if(err) return callback(new VError(err, "error while reading die temperature"));

            var v = readValue/4;
            v *= 0.03125;
            console.log('Raw Tambient', readValue, "( ", v ," *C)");

            //readValue >>= 2;
            return callback(null, v);

        });
    };

    self.readVoltage_uV = function(callback) {

        self.readI2C(tmp006Constants.TMP006_VOBJ, function(err, readValue) {
            if(err) return callback(new VError(err, "error while reading raw voltage"));

            var v = readValue;
            v *= 156.25;
            v /= 1000;
            console.log("Raw voltage ", readValue, " (", v, " uV)");

            return callback(null, v);

        });
    };

    self.readObjTempC = function(callback) {

        self.readDieTempC(function(err, dieTempC) {
            if(err) return callback(err);

            self.readVoltage_uV(function(err, rawVoltage) {
                if(err) return callback(err);

                var Tdie = dieTempC;
                var Vobj = rawVoltage;

                Vobj /= 1000; // uV -> mV
                Vobj /= 1000; // mV -> V

                Tdie += 273.15; // convert to kelvin

                var tdie_tref = Tdie - tmp006Constants.TMP006_TREF;
                var S = (1 + tmp006Constants.TMP006_A1*tdie_tref + tmp006Constants.TMP006_A2*tdie_tref*tdie_tref);
                S *= tmp006Constants.TMP006_S0;
                S /= 10000000;
                S /= 10000000;

                var Vos = tmp006Constants.TMP006_B0 + tmp006Constants.TMP006_B1 * tdie_tref + tmp006Constants.TMP006_B2 * tdie_tref * tdie_tref;

                var fVobj = (Vobj - Vos) + tmp006Constants.TMP006_C2*(Vobj-Vos)*(Vobj-Vos);

                var Tobj = sqrt(sqrt(Tdie * Tdie * Tdie * Tdie + fVobj/S));

                Tobj -= 273.15; // Kelvin -> *C

                callback(null, Tobj);
            });
        });
    }
};

Adafruit_TMP006.prototype.__proto__ = events.EventEmitter.prototype;

module.exports = exports = {
    Adafruit_TMP006: Adafruit_TMP006,
    tmp006Constants: tmp006Constants
};