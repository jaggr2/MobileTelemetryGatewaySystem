var S = require('string'),
    mqtt = require('mqtt'),
    mqttrouter = require('mqtt-router'),
    SerialPort = require("serialport").SerialPort;

// settings
var deviceType = "BLED112Serial";
var sPort = "ttyACM1";
var serialSettings = {
    baudrate: 115200,
    flowControl: true
};
var deviceID = ''; // is currently set to the mac address of the first interface

// init serialport
var serial = null;

require('getmac').getMac(function(err,macAddress){
    if (err)  {
        console.log("unable to get MAC address");
        return
    }

    deviceID = S(macAddress).replaceAll(':','').s;

    console.log("DeviceID:",macAddress);

    // mqtt connect
    var client  = mqtt.connect('mqtt://web:1234@formula.xrj.ch');

    // enable the subscription router
    var router = mqttrouter.wrap(client);

    var publishError = function(message) {
        console.error("Error: ", message);
        client.publish('/' + deviceType + '/' + deviceID + '/' + sPort + '/error', message.toString());
    };

    // Serialport Handlers
    var reconnectSerial = function(callback) {

        serial = new SerialPort("/dev/" + sPort, serialSettings , false); // this is the openImmediately flag [default is true]

        serial.on('data', function (data) {
            console.log('publish received data: ', data);
            client.publish('/' + deviceType + '/' + deviceID + '/' + sPort + '/data', data);
        });

        serial.on('open', function () {
            console.log('opened serialport ', serial.path);
            client.publish('/' + deviceType + '/' + deviceID + '/' + sPort + '/open', '');
        });

        serial.on('close', function () {
            console.log('closed serialport ', serial.path);
            client.publish('/' + deviceType + '/' + deviceID + '/' + sPort + '/close', '');
        });

        serial.on('error', function(error) {
            publishError('Serial-Error ' + error.toString());
        });

        serial.open(function (error) {
            if ( error ) {
                publishError('failed to open serialport: ' + error.toString());
                return;
            }

            if(callback) callback(error);
        });
    };

    // MQTT Handlers
    router.subscribe('/' + deviceType + '/' + deviceID + '/' + sPort + '/write', function(topic, message){

        if(!serial.isOpen()) {

            reconnectSerial(function (err) {
                if(err) {
                    return;
                }

                //console.log('write message to serial', message);
                serial.write(message, function (err) {
                    if (err) {
                        publishError('failed to write on serialport: ' + err.toString());
                    }
                });
            });
        }
        else {
            // message is Buffer
            console.log('write message to serial', message);
            serial.write(message, function (err) {
                if (err) {
                    publishError('failed to write on serialport: ' + err.toString());
                }
            });
        }
    });

    router.subscribe('/' + deviceType + '/' + deviceID + '/' + sPort + '/ping', function(topic, message){
        var status = { isOpen: serial.isOpen(), ping: message };

        client.publish('/' + deviceType + '/' + deviceID + '/' + sPort + '/status', JSON.stringify(status));
    });

    router.subscribe('/' + deviceType + '/' + deviceID + '/all/ping', function(topic, message){
        var status = { isOpen: serial.isOpen(), ping: message };

        client.publish('/' + deviceType + '/' + deviceID + '/' + sPort + '/status', JSON.stringify(status));
    });

    router.subscribe('/' + deviceType + '/all/all/ping', function(topic, message){
        var status = { isOpen: serial.isOpen(), ping: message };

        client.publish('/' + deviceType + '/' + deviceID + '/' + sPort + '/status', JSON.stringify(status));
    });

    // start the gateway up
    client.on('connect', function () {
        reconnectSerial();
    });

});
