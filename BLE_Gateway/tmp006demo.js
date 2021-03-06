var S = require('string'),
    mqtt = require('mqtt'),
    mqttrouter = require('mqtt-router'),
    bg = require('bglib'),
    libCommandQueue = require('commandqueue'),
    bgCommand = libCommandQueue.bluegigaCommand,
    commandQueue = libCommandQueue.commandQueue,
    SerialPort = require("serialport").SerialPort,
    events = require('events'),
    VError = require('verror'),
    AdafruitTMP006 = require('./adafruit_tmp006');

var sPort = "/dev/ttyACM0";

if(process.argv.length > 2)
{
    sPort = process.argv[2];
    console.log(sPort);
}

var serialSettings = {
    baudrate: 115200,
    flowControl: true
};

require('buffertools').extend();

var bglib = new bg();
var deviceType = "BLED112Serial";
var agentID = "agent01";
var serial = null;

/* descriptor management */
var descriptorDefinitions = {
    DEVICE_INFO_SERVICE           : "8EDF0100-67E5-DB83-F85B-A1E2AB1C9E7A",
    DEVICE_INFO_CHAR_MANUFACTURER : "8EDF0101-67E5-DB83-F85B-A1E2AB1C9E7A",
    DEVICE_INFO_CHAR_MODEL        : "8EDF0102-67E5-DB83-F85B-A1E2AB1C9E7A",
    DEVICE_INFO_CHAR_SERIAL       : "8EDF0103-67E5-DB83-F85B-A1E2AB1C9E7A",
    DEVICE_INFO_CHAR_HW_REV       : "8EDF0104-67E5-DB83-F85B-A1E2AB1C9E7A",
    DEVICE_INFO_CHAR_FW_REV       : "8EDF0105-67E5-DB83-F85B-A1E2AB1C9E7A",
    DEVICE_INFO_CHAR_DEVICE_NAME  : "8EDF0106-67E5-DB83-F85B-A1E2AB1C9E7A",
    DEVICE_INFO_CHAR_SAVE_VALUES  : "8EDF0107-67E5-DB83-F85B-A1E2AB1C9E7A",

    LSM330_SERVICE           : "8EDF0200-67E5-DB83-F85B-A1E2AB1C9E7A",
    LSM330_CHAR_ACC_EN       : "8EDF0201-67E5-DB83-F85B-A1E2AB1C9E7A",
    LSM330_CHAR_GYRO_EN      : "8EDF0202-67E5-DB83-F85B-A1E2AB1C9E7A",
    LSM330_CHAR_TEMP_SAMPLE  : "8EDF0203-67E5-DB83-F85B-A1E2AB1C9E7A",
    LSM330_CHAR_ACC_FSCALE   : "8EDF0204-67E5-DB83-F85B-A1E2AB1C9E7A",
    LSM330_CHAR_GYRO_FSCALE  : "8EDF0205-67E5-DB83-F85B-A1E2AB1C9E7A",
    LSM330_CHAR_ACC_ODR      : "8EDF0206-67E5-DB83-F85B-A1E2AB1C9E7A",
    LSM330_CHAR_GYRO_ODR     : "8EDF0207-67E5-DB83-F85B-A1E2AB1C9E7A",
    LSM330_CHAR_TRIGGER_VAL  : "8EDF0208-67E5-DB83-F85B-A1E2AB1C9E7A",
    LSM330_CHAR_TRIGGER_AXIS : "8EDF0209-67E5-DB83-F85B-A1E2AB1C9E7A",

    MEASURE_SERVICE         : "8EDF0300-67E5-DB83-F85B-A1E2AB1C9E7A",
    MEASURE_CHAR_START      : "8EDF0301-67E5-DB83-F85B-A1E2AB1C9E7A",
    MEASURE_CHAR_STOP       : "8EDF0302-67E5-DB83-F85B-A1E2AB1C9E7A",
    MEASURE_CHAR_DURATION   : "8EDF0303-67E5-DB83-F85B-A1E2AB1C9E7A",
    MEASURE_CHAR_DATASTREAM : "8EDF0304-67E5-DB83-F85B-A1E2AB1C9E7A",

    I2C_SERVICE             : "8EDF0500-67E5-DB83-F85B-A1E2AB1C9E7A",
    I2C_CHAR_DEVICE_ADDRESS : "8EDF0501-67E5-DB83-F85B-A1E2AB1C9E7A",
    I2C_CHAR_DEVICE_REGISTER: "8EDF0502-67E5-DB83-F85B-A1E2AB1C9E7A",
    I2C_CHAR_READ_LENGTH    : "8EDF0503-67E5-DB83-F85B-A1E2AB1C9E7A",
    I2C_CHAR_VALUE          : "8EDF0504-67E5-DB83-F85B-A1E2AB1C9E7A"
    };

var getUUIDBuffer = function(UUID) {
    var uuidHex = S(UUID).replaceAll('-','').toLowerCase();

    return new Buffer(uuidHex.toString(), 'hex').reverse();
};

