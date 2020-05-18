'use strict';
const sample_size   = 4;
const buffer_size   = 8 * sample_size;
let data            = [0, 0, 0, 0];
let binBuffer       = '';

function parse_data(raw) {
    // FIX ME, need to accumulate
    binBuffer = raw;
    if (binBuffer.length >= buffer_size) {
        let buffer = Buffer.from(binBuffer, 0, buffer_size);
        if (binBuffer.length > buffer_size) {
            binBuffer = binBuffer.substring(buffer_size);
        } else {
            binBuffer = '';
        }
        for (let i = 0; i < sample_size; i++) {
            data[i] = buffer.readDoubleLE(i*8);
        }
    } 
    return data;
}

exports.parse_data = parse_data;