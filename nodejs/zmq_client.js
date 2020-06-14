'use strict';
const zmq       = require('zeromq');
let   fpga_data = [0, 0, 0, 0];


// ================================
// ZMQ-REQUESTER
const requester = zmq.socket('req');
requester.on('message', data => {
    const code = data.readInt32LE(0);
    const timestamp = data.readInt32LE(4)*1000;
    console.log(`received response: ${code} @ ${new Date(timestamp)}`);
});

// connect to responder
requester.connect('tcp://192.168.1.84:14538');


// ================================
// ZMQ-SUBSCRIBER
const subscriber = zmq.socket('sub');
subscriber.subscribe('');
subscriber.on('message', raw => {
    for (let i = 0; i < 4; i++) {
        fpga_data[i] = raw.readUInt16LE(i*2);
    }
    console.log('received data:', fpga_data);
});

// connector to publisher
subscriber.connect('tcp://192.168.1.84:14537');


// ================================
// send request to start data transfer
console.log('send request...')
let data = Buffer.alloc(10);
data[0] = 0; // start fpga daemon
requester.send(data);

const timer1 = setTimeout(() => {
    data[0] = 2;                // read poll rate
    data.writeInt32BE(200, 1);  // set pollrate
    requester.send(data);
}, 5000); // wait 5 sec

const timer2 = setTimeout(() => {
    data[0] = 1; // shutdown daemon
    requester.send(data);
}, 10000); // shutdown fpga daemon