var Gatt = function(descriptorDefinitionList) {

    var self = this;

    self.attributeList = [];

    for(var key in descriptorDefinitionList) {
        if (descriptorDefinitionList.hasOwnProperty(key)) {
            self.attributeList.push({
                name: key,
                uuid: getUUIDBuffer(descriptorDefinitionList[key]),
                handle: 0,
                lastValue: null })
        }
    }

    self.getAttributeByName = function(name) {

        for(var i = 0;  i < self.attributeList.length; i++) {

            if(self.attributeList[i].name == name) {
                return self.attributeList[i];
            }
        }

        return null;
    };

    self.getAttributeByUUID = function(uuid) {

        for(var i = 0;  i < self.attributeList.length; i++) {

            if(self.attributeList[i].uuid.equals(uuid)) {
                return self.attributeList[i];
            }
        }

        return null;
    };

    self.getAttributeByHandle = function(handle) {

        for(var i = 0;  i < self.attributeList.length; i++) {

            if(self.attributeList[i].handle == handle) {
                return self.attributeList[i];
            }
        }

        return null;
    };

};


var connectionStates = {
    DISCONNECTED: 1,
    CONNECTING: 2,
    CONNECTED: 3,
    DISCONNECTING:   4
};

var getConnectionStateName = function(theState) {
    for(var key in connectionStates) {
        if (connectionStates.hasOwnProperty(key)) {

            if(connectionStates[key] == theState) {
                return key;
            }
        }
    }
    return "UNKNOWN";
};

