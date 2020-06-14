'use strict';
const zmq       = require('zeromq');
const parse     = require('./parse_data');
let   fpga_data = [0, 0, 0, 0];


// ================================
// ZMQ-REQUESTER
const requester = zmq.socket('req');
requester.on('message', data => {
    console.log('received response - ', data.readInt32LE(0));
});

// connect to responder
requester.connect('tcp://192.168.1.84:14538');


// ================================
// ZMQ-SUBSCRIBER
const subscriber = zmq.socket('sub');
subscriber.subscribe('');
subscriber.on('message', raw => {
    fpga_data = parse(raw);
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
    data.writeInt32LE(200, 1);  // set pollrate
    requester.send(data);
}, 10000); // wait 5 sec

const timer2 = setTimeout(() => {
    data[0] = 1; // shutdown daemon
    requester.send(data);
}, 10000); // shutdown fpga daemon