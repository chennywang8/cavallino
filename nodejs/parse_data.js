'use strict';
const sample_size   = 4;
const buffer_size   = 8 * sample_size;
let data            = [0, 0, 0, 0];
let buffer          = Buffer.alloc(0, 0, 'binary');

function parse_data(raw) {
    buffer = Buffer.concat([buffer, raw]);
    if (buffer.length >= buffer_size) {
        for (let i = 0; i < sample_size; i++) {
            data[i] = buffer.readDoubleLE(i*8);
        }
        buffer = buffer.subarray(buffer_size);
    } 
    return data;
}

module.exports = parse_data;