var btRemoteDevice = function(macBuffer, gatewaysCommandqueue) {

    var self = this;
    events.EventEmitter.call(this);

    self.connectionId = null;
    self.macBuffer = macBuffer;
    self.mac = macBuffer.toString('hex');
    self.connectionState = connectionStates.DISCONNECTED;
    self.pollingInterval = null;
    self.commandQueue = gatewaysCommandqueue;
    self.gatt = null;
    self.ccidHandle = null;
    self.asyncGATTHandle = null;
    self.connectionTimer = null;

    self.setPollingInterval = function(theInterval) {
        self.pollingInterval = theInterval;
    };

    self.changeState = function (err, newState) {

        var oldState = self.connectionState;
        self.connectionState = newState;

        switch(newState) {
            case connectionStates.CONNECTED:
                clearTimeout(self.connectionTimer);
                break;

            case connectionStates.DISCONNECTED:
                self.connectionId = null;
                break;
        }

        self.emit('connectionStateChange', err, newState, oldState);
    };

    self.disconnect = function(callback) {

        if(self.connectionState !== connectionStates.CONNECTED) {
            return callback(null);
        }

        self.changeState(null, connectionStates.DISCONNECTING);
        clearInterval(self.pollingInterval);

        self.commandQueue.addCommand(new bgCommand.bgCommand(bg.api.connectionDisconnect, [self.connectionId]), 10000, function (err) { //, command, result
            if(err) return callback(new VError(err, 'Error while disconecting btRemoteDevice %s', self.mac));

            return callback(null);
        });
    };

    self.connect = function(callback) {

        self.changeState(null, connectionStates.CONNECTING);

        // direct connect
        self.commandQueue.addCommand(new bgCommand.bgCommand(bg.api.gapConnectDirect, [self.macBuffer, 1, 60, 75, 700, 9]), 10000, function(err, command, result) {

            if(err) return callback(new VError(err, 'Error while connecting btRemoteDevice %s', self.mac));

            console.log("Start connecting to ", self.mac, ". Result:", result );

            self.connectionId = result.connection_handle;
            self.connectionTimer = setTimeout(function(theClient) {
                console.error("got no connection within 10s. Disconnect...");
                theClient.disconnect(function(err) {

                    if(err) return console.error(err);

                });

            }, 10000, self);

            return callback(null);
        });
    };

    self.readGATTIndex = function (descriptorDefinitionList, forceReload, callback) {

        if(self.connectionState !== connectionStates.CONNECTED) return callback(new VError('can not read GATT when btRemoteDevice is not connected. Current state: %s', getConnectionStateName(self.connectionState)));

        if(self.gatt && !forceReload) {
            return callback(null);
        }

        if(!descriptorDefinitionList) return callback(new VError('readEntireGATT() invalid parameter descriptorDefinitions'));

        console.log("btRemoteDevice ", self.mac, " reload GATT handle index from device...");

        self.gatt = new Gatt(descriptorDefinitionList);

        self.commandQueue.addCommand(new bgCommand.bgCommand(bg.api.attClientFindInformation, [self.connectionId, 1, 0xffff]), 10000, function(err, command, result) {

            if(err) return callback(new VError(err, 'Error while reading GATT of btRemoteDevice %s', self.mac));

            return callback(null);
        });
    };

    self.enableGattListener = function(callback) {

        if(self.connectionState !== connectionStates.CONNECTED) return callback(new VError('can not enable GATT Listener when btRemoteDevice is not connected'));

        if(!self.ccidHandle ) return callback(new VError("no ccidHandle available!"));

        self.commandQueue.addCommand(new bgCommand.bgCommand(bg.api.attClientAttributeWrite, [self.connectionId, self.ccidHandle, new Buffer([0x01, 0x00])]), 30000, function(err) {

            if(err) return callback(new VError(err, 'Error while activating CCID of btRemoteDevice %s', self.mac));

            return callback(null);

        });
    };

    self.writeGATTAttribute = function(name, newValueBuffer, callback) {

        var descriptor = self.gatt.getAttributeByName(name);

        if(!descriptor) return callback(new VError('unknown descriptor %s', name));

        if(!(descriptor.handle > 0)) return callback(new VError('descriptor %s has no handle, read readGATTIndex first', name));

        self.commandQueue.addCommand(new bgCommand.bgCommand(bg.api.attClientAttributeWrite, [self.connectionId, descriptor.handle, newValueBuffer]), 30000, callback);

    };

    self.readGATTAttribut = function(name, callback) {

        var descriptor = self.gatt.getAttributeByName(name);

        if(!descriptor) return callback(new VError('unknown descriptor %s', name));

        if(!(descriptor.handle > 0)) return callback(new VError('descriptor %s has no handle, read readGATTIndex first', name));

        self.commandQueue.addCommand(new bgCommand.bgCommand(bg.api.attClientReadByHandle, [self.connectionId, descriptor.handle]), 30000, callback);
    };

    self.handleConnectionEvent = function(packet) {

        if(packet.response.reason) {
            // an error happened, if we are in disconnecting state, this is expected
            var isExpected = self.connectionState === connectionStates.DISCONNECTING;
            var wereWeConnected = self.connectionState === connectionStates.CONNECTING;

            self.changeState((!isExpected ? packet.response.reason.message : null), connectionStates.DISCONNECTED);

            var reconnectInS = 2;
            if( !isExpected && wereWeConnected ) {
                console.log("Reconnect to device ", self.mac, " due to connection loss in ", reconnectInS, "s")
            }

            setTimeout(self.connect, reconnectInS * 1000, function(err) {
                if(err) return console.error("Reconnect to device ", self.mac, " failed: ", err);
            });
        }
        else {
            // conection state changed

            if( ( packet.response.flags & bglib.ConnectionStatus.connection_connected ) && ( packet.response.flags & bglib.ConnectionStatus.connection_completed ) ) {
                self.changeState(null, connectionStates.CONNECTED);
            }
            else {

                var flags = '';
                flags += ( packet.response.flags & bglib.ConnectionStatus.connection_connected ) ? 'connected,' : '';
                flags += ( packet.response.flags & bglib.ConnectionStatus.connection_encrypted ) ? 'encrypted,' : '';
                flags += ( packet.response.flags & bglib.ConnectionStatus.connection_completed) ? 'completed,' : '';
                flags += ( packet.response.flags & bglib.ConnectionStatus.connection_parameters_change) ? 'parameters_change,' : '';

                var adresstype = '';
                switch(packet.response.address_type){
                    case bglib.BluetoothAddressTypes.gap_address_type_public:
                        adresstype = 'public';
                        break;
                    case bglib.BluetoothAddressTypes.gap_address_type_random:
                        adresstype = 'random';
                        break;
                }

                console.log("Device ", self.mac , " changed connection status: ", flags, " address:", packet.response.address.toString('hex'), ", adr-type:", adresstype, ", conn_interval:", packet.response.conn_interval, ", timeout:", packet.response.timeout, ", latency:", packet.response.latency, ", bonding:", packet.response.bonding);
            }
        }
    };

    self.handleAttributeEvent = function(packet) { // packet.packet.cClass ist immer 4

        switch(packet.packet.cID) {
            case 5:

                if(packet.response.atthandle) {
                    var attributData = self.gatt.getAttributeByHandle(packet.response.atthandle);
                    attributData.lastValue = packet.response.value;
                    //console.log("btRemoteDevice ", self.mac, " got value of attribut ", attributData.name ,":", attributData.lastValue);

                    self.emit('attributeValueReceived', attributData);
                }
                else {
                    console.error("error on receiving attribut value");
                }

                break;

            case 4:
                var ccidUuid = new Buffer([0x02, 0x29]);

                if(packet.response.uuid.equals(ccidUuid)) {
                    self.ccidHandle = packet.response.chrhandle;
                }

                var attr = self.gatt.getAttributeByUUID(packet.response.uuid);

                if(attr) {
                    console.log('Device ', self.mac, ' updated handle information for uuid ', attr.name);
                    attr.handle = packet.response.chrhandle;
                }
                else {
                    attr = {
                        name: packet.response.uuid.reverse().toString('hex'),
                        uuid: packet.response.uuid,
                        handle: packet.response.chrhandle,
                        lastValue: null };

                    self.gatt.attributeList.push(attr);

                    console.log('Device ', self.mac, ' updated handle information for new uuid ', attr.name);
                }
                break;

            case 1:
                console.error('Device ', self.mac, ': Unhandled attribut completed event: ', packet.response);
                break;

            default:
                console.log(self.mac, ' got unknown attribut class packet', packet);
        }
    };

};
btRemoteDevice.prototype.__proto__ = events.EventEmitter.prototype;



