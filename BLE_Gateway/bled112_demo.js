var S = require('string'),
    mqtt = require('mqtt'),
    mqttrouter = require('mqtt-router'),
    bg = require('bglib'),
    libCommandQueue = require('commandqueue'),
    bgCommand = libCommandQueue.bluegigaCommand,
    commandQueue = libCommandQueue.commandQueue,
    SerialPort = require("serialport").SerialPort,
    events = require('events'),
    VError = require('verror');

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




/* store this to redis */

var clientList = [];

var getClientByMAC = function(mac) {
    for(var i = 0; i < clientList.length; i++) {
        if(clientList[i].mac == mac) {
            return clientList[i];
        }
    }
    return null;
};

var getClientByConnectionHandle = function(connHandle) {
    for(var i = 0; i < clientList.length; i++) {
        if(clientList[i].connectionId == connHandle) {
            return clientList[i];
        }
    }
    return null;
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

        gateway.commandQueue.addCommand(new bgCommand.bgCommand(bg.api.connectionDisconnect, [self.connectionId]), 10000, function (err) { //, command, result
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

    self.readGATTIndex = function(descriptorDefinitionList, callback) {

        if(self.connectionState !== connectionStates.CONNECTED) return callback(new VError('can not read GATT when btRemoteDevice is not connected. Current state: %s', getConnectionStateName(self.connectionState)));

        if(!descriptorDefinitionList) return callback(new VError('readEntireGATT() invalid parameter descriptorDefinitions'));

        console.log("btRemoteDevice ", self.mac, " reload GATT handle index from device...");
        self.gatt = new Gatt(descriptorDefinitionList);

        self.commandQueue.addCommand(new bgCommand.bgCommand(bg.api.attClientFindInformation, [self.connectionId, 1, 0xffff]), 10000, function(err, command, result) {

            if(err) return callback(new VError(err, 'Error while reading GATT of btRemoteDevice %s', self.mac));

            var ccidUuid = new Buffer([0x02, 0x29]);

            if(!result.resultList) {
                return callback(new VError("got no result while reading gatt", result.resultList));
            }

            for(var j = 0; j < result.resultList.length; j++) {

                if(result.resultList[j].uuid.equals(ccidUuid)) {
                    self.ccidHandle = result.resultList[j].chrhandle;
                }

                var attr = self.gatt.getAttributeByUUID(result.resultList[j].uuid);

                if(attr) {
                    attr.handle = result.resultList[j].chrhandle;
                }
            }

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

        if(!descriptor) return callback(new VError('unknown descriptor %s', key));

        if(!(descriptor.handle > 0)) return callback(new VError('descriptor %s has no handle, read readGATTIndex first', key));

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

                console.log("Connection to ", self.mac , " changed status: ", flags, " address:", packet.response.address.toString('hex'), ", adr-type:", adresstype, ", conn_interval:", packet.response.conn_interval, ", timeout:", packet.response.timeout, ", latency:", packet.response.latency, ", bonding:", packet.response.bonding);
            }
        }
    };

    self.handleAttributeEvent = function(packet) { // packet.packet.cClass ist immer 4

        switch(packet.packet.cID) {
            case 5:
                self.emit('attributeValueReceived', packet);
                break;
            case 4:
                console.log(self.mac, ' got handle information', packet);
                break;

            default:
                console.log(self.mac, ' got unknown attribut class packet', packet);
        }
    };

};
btRemoteDevice.prototype.__proto__ = events.EventEmitter.prototype;



/* store this to redis */
var globalTimer = null;
var gateway = null;
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



    gateway = {
        name: sPort,
        lastStatusMessage: new Date(),
//      lastState: status.isOpen,
        commandQueue: new commandQueue({}) // { eventParams: '/' + deviceType + '/' + params.device + '/' + params.port + '/write' }
    };


    gateway.commandQueue.on('write', function(data, eventParams) {
        //console.log("publish to ", eventParams, data);
        //client.publish(eventParams, data);

        if(!serial.isOpen()) {

            reconnectSerial(function (err) {
                if(err) {
                    return;
                }

                //console.log('write message to serial', message);
                serial.write(data, function (err) {
                    if (err) {
                        console.error('failed to write on serialport: ' + err.toString());
                    }
                });
            });
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

    gateway.commandQueue.on('asyncPacket', function(packet, eventParams) {

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
                theRemoteDevice =  getClientByConnectionHandle(packet.response.connection);

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

                    if(packet.packet.cID == 0) {

                        var btData = packet.response.data;
                        for (var i = 0; i < btData.length; i++) {
                            if (btData[i].typeFlag == 9) { // Complete local name


                                if (btData[i].data == 'TXW51') {
                                    //gateway.foundSming = true;

                                    console.log('Found Sming ', packet.response.sender.toString('hex'), packet.response.rssi);
                                    mqttClient.publish('/sming/found', packet.response.sender.toString('hex') + " " + packet.response.rssi);


                                    if(getClientByMAC(packet.response.sender.toString('hex')) == null) {

                                        var newRemoteDevice = new btRemoteDevice(packet.response.sender, gateway.commandQueue);
                                        clientList.push(newRemoteDevice);

                                        newRemoteDevice.on('connectionStateChange', function(err, newState) {

                                            var theRemoteDevice = this;

                                            if(err) {
                                                console.error("btRemoteDevice ", theRemoteDevice.mac, " is now ", getConnectionStateName(newState), " due to error ", err);
                                                mqttClient.publish('/sming/' + theRemoteDevice.mac + '/connectionState', getConnectionStateName(newState) + " due to error " + err)
                                            }
                                            else {
                                                console.log("btRemoteDevice ", theRemoteDevice.mac, " is now ", getConnectionStateName(newState));
                                                mqttClient.publish('/sming/' + theRemoteDevice.mac + '/connectionState', getConnectionStateName(newState) + " ");
                                            }

                                            switch(newState) {

                                                case connectionStates.CONNECTED:

                                                    theRemoteDevice.readGATTIndex(descriptorDefinitions, function(err) { // ;packet.response.connection
                                                        if(err) return console.error(err);

                                                        gateway.connectNextClient();

                                                        theRemoteDevice.smingStartMeasuring(function(err) {

                                                            if(err) return console.error(err);

                                                            console.log("btRemoteDevice measuring started!");
                                                        });

                                                    });
                                                    break;
                                            }
                                        });

                                        newRemoteDevice.on('attributeValueReceived', function(packet) {

                                            var theRemoteDevice = this;

                                            var measureAttributData = theRemoteDevice.gatt.getAttributeByName("MEASURE_CHAR_DATASTREAM");

                                            if(packet.response.atthandle && measureAttributData && packet.response.atthandle == measureAttributData.handle) {


                                                if (packet.response && Buffer.isBuffer(packet.response.value)) {

                                                    var buffer = packet.response.value;
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

                                                        if(!theRemoteDevice.accFscaleMultiplikator) theRemoteDevice.accFscaleMultiplikator = 0.061;

                                                        var sample = { sequenceNumber: sequenceNumber,
                                                            point: [  buffer.readInt16LE(index) * theRemoteDevice.accFscaleMultiplikator, // X-Achse
                                                                    buffer.readInt16LE(index + 2) * theRemoteDevice.accFscaleMultiplikator,    // Y-Achse
                                                                    buffer.readInt16LE(index + 4) * theRemoteDevice.accFscaleMultiplikator ],   // Z-Achse
                                                            accOrGyro: accOrGyro
                                                        };

                                                        if(!theRemoteDevice.measuredCount) {
                                                            console.log("received first measurement data");
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

                                                    //console.log("samples: ", samples);

                                                }
                                                else {
                                                    console.log("btRemoteDevice ", theRemoteDevice.mac, ": Measure Event: ", (result.message ? result.message : result))
                                                }
                                            }
                                            else {
                                                var attributData = theRemoteDevice.gatt.getAttributeByHandle(packet.response.atthandle);

                                                console.log("btRemoteDevice ", theRemoteDevice.mac, ": got value of attribut ", attributData.name ,":", packet.value);
                                            }
                                        });

                                        newRemoteDevice.smingStartMeasuring = function(callback) {

                                            var theRemoteDevice = newRemoteDevice;

                                            theRemoteDevice.setPollingInterval(setInterval(function() {

                                                theRemoteDevice.readGATTAttribut('LSM330_CHAR_TEMP_SAMPLE', function(err, command, result) {

                                                    if(err) {
                                                        console.error('Error reading temperature: ', err);
                                                    }

                                                    if(result && result.readData && result.readData.value && result.readData.value.length == 1) {
                                                        console.log('read temp: ', result.readData.value.readInt8(0));
                                                        mqttClient.publish('/sming/' + theRemoteDevice.mac + '/temp', result.readData.value.readInt8(0).toString());
                                                    }


                                                })
                                             }, 5000));


                                            console.log('start sming measuring for ' + theRemoteDevice.mac);
                                            mqttClient.publish('/sming/' + theRemoteDevice.mac + '/start', 'start measuring');


                                            theRemoteDevice.writeGATTAttribute('LSM330_CHAR_GYRO_EN', new Buffer([1]), function(err, command, result) {

                                                if(err) {
                                                    return callback(new VError(err, "btRemoteDevice %s read LSM330_CHAR_GYRO_EN error", theRemoteDevice.mac));
                                                }

                                                theRemoteDevice.writeGATTAttribute('LSM330_CHAR_ACC_EN', new Buffer([1]), function(err, command, result) {

                                                    if(err) {
                                                        return callback(new VError(err, "btRemoteDevice %s read LSM330_CHAR_ACC_EN error", theRemoteDevice.mac));
                                                    }

                                                    //  gateway.commandQueue.addCommand(new bgCommand.bgCommand(bg.api.attClientAttributeWrite, [connectionHandle, theClient.ccidHandle, new Buffer([0x01, 0x00])]), 30000,
                                                    theRemoteDevice.enableGattListener(function(err) {

                                                        if(err) {
                                                            return callback(new VError(err, "btRemoteDevice %s read enableGattListener error", theRemoteDevice.mac));
                                                        }


                                                        theRemoteDevice.readGATTAttribut('LSM330_CHAR_ACC_FSCALE', function(err, command, result) {

                                                            if(err) {
                                                                return callback(new VError(err, "btRemoteDevice %s read LSM330_CHAR_ACC_FSCALE error", theRemoteDevice.mac));
                                                            }

                                                            switch(result.readData.value.readUInt8(0)) {
                                                                case 0: // 2g messbereich
                                                                    theRemoteDevice.accMessbereichInG = 2;
                                                                    theRemoteDevice.accFscaleMultiplikator = 0.061;
                                                                    break;
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
                                                            }

                                                            console.log(theRemoteDevice.mac,  " Accolemeter",  "Max: " + theRemoteDevice.accMessbereichInG + "G", "Min: " + theRemoteDevice.accFscaleMultiplikator + "mG");
                                                            mqttClient.publish('/sming/' + theRemoteDevice.mac + '/accelometerMaxG', theRemoteDevice.accMessbereichInG);


                                                            theRemoteDevice.writeGATTAttribute('MEASURE_CHAR_START', new Buffer([1]), function(err, command, result) {

                                                                if(err) {
                                                                    return console.error("writeAttribut MEASURE_CHAR_START error", err);
                                                                }

                                                                callback(null, true);
                                                            })

                                                        })

                                                    })




                                                })


                                            })
                                        };

                                        if(globalTimer == null) {
                                            globalTimer = setTimeout(function() {

                                                // stop scanning if we are scanning already
                                                gateway.commandQueue.addCommand(new bgCommand.bgCommand(bg.api.gapEndProcedure, null), 10000, function(err, command, result) {

                                                    if(err) {
                                                        return console.error("gapEndProcedure error", err);
                                                    }
                                                    console.log("Connect with ", clientList.length, " devices");


                                                    gateway.connectNextClient();
                                                });


                                            }, 5000)
                                        }
                                    }

                                    return;
                                }

                            }

                        }

                        console.log("discovered ", packet.response.sender.toString('hex'), packet.response);
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


    gateway.connectNextClient = function() {

        var allConnected = clientList.length > 0;

        for(var i = 0; i < clientList.length; i++) {
            if(clientList[i].connectionState === connectionStates.DISCONNECTED) {

                clientList[i].connect(function(err) {
                   if(err) return console.error(err);

                });

                return;
            }

            allConnected = clientList[i].connectionState === connectionStates.CONNECTED;
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



    gateway.disconnectAll = function() {

        gateway.commandQueue.quitAllCommands("disconnectAll");

        for(var i = 0; i < clientList.length; i++) {

            if(clientList[i].connectionState === connectionStates.CONNECTED) {
                clientList[i].disconnect(function(err) {
                    if(err) return console.error(err);

                });
            }
        }

        // stop advertising if we are advertising already
        gateway.commandQueue.addCommand(new bgCommand.bgCommand(bg.api.gapSetMode, [0, 0]), 10000, function (err, command, result) {

            if (err) {
                return console.error("gapSetMode error", err);
            }
            console.log("gapSetMode result", result);

        });

        // stop scanning if we are scanning already
        gateway.commandQueue.addCommand(new bgCommand.bgCommand(bg.api.gapEndProcedure, null), 10000, function(err, command, result) {

            if(err) {
                return console.error("gapEndProcedure error", err);
            }
            console.log("gapEndProcedure result", result);

        });

    };

    gateway.startScanning = function() {

        mqttClient.publish('/sming/discover', 'Start discovering smings');

         // set scan parameters
         gateway.commandQueue.addCommand(new bgCommand.bgCommand(bg.api.gapSetScanParameters, [0xC8, 0xC8, 0]), 10000, function(err, command, result) {

             if(err) {
                return console.log("gapSetMode error", err);
             }
            console.log("gapSetMode result", result);

             // start scanning now
             gateway.commandQueue.addCommand(new bgCommand.bgCommand(bg.api.gapDiscover, [1]), 10000, function(err, command, result) {

                 if(err) {
                     return console.log("gapDiscover error", err);
                 }
                 console.log("gapDiscover result", result);
             });
         });
    };

    // Serialport Handlers
    var reconnectSerial = function(callback) {

        serial = new SerialPort(sPort, serialSettings , false); // this is the openImmediately flag [default is true]

        serial.on('data', function (data) {
            //console.log('received serial data: ', data);
            //client.publish('/' + deviceType + '/' + deviceID + '/' + sPort + '/data', data);

            //var gwID = sPort;
            //var gateway = getGatewayByName(gwID);

            if(gateway) {
                //console.log("forward message to queue", data);
                bgCommand.bgProcessData(data, gateway.commandQueue);
            }
        });

        serial.on('open', function () {
            console.log('opened serialport ', serial.path);
//            client.publish('/' + deviceType + '/' + deviceID + '/' + sPort + '/open', '');


        });

        serial.on('close', function () {
            console.log('closed serialport ', serial.path);
            //            client.publish('/' + deviceType + '/' + deviceID + '/' + sPort + '/close', '');
        });

        serial.on('error', function(error) {
            console.log('Serial-Error ', error);
        });

        serial.open(function (error) {
            if ( error ) {
                console.log('failed to open serialport: ', error);
                return;
            }

            if(gateway != null) {
                gateway.commandQueue.addCommand(new bgCommand.bgCommand(bg.api.systemHello, null), 10000, function(err, command, result) {

                    if(err) {
                        return console.error("systemHello error", err);
                    }
                    console.log("systemHello ok, restart Advertising... ");
                    gateway.disconnectAll();
                    gateway.startScanning();

                });
            }

            if(callback) callback(error);
        });
    };


    //reconnectSerial();

    // start the gateway up
    mqttClient.on('connect', function () {
        reconnectSerial();
    });

});


///////////////////////////////////////// Exit handler //////////////////////////////////////
function exitHandler(options, err) {
    if (options.cleanup) {
        console.log('cleanup');
        if(gateway) gateway.disconnectAll();
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
