'use strict';

const path = require('path');
const fs = require('fs');

// file-utilities.node is placed in the project root after `npm run build`
const nodePath = path.join(__dirname, 'file-utilities.node');

if (!fs.existsSync(nodePath)) {
  throw new Error(
    `file-utilities.node not found. Run:\n` +
    `  cargo build --release && cp target/release/libfile_utilities.so file-utilities.node`
  );
}

module.exports = require(nodePath);