var linkLayerStates = {
    STANDBY: 1,
    SCANNING: 2,
    INITIATINGCONNTECTION: 3,
    CONNECTED:   4,
    ADVERTISING: 5
};

var getLinkLayerStateName = function(theState) {
    for(var key in linkLayerStates) {
        if (linkLayerStates.hasOwnProperty(key)) {

            if(linkLayerStates[key] == theState) {
                return key;
            }
        }
    }
    return "UNKNOWN";
};

var btDevice = function(name) {

    var self = this;
    events.EventEmitter.call(this);

    self.name = name;
    self.lastStatusMessage = new Date();
    self.linkLayerState = linkLayerStates.STANDBY;
    self.commandQueue = new commandQueue({}); // { eventParams: '/' + deviceType + '/' + params.device + '/' + params.port + '/write' }

    self.clientList = [];

    self.getClientByMAC = function(mac) {
        for(var i = 0; i < self.clientList.length; i++) {
            if(self.clientList[i].mac == mac) {
                return self.clientList[i];
            }
        }
        return null;
    };

    self.getClientByConnectionHandle = function(connHandle) {
        for(var i = 0; i < self.clientList.length; i++) {
            if(self.clientList[i].connectionId == connHandle) {
                return self.clientList[i];
            }
        }
        return null;
    };

    self.changeLinkLayerState = function (err, newState) {

        var oldState = self.linkLayerState;
        self.linkLayerState = newState;

        self.emit('linkLayerStateChange', err, newState, oldState);
    };

    self.init = function(callback) {
        self.commandQueue.addCommand(new bgCommand.bgCommand(bg.api.systemHello, null), 1000, function(err, command, result) {

            if(err) return callback(new VError(err, "Error on SystemHello, seems to be no working bluegiga device"));

            callback(null);
        });
    };

    self.reset = function(callback) {
        self.commandQueue.addCommand(new bgCommand.bgCommand(bg.api.systemReset, [0]), 5000, function(err, command, result) {

            if(err) return callback(new VError(err, "Error on reset"));

            self.changeLinkLayerState(null, linkLayerStates.STANDBY);
            self.clientList = [];
            callback(null);
        });
    };

    self.disconnectAll = function() {

        self.commandQueue.quitAllCommands("disconnectAll");

        for (var i = 0; i < self.clientList.length; i++) {

            if (self.clientList[i].connectionState === connectionStates.CONNECTED) {
                self.clientList[i].disconnect(function (err) {
                    if (err) return console.error(err);

                });
            }
        }

        self.changeLinkLayerState(null, linkLayerStates.STANDBY);
    };


    self.stopAdvertising = function() {
        // stop advertising if we are advertising already
        self.commandQueue.addCommand(new bgCommand.bgCommand(bg.api.gapSetMode, [0, 0]), 10000, function (err, command, result) {

            if (err) {
                return console.error("gapSetMode error", err);
            }
            console.log("gapSetMode result", result);
            self.changeLinkLayerState(null, linkLayerStates.STANDBY);
        });
    };

    self.stopScanning = function() {
    // stop scanning if we are scanning already
        self.commandQueue.addCommand(new bgCommand.bgCommand(bg.api.gapEndProcedure, null), 10000, function(err, command, result) {

            if(err) {
                return console.error("gapEndProcedure error", err);
            }
            console.log("gapEndProcedure result", result);
            self.changeLinkLayerState(null, linkLayerStates.STANDBY);
        });

    };

    self.startScanning = function() {

        mqttClient.publish('/sming/discover', 'BlueGiga' + self.name + ' starts discovering smings');

        // set scan parameters
        self.commandQueue.addCommand(new bgCommand.bgCommand(bg.api.gapSetScanParameters, [0xC8, 0xC8, 0]), 10000, function(err, command, result) {

            if(err) {
                return console.log("gapSetMode error", err);
            }
            console.log("gapSetMode result", result);

            // start scanning now
            self.commandQueue.addCommand(new bgCommand.bgCommand(bg.api.gapDiscover, [1]), 10000, function(err, command, result) {

                if(err) {
                    return console.log("gapDiscover error", err);
                }
                console.log("gapDiscover result", result);
                self.changeLinkLayerState(null, linkLayerStates.SCANNING);
            });
        });
    };

    self.commandQueue.on('asyncPacket', function(packet, eventParams) {

        bgClass = { System : 0,
            PersistentStore : 1,
            AttributeDatabase : 2,
            Connection : 3,
            AttributeClient : 4,
            SecurityManager : 5,
            GenericAccessProfile : 6,
            Hardware : 7,
            Test : 8,
            DFU : 9 };

        //console.log(packet);

        if(packet.responseType == 'Event') {

            var theRemoteDevice = null;
            if(packet.response.connection !== undefined) {
                theRemoteDevice = self.getClientByConnectionHandle(packet.response.connection);

                if(theRemoteDevice == null) {
                    return console.error("there is no client matching connection handle ", packet.response.connection);
                }
            }

            switch(packet.packet.cClass) {

                case bgClass.Connection:
                    if(!theRemoteDevice) return console.error("no device registered for connection ", packet.response.connection);

                    theRemoteDevice.handleConnectionEvent(packet);
                    break;

                case bgClass.AttributeClient:
                    if(!theRemoteDevice) return console.error("no device registered for connection ", packet.response.connection);

                    theRemoteDevice.handleAttributeEvent(packet);
                    break;

                case bgClass.GenericAccessProfile:

                    if(packet.packet.cID == 0) { // Advertisment packet

                        var remoteDevice = self.getClientByMAC(packet.response.sender.toString('hex'));

                        if (remoteDevice == null) {

                            remoteDevice = new btRemoteDevice(packet.response.sender, self.commandQueue);
                            self.clientList.push(remoteDevice);


                            var btData = packet.response.data;
                            for (var i = 0; i < btData.length; i++) {

                                switch (btData[i].typeFlag) {
                                    case 9: // Complete local name
                                        remoteDevice.btLocalName = btData[i].data;

                                }

                            }

                            remoteDevice.lastRssi = packet.response.rssi;
                            remoteDevice.lastSeen = new Date();

                            self.emit("newRemoteDeviceFound", remoteDevice);
                        }
                        else {
                            remoteDevice.lastRssi = packet.response.rssi;
                            remoteDevice.lastSeen = new Date();
                        }

                    }
                    break;

                default:
                    console.log("unimplemented class", packet.packet.cClass, packet.response);
            }
        }
        else {
            console.log("unknown data: ", packet);
        }
    });
};
btDevice.prototype.__proto__ = events.EventEmitter.prototype;


