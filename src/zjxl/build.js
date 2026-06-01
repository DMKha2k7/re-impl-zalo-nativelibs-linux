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

console.log('🏁 Building zjxl...');

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
  const destWrapper = path.join(DEST_DIR, 'zjxl', 'index.js');
  patchFile('zjxl/index.js', destWrapper);
} catch (e) {
  console.warn('⚠️ Warning: Could not patch JS wrapper (perhaps nativelibs-macOS is missing):', e.message);
}

const srcBinary = path.join(LIB_DIR, 'build', 'Release', 'jxl.node');
if (!fs.existsSync(srcBinary)) {
  throw new Error(`Compiled addon not found: ${srcBinary}`);
}

// 4. Local copy directory for testing
const localOutDir = path.join(LIB_DIR, 'build', 'linux_x64');
if (!fs.existsSync(localOutDir)) {
  fs.mkdirSync(localOutDir, { recursive: true });
}
const localBinary = path.join(localOutDir, 'jxl.node');
fs.copyFileSync(srcBinary, localBinary);
console.log(`✓ Copied local binary to ${localBinary}`);

// 5. Copy to patch destination
const destOutDir = path.join(DEST_DIR, 'zjxl', 'build', 'linux_x64');
if (!fs.existsSync(destOutDir)) {
  fs.mkdirSync(destOutDir, { recursive: true });
}
const destBinary = path.join(destOutDir, 'jxl.node');
fs.copyFileSync(srcBinary, destBinary);
console.log(`🚀 Copied native binary to ${destBinary}`);

// 6. Gather OpenCV, libjxl, etc. shared libraries to output directories
console.log('🔍 Gathering JXL and OpenCV dependency shared libraries...');
handleZjxlSO(srcBinary, [localOutDir, destOutDir]);

console.log('✅ zjxl build completed.');

function handleZjxlSO(builtNode, destDirs) {
  const copiedDeps = new Set();
  const queue = [builtNode];
  const targets = [
    'libjxl',
    'libopencv_core',
    'libopencv_imgproc',
    'libjpeg',
    'libturbojpeg',
    'libbrotli',
    'libhwy'
  ];

  let copied = 0;

  while (queue.length > 0) {
    const currentFile = queue.shift();
    try {
      const lddOut = execSync(`ldd "${currentFile}" 2>/dev/null || true`).toString();
      for (const line of lddOut.split('\n')) {
        const m = line.match(/=>\s+(\S+)/);
        if (m) {
          const srcPath = m[1];
          if (!fs.existsSync(srcPath)) continue;

          const libName = path.basename(srcPath);
          const isTarget = targets.some(t => libName.includes(t));
          if (!isTarget) continue;

          const realSrc = fs.realpathSync(srcPath);
          if (copiedDeps.has(realSrc)) continue;

          copiedDeps.add(realSrc);

          for (const destDir of destDirs) {
            const dstPath = path.join(destDir, libName);

            if (fs.existsSync(dstPath)) {
              const realDst = fs.realpathSync(dstPath);
              if (realSrc === realDst) continue;
            }

            try {
              if (fs.existsSync(dstPath)) {
                fs.unlinkSync(dstPath);
              }
              fs.copyFileSync(realSrc, dstPath);
              copied++;
              queue.push(dstPath);
            } catch (err) {
              console.warn(`  Failed to copy dependency ${libName} to ${destDir}: ${err.message}`);
            }
          }
        }
      }
    } catch (e) {
      console.warn(`  Failed to trace dependencies on ${currentFile}: ${e.message}`);
    }
  }
  console.log(`✓ Copied ${copied} dependency files to output directories`);
}
