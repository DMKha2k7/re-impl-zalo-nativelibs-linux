'use strict';

const fs = require('fs');
const path = require('path');
const { execSync } = require('child_process');

const LIB_DIR = __dirname;
const ROOT_DIR = path.resolve(LIB_DIR, '../..');
const DEST_DIR = path.resolve(ROOT_DIR, 'nativelibs-linux-patch');

const ELECTRON_VERSION = '22.3.27';
const ELECTRON_DIST_URL = 'https://electronjs.org/headers';
const ELECTRON_ARCH = 'x64';

console.log('🏁 Building mp4thumb...');

// 1. Install dependencies
console.log('🏃 Installing npm dependencies...');
execSync('npm install --ignore-scripts --no-audit --no-fund', { cwd: LIB_DIR, stdio: 'inherit' });

// 2. Compile via node-gyp
console.log('🏃 Rebuilding native addon with node-gyp...');
execSync(`npx node-gyp rebuild --target=${ELECTRON_VERSION} --arch=${ELECTRON_ARCH} --dist-url=${ELECTRON_DIST_URL}`, { cwd: LIB_DIR, stdio: 'inherit' });

// 3. Patch and copy wrapper JS
try {
  const { patchFile } = require(path.join(ROOT_DIR, 'patch-js-helper'));
  console.log('📄 Patching & copying JS wrapper...');
  const destWrapper = path.join(DEST_DIR, 'mp4thumb', 'index.js');
  patchFile('mp4thumb/index.js', destWrapper);
} catch (e) {
  console.warn('⚠️ Warning: Could not patch JS wrapper (perhaps nativelibs-macOS is missing):', e.message);
}

// 4. Copy to patch destination
const srcBinary = path.join(LIB_DIR, 'build', 'Release', 'mp4thumb.node');
if (!fs.existsSync(srcBinary)) {
  throw new Error(`Compiled addon not found: ${srcBinary}`);
}
const destBinary = path.join(DEST_DIR, 'mp4thumb', 'linux-x64', 'mp4thumb.node');
const destBinaryParent = path.dirname(destBinary);
if (!fs.existsSync(destBinaryParent)) {
  fs.mkdirSync(destBinaryParent, { recursive: true });
}
fs.copyFileSync(srcBinary, destBinary);
console.log(`🚀 Copied native binary to ${destBinary}`);

console.log('✅ mp4thumb build completed.');
