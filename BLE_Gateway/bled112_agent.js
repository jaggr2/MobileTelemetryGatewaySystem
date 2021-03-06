var S = require('string'),
    mqtt = require('mqtt'),
    mqttrouter = require('mqtt-router'),
    bg = require('bglib'),
    libCommandQueue = require('commandqueue'),
    bgCommand = libCommandQueue.bluegigaCommand,
    commandQueue = libCommandQueue.commandQueue;

console.log(libCommandQueue);

require('buffertools').extend();

var bglib = new bg();
var deviceType = "BLED112Serial";
var agentID = "agent01";


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
    MEASURE_CHAR_DATASTREAM : "8EDF0304-67E5-DB83-F85B-A1E2AB1C9E7A"
    };

var getUUIDBuffer = function(UUID) {
    var uuidHex = S(UUID).replaceAll('-','').toLowerCase();

    return new Buffer(uuidHex.toString(), 'hex').reverse();
};


var getDescriptors = function() {

    var allDescriptors = [];
    for(var key in descriptorDefinitions) {
        if (descriptorDefinitions.hasOwnProperty(key)) {
            allDescriptors.push({
                name: key,
                uuid: getUUIDBuffer(descriptorDefinitions[key]),
                handle: 0 })
        }
    }

    return allDescriptors;
};

var getDescriptorByKey = function(list, key) {

    for(var i = 0;  i < list.length; i++) {

        if(list[i].name == key) {
            return list[i];
        }
    }

    return null;
};

var setDescriptorHandle = function(list, uuid, handle) {

    for(var i = 0;  i < list.length; i++) {

        if(list[i].uuid.equals(uuid)) {
            list[i].handle = handle;
            return list[i];
        }
    }

    return null;
};

var setDescriptorValueByHandle = function(list, handle, value) {

    for(var i = 0;  i < list.length; i++) {

        if(list[i].handle == handle) {
            list[i].value = value;
            return list[i];
        }
    }

    return null;
};


/* store this to redis */
var deviceList = [];

var setGatewayByName = function(name, value) {
    var entry = getGatewayByName(name);
    if(entry != null) {
        entry.value = value;
    }
    else {
        entry = { name: name, value: value };
        deviceList.push(entry);
    }
    return entry.value;
};

var getGatewayByName = function(name) {
    for(var i = 0; i < deviceList.length; i++) {
        if(deviceList[i].name == name) {
            return deviceList[i].value;
        }
    }
    return null;
};
/* store this to redis */


