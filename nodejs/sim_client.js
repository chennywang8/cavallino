'use strict';
const net       = require('net');
const spawn     = require('child_process').spawn;
const parse     = require('./parse_data');
let   simData   = [0, 0, 0, 0];

// ----------------------------
// launch sim server
const sim       = spawn('../sim/sim');
sim.stdout.on('data', (data) => {
    console.log(`server-> ${data}`);
});

// ----------------------------
// launch sim client
const client    = net.connect('/tmp/sim_socket', () => {
    console.log('connect to server...');
});

client.on('data', (raw) => {
    simData = parse(raw);
    console.log(`${simData} @ ${new Date(simData[3]*1000)}`);
});

client.on('end', () => {
    console.log('disconnected from server...');
});