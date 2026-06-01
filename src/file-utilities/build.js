'use strict';

const fs = require('fs');
const path = require('path');
const { execSync } = require('child_process');

const LIB_DIR = __dirname;
const ROOT_DIR = path.resolve(LIB_DIR, '../..');
const DEST_DIR = path.resolve(ROOT_DIR, 'nativelibs-linux-patch');

console.log('🏁 Building file-utilities...');

// 1. Compile Rust codebase
console.log('🏃 Running cargo build --release...');
execSync('cargo build --release', { cwd: LIB_DIR, stdio: 'inherit' });

// 2. Local copy (for local tests)
const srcBinary = path.join(LIB_DIR, 'target', 'release', 'libfile_utilities.so');
if (!fs.existsSync(srcBinary)) {
  throw new Error(`Compiled Rust library not found: ${srcBinary}`);
}
const localBinaryDir = path.join(LIB_DIR, 'linux');
if (!fs.existsSync(localBinaryDir)) {
  fs.mkdirSync(localBinaryDir, { recursive: true });
}
const localBinary = path.join(localBinaryDir, 'file-utilities.node');
fs.copyFileSync(srcBinary, localBinary);
console.log(`✓ Copied local binary to ${localBinary}`);

// 3. Patch and copy wrapper JS
try {
  const { patchFile } = require(path.join(ROOT_DIR, 'patch-js-helper'));
  console.log('📄 Patching & copying JS wrapper...');
  const destWrapper = path.join(DEST_DIR, 'file-utilities', 'index.js');
  patchFile('file-utilities/index.js', destWrapper);
} catch (e) {
  console.warn('⚠️ Warning: Could not patch JS wrapper (perhaps nativelibs-macOS is missing):', e.message);
}

// 4. Copy to patch destination
const destBinary = path.join(DEST_DIR, 'file-utilities', 'linux', 'file-utilities.node');
const destBinaryParent = path.dirname(destBinary);
if (!fs.existsSync(destBinaryParent)) {
  fs.mkdirSync(destBinaryParent, { recursive: true });
}
fs.copyFileSync(srcBinary, destBinary);
console.log(`🚀 Copied native binary to ${destBinary}`);

console.log('✅ file-utilities build completed.');
