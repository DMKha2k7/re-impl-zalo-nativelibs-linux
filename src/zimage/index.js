'use strict';

const path = require('path');
const fs   = require('fs');

// Load from linux_x64/ (matches original zimage structure)
const nodePath = path.join(__dirname, 'linux_x64', 'zimage.node');

if (!fs.existsSync(nodePath)) {
    throw new Error(
        `zimage.node not found at ${nodePath}\n` +
        'Run: npm run build'
    );
}

// Set RPATH so libvips.so.42 is found in same directory
process.env.LD_LIBRARY_PATH = [
    path.join(__dirname, 'linux_x64'),
    process.env.LD_LIBRARY_PATH || ''
].filter(Boolean).join(':');

const native = require(nodePath);

/**
 * thumbnail(buffer, width, height, format, quality) → Promise<Buffer>
 */
function thumbnail(buffer, width, height, format, quality) {
    if (quality === undefined || quality === null) quality = 80;
    if (format  === undefined || format  === null) format  = 'jpeg';
    return new Promise((resolve, reject) => {
        native.thumbnail({ buffer, width, height, format, quality }, (err, result) => {
            if (err) reject(err);
            else resolve(result);
        });
    });
}

/**
 * thumbnailFs(inputPath, outputPath, width, height, quality) → Promise<void>
 */
function thumbnailFs(inputPath, outputPath, width, height, quality) {
    if (quality === undefined || quality === null) quality = 80;
    return new Promise((resolve, reject) => {
        native.thumbnailFs({ inputPath, outputPath, width, height, quality }, (err) => {
            if (err) reject(err);
            else resolve();
        });
    });
}

module.exports = { thumbnail, thumbnailFs, _native: native };