var StartupResetDone = true;
var globalTimer = null;
var mqttClient = null;

require('getmac').getMac(function(err,macAddress){
    if (err)  {
        console.log("unable to get MAC address");
        return
    }

    macAddress = S(macAddress).replaceAll(':','').s;

    console.log("DeviceID:",macAddress);


    // mqtt connect
    mqttClient = mqtt.connect('mqtt://web:1234@formula.xrj.ch');

    // enable the subscription router
    var router = mqttrouter.wrap(mqttClient);

    // MQTT Handlers
    /*
    router.subscribe('/' + deviceType + '/+:device/+:port/data', function(topic, message, params){

        var gwID = params.device + "/" + params.port;
        var gateway = getGatewayByName(gwID);

        if(gateway) {
            //console.log("forward message to queue", message);
            bgCommand.bgProcessData(message, gateway.commandQueue);
        }

    });
    */

    // Serial-Port Handling
    serial = new SerialPort(sPort, serialSettings , false); // this is the openImmediately flag [default is true]

    serial.on('error', function(error) {
        console.log('Serial-Error ', error);
    });

    serial.on('open', function () {
        console.log('opened serialport ', serial.path);
//            client.publish('/' + deviceType + '/' + deviceID + '/' + sPort + '/open', '');

        serial.btDevice = new btDevice(sPort);

        serial.btDevice.commandQueue.on('write', function(data, eventParams) {
            //console.log("publish to ", eventParams, data);
            //client.publish(eventParams, data);

            if(!serial.isOpen()) {
                console.error('failed to write on serialport: Serialport is not open');
                /* reconnectSerial(function (err) {
                    if(err) {
                        return;
                    }

                    //console.log('write message to serial', message);
                    serial.write(data, function (err) {
                        if (err) {
                            console.error('failed to write on serialport: ' + err.toString());
                        }
                    });
                }); */
            }
            else {
                // message is Buffer
                //console.log('write message to serial', data);
                serial.write(data, function (err) {
                    if (err) {
                        console.error('failed to write on serialport: ' + err.toString());
                    }
                });
            }

        });

        serial.on('data', function (data) {
            bgCommand.bgProcessData(data, serial.btDevice.commandQueue);
        });

        serial.on('close', function () {

            console.log('closed serialport ', serial.path);
            serial.btDevice.commandQueue.quitAllCommands('serial port closed');
            //            client.publish('/' + deviceType + '/' + deviceID + '/' + sPort + '/close', '');
        });

        serial.btDevice.init(function(err) {
            if(err) console.error("Error initialising BlueGiga: ", err);

            console.log("BlueGiga Device connection is ok!");
            serial.emit('btDeviceInit');
        });
    });


    serial.on('btDeviceInit', function() {

        if(!StartupResetDone) {

            StartupResetDone = true;

            serial.btDevice.reset(function() {

                console.log("reset ok, reinit Bluegiga Dongle");

                serial.open(function(err) {
                    if (err) return console.error(err);
                });
            });

        }
        else {
            decorateSmingFunctionality(serial.btDevice);

            serial.btDevice.stopScanning();

            console.log("Start Scanning for devices...");
            serial.btDevice.startScanning();
        }
    });


    // start the gateway up
    mqttClient.on('connect', function () {
        console.log("mqtt connected, connect serial...");
        serial.open(function(err) {
            if(err) return console.error(err);
        });
    });

});