require('getmac').getMac(function(err,macAddress){
    if (err)  {
        console.log("unable to get MAC address");
        return
    }

    macAddress = S(macAddress).replaceAll(':','').s;

    console.log("DeviceID:",macAddress);

    // mqtt connect
    var client  = mqtt.connect('mqtt://web:1234@formula.xrj.ch');

    // enable the subscription router
    var router = mqttrouter.wrap(client);

    // MQTT Handlers
    router.subscribe('/' + deviceType + '/+:device/+:port/data', function(topic, message, params){

        var gwID = params.device + "/" + params.port;
        var gateway = getGatewayByName(gwID);

        if(gateway) {
            //console.log("forward message to queue", message);
            bgCommand.bgProcessData(message, gateway.commandQueue);
        }

    });

    router.subscribe('/' + deviceType + '/+:device/+:port/status', function(topic, message, params){

        var status = JSON.parse(message);
        var gwID = params.device + "/" + params.port;
        console.log("received status from ", gwID, ": ", status);

        var gateway = getGatewayByName(gwID);

        if(gateway == null) {
            //console.log("New gateway ", gwID, deviceList);
            // new gateway
            var newCommandQueue = new commandQueue({ eventParams: '/' + deviceType + '/' + params.device + '/' + params.port + '/write' });

            newCommandQueue.on('write', function(data, eventParams) {
                //console.log("publish to ", eventParams, data);
                client.publish(eventParams, data);
            });

            newCommandQueue.on('asyncPacket', function(packet, eventParams) {
                //console.log("async data from ", eventParams, ": ", packet);

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


                if(packet.responseType == 'Event') {

                    switch(packet.packet.cClass) {
                        case bgClass.GenericAccessProfile:

                            if(packet.packet.cID == 0) {

                                // ignore discover packets when sming is found
                                if (gateway.foundSming) return;

                                var btData = packet.response.data;
                                for (var i = 0; i < btData.length; i++) {
                                    if (btData[i].typeFlag == 9) { // Complete local name

                                        if (btData[i].data == 'TXW51') {
                                            gateway.foundSming = true;
                                            console.log('Found sming ', packet.response.sender.toString('hex'), packet.response.rssi);
                                            client.publish('/sming/found', packet.response.sender.toString('hex'));
                                            gateway.getSmingDetails(packet.response.sender);
                                            return;
                                        }

                                    }

                                }

                                console.log("discovered ", packet.response.sender.toString('hex'), packet.response);
                            }
                            break;

                        case bgClass.Connection:

                            if(packet.response.reason) {
                                gateway.commandQueue.quitAllCommands(packet.response.reason.message);

                                console.error("Connection ", packet.response.connection, " error: ", packet.response.reason.message);
                                return;
                            }

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

                            console.log("Connection ", packet.response.connection, " status change: ", flags, " address:", packet.response.address.toString('hex'), ", adr-type:", adresstype, ", conn_interval:", packet.response.conn_interval, ", timeout:", packet.response.timeout, ", latency:", packet.response.latency, ", bonding:", packet.response.bonding);
                            break;

                        case bgClass.AttributeClient:

                            if(packet.response.atthandle && gateway.MEASURE_CHAR_DATASTREAM_HANDLE && packet.response.atthandle == gateway.MEASURE_CHAR_DATASTREAM_HANDLE) {


                                if(packet.response && Buffer.isBuffer(packet.response.value)) {

                                    var buffer = packet.response.value;
                                    var controllByte = buffer.readInt8(0);
                                    var numberOfSamples = controllByte & 0x0F;
                                    var validAxis = (controllByte >> 4) & 0x07;
                                    var accOrGyro = (controllByte >> 7) & 0x01;
                                    var sequenceNumber = buffer.readInt8(1);

                                    console.log("Measure Event ", buffer, numberOfSamples, validAxis, accOrGyro);

                                    var samples = [];

                                    // Decode data points.
                                    for (var j = 0; j < numberOfSamples; j++) {

                                        var index = 2 + j*6;

                                        var sample = { sequenceNumber: sequenceNumber,
                                            point: [  buffer.readInt16LE(index), // X-Achse
                                                buffer.readInt16LE(index + 2),    // Y-Achse
                                                buffer.readInt16LE(index + 4) ],   // Z-Achse
                                            accOrGyro: accOrGyro
                                        };

                                        client.publish('/sming/measurement', JSON.stringify(sample));
                                        samples.push(sample);
                                    }

                                    console.log("samples: ", samples);

                                }
                                else {
                                    console.log("Measure Event: ", (result.message ? result.message : result))
                                }
                            }
                            else {
                                console.log("Attribute: ", packet);
                            }
                            break;


                        default:
                            console.log("unimplemented class", packet.packet.cClass, packet.response);
                    }
                }
            });

            var newValue = {
                lastStatusMessage: new Date(),
                lastState: status.isOpen,
                commandQueue: newCommandQueue
            };

            gateway = setGatewayByName(gwID, newValue);

            gateway.foundSming = false;

            gateway.commandQueue.addCommand(new bgCommand.bgCommand(bg.api.systemHello, null), 10000, function(err, command, result) {

                if(err) {
                    return console.error("systemHello error", err);
                }
                console.log("systemHello result", result);

            });


            gateway.disconnect = function() {
                // disconnect if we are connected already
                gateway.commandQueue.addCommand(new bgCommand.bgCommand(bg.api.connectionDisconnect, [0]), 10000, function (err, command, result) {

                    if (err) {
                        return console.error("connectionDisconnect error", err);
                    }
                    console.log("connectionDisconnect result", result);
                });

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

                client.publish('/sming/discover', 'Start discovering smings');

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



            gateway.getSmingDetails = function(bufferAddr) {

                /* stop scanning if we are scanning already
                gateway.commandQueue.addCommand(new bgCommand.bgCommand(bg.api.gapEndProcedure, null), 10000, function(err, command, result) {

                    if(err) {
                        return console.error("gapEndProcedure error", err);
                    }
                    console.log("gapEndProcedure result", result);
                }); */
                client.publish('/sming/read', 'read bluetooth attributes');


                // direct connect
                gateway.commandQueue.addCommand(new bgCommand.bgCommand(bg.api.gapConnectDirect, [bufferAddr, 1, 60, 76, 100, 9]), 10000, function(err, command, result) {

                    if(err) {
                        return console.error("gapConnectDirect error", err);
                    }
                    console.log("gapConnectDirect result", result);

                    var connectionHandle = result.connection_handle;


                    console.log('connectionHandle: ', connectionHandle);

                    /*
                    gateway.commandQueue.addCommand(new bgCommand.bgCommand(bg.api.attClientReadByHandle, [connectionHandle, 13]), 10000, function(err, command, result) {

                        if(err) {
                            return console.error("attClientReadByHandle error", err);
                        }
                        console.log("attClientReadByHandle result", result);
                    }); */



                    // read attribute
                    /*
                    gateway.commandQueue.addCommand(new bgCommand.bgCommand(bg.api.attClientReadByGroupType, [connectionHandle, 1, 0xFFFF, [0x00, 0x28]]), 10000, function(err, command, result) {

                        if(err) {
                            return console.error("attClientReadByType error", err);
                        }

                        var infoService = getUUIDBuffer(DEVICE_INFO_SERVICE);
                        var lsm330Service = getUUIDBuffer(LSM330_SERVICE);
                        var measureService = getUUIDBuffer(MEASURE_SERVICE);

                        console.log("attClientReadByType", result);
                        console.log(infoService, lsm330Service, measureService);

                        for(var k = 0; k < result.resultList.length; k++) {

                            if( infoService.equals(result.resultList[k].uuid)) {
                                //case lsm330Service.equals(result.resultList[k].uuid):
                                //case measureService.equals(result.resultList[k].uuid):
*/
                                    gateway.commandQueue.addCommand(new bgCommand.bgCommand(bg.api.attClientFindInformation, [connectionHandle, 1, 0xffff]), 10000, function(err, command, result) {

                                        if(err) {
                                            return console.error("attClientFindInformation error", err);
                                        }
                                        //console.log("attClientFindInformation result", command.duration, result);


                                        var HandleList = [];
                                        var descriptorList = getDescriptors();
                                        var ccidUuid = new Buffer([0x02, 0x29]);

                                        for(var j = 0; j < result.resultList.length; j++) {


                                            if(result.resultList[j].uuid.equals(ccidUuid)) {
                                                console.log("CCID Handle gefunden:", result.resultList[j].chrhandle);
                                                gateway.ccidHandle = result.resultList[j].chrhandle;
                                            }

                                            var foundDescriptor = setDescriptorHandle(descriptorList, result.resultList[j].uuid, result.resultList[j].chrhandle);
                                            if( foundDescriptor !== null) {
                                                HandleList.push(result.resultList[j].chrhandle)

                                                if(foundDescriptor.name == "MEASURE_CHAR_DATASTREAM") {
                                                    gateway.MEASURE_CHAR_DATASTREAM_HANDLE = foundDescriptor.handle;
                                                    console.log("MEASURE_CHAR_DATASTREAM Handle gefunden:", result.resultList[j].chrhandle);
                                                }
                                            }
                                        }


                                        console.log("attClientFindInformation result -> list of handles of interest: ", HandleList);


                                        for(var l = 0; l < HandleList.length; l++) {

                                            var theCommand = new bgCommand.bgCommand(bg.api.attClientReadByHandle, [connectionHandle, HandleList[l]]);

                                            theCommand.isLastCommand = (l + 1 >= HandleList.length);

                                            gateway.commandQueue.addCommand(theCommand, 10000, function (err, command, result) {

                                                if (err) {
                                                    return console.error("attClientReadByHandle error", err);
                                                }
                                                console.log("attClientReadByHandle result", command.duration, ( result.readData && result.readData.value ? result.readData.value.toString() : (result.message ? result.message : result)));


                                                if(result.readData && result.readData.value) {
                                                    setDescriptorValueByHandle(descriptorList, result.readData.atthandle, result.readData.value);
                                                }

                                                if(command.command.isLastCommand) {

                                                    console.log("finished: ", descriptorList);


                                                    gateway.startMeasuring(connectionHandle, descriptorList, function(err) {


                                                        /*
                                                        for(var o = 0; o < 29; o++) {

                                                            gateway.readAttribut(connectionHandle, descriptorList, "MEASURE_CHAR_DATASTREAM", function (err, command, result) {


                                                            });

                                                        }
                                                        */

                                                    })


                                                }
                                            });
                                        }
                                    });

  /*                                  //break;
                            }


                        }

                        /*
                        gateway.commandQueue.addCommand(new bgCommand.bgCommand(bg.api.connectionDisconnect, [connectionHandle]), 10000, function (err, command, result) {

                            if (err) {
                                return console.log("connectionDisconnect error", err);
                            }
                            console.log("connectionDisconnect result", result);
                        }); */
                        gateway.foundSming = false;
                    });

                    /*

                    gateway.commandQueue.addCommand(new bgCommand.bgCommand(bg.api.connectionDisconnect, [connectionHandle]), 10000, function (err, command, result) {

                        if (err) {
                            return console.log("connectionDisconnect error", err);
                        }
                        console.log("connectionDisconnect result", result);
                    });
                    gateway.foundSming = false;
                     */

                //});



                setTimeout(function() {

                    client.publish('/sming/stop', 'stop sming measuring, wait 60s before scanning');
                    gateway.disconnect();

                    setTimeout(gateway.startScanning, 60000) }, 2 * 60 * 1000);

                //gateway.startScanning();

            };

            gateway.startMeasuring = function(connectionHandle, descriptorList, callback) {

                client.publish('/sming/start', 'start sming measuring');

                if(!gateway.ccidHandle ) {
                    return callback("no ccidHandle available!");
                }

                gateway.writeAttribut(connectionHandle, descriptorList, 'LSM330_CHAR_GYRO_EN', new Buffer([1]), function(err, command, result) {

                    if(err) {
                        return console.error("writeAttribut LSM330_CHAR_GYRO_EN error", err);
                    }

                    gateway.writeAttribut(connectionHandle, descriptorList, 'LSM330_CHAR_ACC_EN', new Buffer([1]), function(err, command, result) {

                        if(err) {
                            return console.error("writeAttribut LSM330_CHAR_ACC_EN error", err);
                        }

                        gateway.commandQueue.addCommand(new bgCommand.bgCommand(bg.api.attClientAttributeWrite, [connectionHandle, gateway.ccidHandle, new Buffer([0x01, 0x00])]), 30000, function(err, command, result) {

                            if(err) {
                                return console.error("write ccidHandle error", err);
                            }

                            gateway.writeAttribut(connectionHandle, descriptorList, 'MEASURE_CHAR_START', new Buffer([1]), function(err, command, result) {

                                if(err) {
                                    return console.error("writeAttribut MEASURE_CHAR_START error", err);
                                }

                                callback(null, true);



                            })



                        })




                    })


                })






            };

            gateway.writeAttribut = function(connection, descriptorList, key, newValueBuffer, callback) {

                var descriptor = getDescriptorByKey(descriptorList, key);
                if(!descriptor) {
                    return callback(err, 'unknown descriptor ' + key);
                }

                if(! (descriptor.handle > 0)) {
                    return callback(err, 'descriptor ' + key + ' has no handle');
                }

                gateway.commandQueue.addCommand(new bgCommand.bgCommand(bg.api.attClientAttributeWrite, [connection, descriptor.handle, newValueBuffer]), 30000, callback);

            };

            gateway.readAttribut = function(connection, descriptorList, key, callback) {

                var descriptor = getDescriptorByKey(descriptorList, key);
                if(!descriptor) {
                    return callback(err, 'unknown descriptor ' + key);
                }

                if(! (descriptor.handle > 0)) {
                    return callback(err, 'descriptor ' + key + ' has no handle');
                }

                gateway.commandQueue.addCommand(new bgCommand.bgCommand(bg.api.attClientReadByHandle, [connection, descriptor.handle]), 30000, callback);

            };

            gateway.disconnect();

            gateway.startScanning();



            /*
            bglib.getPacket(bg.api.systemHello, function(err, packet) {

                var sPort = "ttyACM0";

                console.log("publish hello to ", '/' + deviceType + '/' + macAddress + '/' + sPort + '/write', packet);
                client.publish('/' + deviceType + '/' + macAddress + '/' + sPort + '/write', packet.getByteArray());
            });
            */
        }


    });

    router.subscribe('/' + deviceType + '/+:device/+:port/error', function(topic, message, params){

        console.log("received error from ", params.device, params.port, ": ", message);
    });

    router.subscribe('/' + deviceType + '/+:device/+:port/close', function(topic, message, params){

        console.log("received close from ", params.device, params.port, ": ", message);
    });

    router.subscribe('/' + deviceType + '/+:device/+:port/open', function(topic, message, params){

        console.log("received open from ", params.device, params.port, ": ", message);
    });

    var pingAllGateways = function() {
        client.publish('/' + deviceType + '/all/all/ping', agentID);
    };

    // start the gateway up
    client.on('connect', function () {

        pingAllGateways();

        setInterval(pingAllGateways, 10000);
    });
});