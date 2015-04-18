var S = require('string'),
    mqtt = require('mqtt'),
    mqttrouter = require('mqtt-router'),
    SerialPort = require("serialport").SerialPort;

// settings
var deviceType = "BLED112Serial";
var sPort = "ttyACM0";
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

    // MQTT Handlers
    router.subscribe('/sming/measurement', function(topic, message){

        var sample = JSON.parse(message);
        console.log('measurement:', sample.point);
    });

    // start the gateway up
    client.on('connect', function () {
        console.log('connected');
    });

});
