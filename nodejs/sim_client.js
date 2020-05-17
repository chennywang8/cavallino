const net       = require('net');
const client    = net.connect('/tmp/sim_socket', () => {
    console.log('connect to server...');
});

client.on('data', (data) => {
    console.log(data.toString());
});

client.on('end', () => {
    console.log('disconnected from server...');
});