function decorateSmingFunctionality(gateway) {


    gateway.on("linkLayerStateChange", function(err, newState) {
        if(err) {
            console.error("btDevice State is now ", getLinkLayerStateName(newState), " due to error ", err);
            mqttClient.publish('/sming/blueGigaDongle/linkLayerState', getLinkLayerStateName(newState) + " due to error " + err)
        }
        else {
            console.log("btDevice State is now ", getLinkLayerStateName(newState));
            mqttClient.publish('/sming/blueGigaDongle/linkLayerState', getLinkLayerStateName(newState) + " ");
        }
    });

    gateway.on("newRemoteDeviceFound", function(newRemoteDevice) {

        if (newRemoteDevice.btLocalName == 'TXW51') {

            newRemoteDevice.isSming = true;

            console.log('Found Sming ', newRemoteDevice.mac, newRemoteDevice.lastRssi);
            mqttClient.publish('/sming/found', newRemoteDevice.mac + " " + newRemoteDevice.lastRssi);


            newRemoteDevice.on('connectionStateChange', function(err, newState) {

                var theRemoteDevice = newRemoteDevice;

                if(err) {
                    console.error("Device ", theRemoteDevice.mac, " is now ", getConnectionStateName(newState), " due to error ", err);
                    mqttClient.publish('/sming/' + theRemoteDevice.mac + '/connectionState', getConnectionStateName(newState) + " due to error " + err)
                }
                else {
                    console.log("Device ", theRemoteDevice.mac, " is now ", getConnectionStateName(newState));
                    mqttClient.publish('/sming/' + theRemoteDevice.mac + '/connectionState', getConnectionStateName(newState) + " ");
                }

                switch(newState) {

                    case connectionStates.CONNECTED:

                        theRemoteDevice.readGATTIndex(descriptorDefinitions, true, function (err) { // ;packet.response.connection
                            if (err) return console.error(err);

                            console.log("INFO: connect next client and start measuring!");


                            newRemoteDevice.smingStartMeasuring(function (err) {

                                gateway.connectNextClient();

                                if (err) {
                                    setTimeout(newRemoteDevice.smingStartMeasuring, 2000, function (err) {
                                        if (err) console.log(err)
                                    });

                                    return console.error(err);
                                }

                                console.log("Device ", newRemoteDevice.mac ," has started measuring!");
                            });
                        });
                        break;
                }
            });

            newRemoteDevice.on('attributeValueReceived', function(attribut) {

                var theRemoteDevice = this;

                if(attribut.name == "MEASURE_CHAR_DATASTREAM") {

                    if (!attribut.lastValue || !Buffer.isBuffer(attribut.lastValue)) {
                        return console.error("Device ", theRemoteDevice.mac, ": Unknown Measure Data: ", attribut.lastValue)
                    }

                    var buffer = attribut.lastValue;
                    var controllByte = buffer.readInt8(0);
                    var numberOfSamples = controllByte & 0x0F;
                    var validAxis = (controllByte >> 4) & 0x07;
                    var accOrGyro = (controllByte >> 7) & 0x01;
                    var sequenceNumber = buffer.readInt8(1);

                    //console.log("Measure Event ", numberOfSamples, validAxis, accOrGyro);

                    //var samples = [];

                    // Decode data points.
                    for (var j = 0; j < numberOfSamples; j++) {

                        var index = 2 + j * 6;

                        var sample = { sequenceNumber: sequenceNumber,
                            point: [  buffer.readInt16LE(index) * theRemoteDevice.accFscaleMultiplikator, // X-Achse
                                    buffer.readInt16LE(index + 2) * theRemoteDevice.accFscaleMultiplikator,    // Y-Achse
                                    buffer.readInt16LE(index + 4) * theRemoteDevice.accFscaleMultiplikator ],   // Z-Achse
                            accOrGyro: accOrGyro
                        };

                        if(!theRemoteDevice.measuredCount) {
                            console.log("Device ", theRemoteDevice.mac , " received first measurement data");
                            theRemoteDevice.measuredCount = 1;
                        }
                        else {
                            theRemoteDevice.measuredCount += 1;
                        }

                        mqttClient.publish('/sming/measurement', JSON.stringify(sample));

                        var fs = require('fs');
                        fs.appendFile("data.txt", theRemoteDevice.mac + "," + (new Date()).getTime() + "," + sample.point[0] + "," + sample.point[1] + "," + sample.point[2] + "\n", function (err) {
                            if (err) {
                                return console.log(err);
                            }
                        });
                        //samples.push(sample);
                    }
                }
            });

            newRemoteDevice.smingStartMeasuring = function(callback) {

                var theRemoteDevice = newRemoteDevice;


                theRemoteDevice.tmp006 = new AdafruitTMP006.Adafruit_TMP006(null, null);

                theRemoteDevice.tmp006.on('writeI2C', function(i2cAddr, register, writeSize, value, callback) {

                    console.log('writeI2C', i2cAddr, register, writeSize, value)
,
                    theRemoteDevice.writeGATTAttribute('I2C_CHAR_DEVICE_ADDRESS', new Buffer([i2cAddr]), function(err, command, result) {

                        if (err) {
                            return callback(new VError(err, "btRemoteDevice %s write I2C_CHAR_DEVICE_ADDRESS error", theRemoteDevice.mac));
                        }

                        theRemoteDevice.writeGATTAttribute('I2C_CHAR_DEVICE_REGISTER', new Buffer([register]), function(err, command, result) {

                            if (err) {
                                return callback(new VError(err, "btRemoteDevice %s write I2C_CHAR_DEVICE_REGISTER error", theRemoteDevice.mac));
                            }

                            theRemoteDevice.writeGATTAttribute('I2C_CHAR_READ_LENGTH', new Buffer([writeSize]), function(err, command, result) {

                                if (err) {
                                    return callback(new VError(err, "btRemoteDevice %s write I2C_CHAR_READ_LENGTH error", theRemoteDevice.mac));
                                }

                                theRemoteDevice.writeGATTAttribute('I2C_CHAR_VALUE', new Buffer([value]), function(err, command, result) {

                                    if (err) {
                                        return callback(new VError(err, "btRemoteDevice %s write I2C_CHAR_VALUE error", theRemoteDevice.mac));
                                    }

                                    console.log('writeI2C success');
                                    callback(null);
                                });
                            });
                        });
                    });
                });

                theRemoteDevice.tmp006.on('readI2C', function(i2cAddr, register, readSize, callback) {

                    console.log('readI2C', i2cAddr, register, readSize);

                    theRemoteDevice.writeGATTAttribute('I2C_CHAR_DEVICE_ADDRESS', new Buffer([i2cAddr]), function(err, command, result) {

                        if (err) {
                            return callback(new VError(err, "btRemoteDevice %s write I2C_CHAR_DEVICE_ADDRESS error", theRemoteDevice.mac));
                        }

                        theRemoteDevice.writeGATTAttribute('I2C_CHAR_DEVICE_REGISTER', new Buffer([register]), function(err, command, result) {

                            if (err) {
                                return callback(new VError(err, "btRemoteDevice %s write I2C_CHAR_DEVICE_REGISTER error", theRemoteDevice.mac));
                            }

                            theRemoteDevice.writeGATTAttribute('I2C_CHAR_READ_LENGTH', new Buffer([readSize]), function(err, command, result) {

                                if (err) {
                                    return callback(new VError(err, "btRemoteDevice %s write I2C_CHAR_READ_LENGTH error", theRemoteDevice.mac));
                                }

                                theRemoteDevice.readGATTAttribut('I2C_CHAR_VALUE', function(err, command, result) {

                                    if (err) {
                                        return callback(new VError(err, "btRemoteDevice %s write I2C_CHAR_VALUE error", theRemoteDevice.mac));
                                    }

                                    callback(null, result.readData.value);
                                });
                            });
                        });
                    });
                });


                console.log('Device ', theRemoteDevice.mac , ' is starting up sming measuring.... ');

                mqttClient.publish('/sming/' + theRemoteDevice.mac + '/start', 'start measuring');

                theRemoteDevice.tmp006.initTMP006(function(err) {

                    if (err) {
                        return callback(new VError(err, "btRemoteDevice %s initTMP006", theRemoteDevice.mac));
                    }

                    theRemoteDevice.setPollingInterval(setInterval(function () {

                        theRemoteDevice.tmp006.readObjTempC(function (err, tempC) {

                            if (err) {
                                return console.error(theRemoteDevice.mac, 'Error reading infrared temperature: ', err);
                            }

                            console.log('Device', theRemoteDevice.mac, 'read infrared temp: ', tempC);

                        })
                    }, 5000));

                    callback(null, true);

                });



                /*
                theRemoteDevice.writeGATTAttribute('LSM330_CHAR_GYRO_EN', new Buffer([1]), function(err, command, result) {

                    if(err) {
                        return callback(new VError(err, "btRemoteDevice %s read LSM330_CHAR_GYRO_EN error", theRemoteDevice.mac));
                    }

                    theRemoteDevice.writeGATTAttribute('LSM330_CHAR_ACC_EN', new Buffer([1]), function(err, command, result) {

                        if(err) {
                            return callback(new VError(err, "btRemoteDevice %s read LSM330_CHAR_ACC_EN error", theRemoteDevice.mac));
                        }


                        theRemoteDevice.writeGATTAttribute('LSM330_CHAR_ACC_FSCALE', new Buffer([4]), function(err, command, result) {

                            if (err) {
                                return callback(new VError(err, "btRemoteDevice %s read LSM330_CHAR_ACC_FSCALE error", theRemoteDevice.mac));
                            }

                            //  gateway.commandQueue.addCommand(new bgCommand.bgCommand(bg.api.attClientAttributeWrite, [connectionHandle, theClient.ccidHandle, new Buffer([0x01, 0x00])]), 30000,
                            theRemoteDevice.enableGattListener(function (err) {

                                if (err) {
                                    return callback(new VError(err, "btRemoteDevice %s read enableGattListener error", theRemoteDevice.mac));
                                }

                                theRemoteDevice.readGATTAttribut('LSM330_CHAR_ACC_FSCALE', function (err, command, result) {

                                    if (err) {
                                        return callback(new VError(err, "btRemoteDevice %s read LSM330_CHAR_ACC_FSCALE error", theRemoteDevice.mac));
                                    }

                                    switch (result.readData.value.readUInt8(0)) {
                                        case 1: // 4g messbereich
                                            theRemoteDevice.accMessbereichInG = 4;
                                            theRemoteDevice.accFscaleMultiplikator = 0.122;
                                            break;
                                        case 2: // 6g messbereich
                                            theRemoteDevice.accMessbereichInG = 6;
                                            theRemoteDevice.accFscaleMultiplikator = 0.183;
                                            break;
                                        case 3: // 8g messbereich
                                            theRemoteDevice.accMessbereichInG = 8;
                                            theRemoteDevice.accFscaleMultiplikator = 0.244;
                                            break;
                                        case 4: // 16g messbereich
                                            theRemoteDevice.accMessbereichInG = 16;
                                            theRemoteDevice.accFscaleMultiplikator = 0.732;
                                            break;
                                        default: // 2g messbereich
                                            theRemoteDevice.accMessbereichInG = 2;
                                            theRemoteDevice.accFscaleMultiplikator = 0.061;
                                            break;
                                    }

                                    console.log("Device ", theRemoteDevice.mac, " Accolemeter", result.readData.value, " Max: " + theRemoteDevice.accMessbereichInG + "G", "Min: " + theRemoteDevice.accFscaleMultiplikator + "mG");
                                    mqttClient.publish('/sming/' + theRemoteDevice.mac + '/accelometer', " Max: " + theRemoteDevice.accMessbereichInG + "G / Min: " + theRemoteDevice.accFscaleMultiplikator + "mG");


                                    theRemoteDevice.writeGATTAttribute('MEASURE_CHAR_START', new Buffer([1]), function (err, command, result) {

                                        if (err) {
                                            return callback(new VError(err, "btRemoteDevice %s write MEASURE_CHAR_START error", theRemoteDevice.mac));
                                        }

                                        theRemoteDevice.setPollingInterval(setInterval(function () {

                                            theRemoteDevice.readGATTAttribut('LSM330_CHAR_TEMP_SAMPLE', function (err, command, result) {

                                                if (err) {
                                                    return console.error(theRemoteDevice.mac, 'Error reading temperature: ', err);
                                                }

                                                if (result && result.readData && result.readData.value && result.readData.value.length == 1) {
                                                    console.log('Device', theRemoteDevice.mac, 'read temp: ', result.readData.value.readInt8(0));
                                                    mqttClient.publish('/sming/' + theRemoteDevice.mac + '/temp', result.readData.value.readInt8(0).toString());
                                                }


                                            })
                                        }, 5000));

                                        callback(null, true);
                                    })

                                })

                            })

                        })


                    })


                })
                */
            };

            if(globalTimer == null) {
                globalTimer = setTimeout(function() {

                    // stop scanning if we are scanning already
                    gateway.commandQueue.addCommand(new bgCommand.bgCommand(bg.api.gapEndProcedure, null), 10000, function(err, command, result) {

                        if(err) {
                            return console.error("gapEndProcedure error", err);
                        }
                        console.log("Connect with ", gateway.clientList.length, " devices");


                        gateway.connectNextClient();
                    });


                }, 5000)
            }
        }
        else {
            console.log("Unknown btRemoteDevice ", newRemoteDevice.mac, " found!");
        }
    });


    gateway.connectNextClient = function() {

        var allConnected = gateway.clientList.length > 0;

        for(var i = 0; i < gateway.clientList.length; i++) {
            if(gateway.clientList[i].connectionState === connectionStates.DISCONNECTED && gateway.clientList[i].isSming) {

                gateway.clientList[i].connect(function(err) {
                    if(err) return console.error(err);

                });

                return;
            }

            allConnected = gateway.clientList[i].connectionState === connectionStates.CONNECTED;
        }


        /*
         if(allConnected && !gateway.isMeasuringStartet) {
         gateway.isMeasuringStartet = true;

         for(var i = 0; i < clientList.length; i++) {
         setTimeout(clientList[i].smingStartMeasuring, i * 1000, function(err) {

         if(err) return console.error(err);

         //console.log("btRemoteDevice measuring started!");
         });
         }

         }*/

    };
}


///////////////////////////////////////// Exit handler //////////////////////////////////////
function exitHandler(options, err) {
    if (options.cleanup) {
        console.log('cleanup');
        if(serial.btDevice) serial.btDevice.disconnectAll();
    }
    if (err) console.log(err.stack);
    if (options.exit) process.exit();
}

//do something when app is closing
process.on('exit', exitHandler.bind(null,{cleanup:true}));

//catches ctrl+c event
process.on('SIGINT', exitHandler.bind(null, {exit:true}));

//catches uncaught exceptions
process.on('uncaughtException', exitHandler.bind(null, {exit:true}));
