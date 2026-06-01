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

console.log('🏁 Building zimage...');

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
  const destWrapper = path.join(DEST_DIR, 'zimage', 'index.js');
  patchFile('zimage/index.js', destWrapper);
} catch (e) {
  console.warn('⚠️ Warning: Could not patch JS wrapper (perhaps nativelibs-macOS is missing):', e.message);
}

const srcBinary = path.join(LIB_DIR, 'build', 'Release', 'zimage.node');
if (!fs.existsSync(srcBinary)) {
  throw new Error(`Compiled addon not found: ${srcBinary}`);
}

// 4. Local copy directory for testing
const localOutDir = path.join(LIB_DIR, 'linux_x64');
if (!fs.existsSync(localOutDir)) {
  fs.mkdirSync(localOutDir, { recursive: true });
}
const localBinary = path.join(localOutDir, 'zimage.node');
fs.copyFileSync(srcBinary, localBinary);
console.log(`✓ Copied local binary to ${localBinary}`);

// 5. Copy to patch destination
const destOutDir = path.join(DEST_DIR, 'zimage', 'linux_x64');
if (!fs.existsSync(destOutDir)) {
  fs.mkdirSync(destOutDir, { recursive: true });
}
const destBinary = path.join(destOutDir, 'zimage.node');
fs.copyFileSync(srcBinary, destBinary);
console.log(`🚀 Copied native binary to ${destBinary}`);

// 6. Gather libvips shared libraries to local and patch directories
console.log('🔍 Gathering libvips shared library dependencies...');
handleLibvipsSO(srcBinary, [localOutDir, destOutDir]);

console.log('✅ zimage build completed.');

function handleLibvipsSO(builtNode, destDirs) {
  let libvipsPath = null;
  try {
    const lddOut = execSync(`ldd "${builtNode}" 2>/dev/null || true`).toString();
    for (const line of lddOut.split('\n')) {
      if (line.includes('libvips')) {
        const m = line.match(/=>\s+(\S+)/);
        if (m) {
          libvipsPath = m[1];
          break;
        }
      }
    }
  } catch (e) { }

  let vipsLibDir = null;
  try {
    const libFlags = execSync('pkg-config --libs-only-L vips 2>/dev/null').toString().trim();
    if (libFlags) {
      vipsLibDir = libFlags.replace('-L', '').trim();
    }
  } catch (e) { }

  const soSearch = [
    '/usr/lib',
    '/usr/lib64',
    '/usr/local/lib',
    vipsLibDir
  ].filter(Boolean);

  const soPatterns = [/^libvips/];
  let copied = 0;

  for (const dir of soSearch) {
    if (!fs.existsSync(dir)) continue;
    let files;
    try {
      files = fs.readdirSync(dir);
    } catch { continue; }

    for (const f of files) {
      if (!soPatterns.some(p => p.test(f))) continue;
      if (!f.includes('.so')) continue;

      const src = path.join(dir, f);

      for (const destDir of destDirs) {
        const dst = path.join(destDir, f);
        try {
          const st = fs.lstatSync(src);
          if (st.isSymbolicLink()) {
            const link = fs.readlinkSync(src);
            try { fs.unlinkSync(dst); } catch { }
            fs.symlinkSync(link, dst);
          } else {
            fs.copyFileSync(src, dst);
          }
          copied++;
        } catch (e) {
          console.warn(`  Failed to copy libvips file ${f} to ${destDir}: ${e.message}`);
        }
      }
    }
  }
  console.log(`✓ Copied ${copied} libvips dependency files to output directories`);
}
