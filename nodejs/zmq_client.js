'use strict';

const zmq = require('zeromq');

// create requester
const requester = zmq.socket('req');
requester.on('message', data => {
    console.log('received response', data);
});

// connect to responder
requester.connect('tcp://192.168.1.84:14538');

// send request to start data transfer
console.log('send request...')
let data = Buffer.alloc(10);
data[0] = 0;
requester.send(data);