/**
 * build.js
 * Orchestrates compilation and packaging of all 7 native libraries for Linux.
 * Outputs the built modules with exact directory structure and JS wrappers into project's `nativelibs/` folder.
 */

'use strict';

const fs = require('fs');
const path = require('path');
const { execSync } = require('child_process');
const { patchFile } = require('./patch-js-helper');

const ROOT_DIR = path.resolve(__dirname, '.');
const NATIVELIBS_BUILD_DIR = path.resolve(ROOT_DIR);
const DEST_DIR = path.resolve(ROOT_DIR, 'nativelibs-linux-patch');

// Handle clean command
if (process.argv.includes('clean')) {
  console.log(`🧹 Cleaning up old build artifacts in ${NATIVELIBS_BUILD_DIR}...`);
  const subdirs = [
    'db-cross-v4',
    'file-utilities',
    'file-utils',
    'mp4thumb',
    'zimage',
    'zjxl',
    // 'zwalker'
  ];

  let cleanedCount = 0;
  for (const dir of subdirs) {
    const fullPath = path.join(NATIVELIBS_BUILD_DIR, 'src', dir);
    if (!fs.existsSync(fullPath)) continue;

    // Folders to clean
    const cleanDirs = ['build', 'target', 'node_modules', 'linux_x64'];
    for (const d of cleanDirs) {
      const p = path.join(fullPath, d);
      if (fs.existsSync(p)) {
        console.log(`  Deleting folder: ${path.relative(ROOT_DIR, p)}`);
        fs.rmSync(p, { recursive: true, force: true });
        cleanedCount++;
      }
    }

    // Files to clean
    try {
      const files = fs.readdirSync(fullPath);
      for (const f of files) {
        if (f.endsWith('.node') || (dir === 'mp4thumb' && f.endsWith('.jpg'))) {
          const p = path.join(fullPath, f);
          console.log(`  Deleting file: ${path.relative(ROOT_DIR, p)}`);
          fs.unlinkSync(p);
          cleanedCount++;
        }
      }
    } catch (e) {
      // ignore
    }
  }
  console.log(`✨ Cleanup complete! Removed ${cleanedCount} folders/files.`);
  process.exit(0);
}

console.log('🏁 Starting native libraries build process...');
console.log(`📂 Source Build Dir: ${NATIVELIBS_BUILD_DIR}`);
console.log(`📂 Destination Dir:  ${DEST_DIR}`);

// 1. Clean and recreate the target `nativelibs` directory
if (fs.existsSync(DEST_DIR)) {
  console.log(`🧹 Cleaning existing nativelibs directory: ${DEST_DIR}`);
  fs.rmSync(DEST_DIR, { recursive: true, force: true });
}
fs.mkdirSync(DEST_DIR, { recursive: true });

// Helper to run commands with log
function runCmd(cmd, cwd) {
  console.log(`🏃 Running: "${cmd}" in ${cwd}`);
  execSync(cmd, { cwd, stdio: 'inherit' });
}

// Helper to copy file ensuring parent directory exists
function copyFile(src, dst) {
  const parent = path.dirname(dst);
  if (!fs.existsSync(parent)) {
    fs.mkdirSync(parent, { recursive: true });
  }
  fs.copyFileSync(src, dst);
}

// Helper to copy directory recursive
function copyDir(src, dst) {
  if (!fs.existsSync(src)) return;
  const parent = path.dirname(dst);
  if (!fs.existsSync(parent)) {
    fs.mkdirSync(parent, { recursive: true });
  }
  if (fs.lstatSync(src).isDirectory()) {
    if (!fs.existsSync(dst)) {
      fs.mkdirSync(dst, { recursive: true });
    }
    const files = fs.readdirSync(src);
    for (const f of files) {
      copyDir(path.join(src, f), path.join(dst, f));
    }
  } else {
    fs.copyFileSync(src, dst);
  }
}

// Define modules to build
const modules = [
  {
    name: 'db-cross-v4',
    type: 'gyp',
    buildDir: path.join(NATIVELIBS_BUILD_DIR, 'src', 'db-cross-v4'),
    srcBinary: path.join(NATIVELIBS_BUILD_DIR, 'src', 'db-cross-v4', 'build', 'Release', 'db-cross-v4-native.node'),
    destBinary: path.join(DEST_DIR, 'db-cross-v4', 'prebuilt', 'linux', 'electron_x86_64', 'db-cross-v4-native.node'),
    wrappers: [
      { relPath: 'db-cross-v4/dist/binding.js', dest: path.join(DEST_DIR, 'db-cross-v4', 'dist', 'binding.js') }
    ]
  },
  {
    name: 'file-utilities',
    type: 'rust',
    buildDir: path.join(NATIVELIBS_BUILD_DIR, 'src', 'file-utilities'),
    srcBinary: path.join(NATIVELIBS_BUILD_DIR, 'src', 'file-utilities', 'target', 'release', 'libfile_utilities.so'),
    destBinary: path.join(DEST_DIR, 'file-utilities', 'linux', 'file-utilities.node'),
    wrappers: [
      { relPath: 'file-utilities/index.js', dest: path.join(DEST_DIR, 'file-utilities', 'index.js') }
    ]
  },
  {
    name: 'file-utils',
    type: 'gyp',
    buildDir: path.join(NATIVELIBS_BUILD_DIR, 'src', 'file-utils'),
    srcBinary: path.join(NATIVELIBS_BUILD_DIR, 'src', 'file-utils', 'build', 'Release', 'file-utils.node'),
    destBinary: path.join(DEST_DIR, 'file-utils', 'linux', 'file-utils.node'),
    wrappers: [
      { relPath: 'file-utils/index.js', dest: path.join(DEST_DIR, 'file-utils', 'index.js') }
    ]
  },
  {
    name: 'mp4thumb',
    type: 'gyp',
    buildDir: path.join(NATIVELIBS_BUILD_DIR, 'src', 'mp4thumb'),
    srcBinary: path.join(NATIVELIBS_BUILD_DIR, 'src', 'mp4thumb', 'build', 'Release', 'mp4thumb.node'),
    destBinary: path.join(DEST_DIR, 'mp4thumb', 'linux-x64', 'mp4thumb.node'),
    wrappers: [
      { relPath: 'mp4thumb/index.js', dest: path.join(DEST_DIR, 'mp4thumb', 'index.js') }
    ]
  },
  {
    name: 'zimage',
    type: 'gyp-custom', // requires custom SO copying
    buildDir: path.join(NATIVELIBS_BUILD_DIR, 'src', 'zimage'),
    srcBinary: path.join(NATIVELIBS_BUILD_DIR, 'src', 'zimage', 'build', 'Release', 'zimage.node'),
    destBinary: path.join(DEST_DIR, 'zimage', 'linux_x64', 'zimage.node'),
    wrappers: [
      { relPath: 'zimage/index.js', dest: path.join(DEST_DIR, 'zimage', 'index.js') }
    ]
  },
  {
    name: 'zjxl',
    type: 'gyp-custom', // requires custom SO copying
    buildDir: path.join(NATIVELIBS_BUILD_DIR, 'src', 'zjxl'),
    srcBinary: path.join(NATIVELIBS_BUILD_DIR, 'src', 'zjxl', 'build', 'Release', 'jxl.node'),
    destBinary: path.join(DEST_DIR, 'zjxl', 'build', 'linux_x64', 'jxl.node'),
    wrappers: [
      { relPath: 'zjxl/index.js', dest: path.join(DEST_DIR, 'zjxl', 'index.js') }
    ]
  },
  {
    // name: 'zwalker',
    // type: 'rust',
    // buildDir: path.join(NATIVELIBS_BUILD_DIR, 'src', 'zwalker'),
    // srcBinary: path.join(NATIVELIBS_BUILD_DIR, 'src', 'zwalker', 'target', 'release', 'libzwalker.so'),
    // destBinary: path.join(DEST_DIR, 'zwalker', 'zwalker.linux-x64-gnu.node'),
    // wrappers: [
    //   { relPath: 'zwalker/index.js', dest: path.join(DEST_DIR, 'zwalker', 'index.js') }
    // ]
  }
];

// Build and package each module
for (const mod of modules) {
  console.log(`\n📦 ==================== Building ${mod.name} ====================`);
  try {
    // A. Compilation
    if (mod.type === 'gyp' || mod.type === 'gyp-custom') {
      runCmd('npm install --no-audit --no-fund', mod.buildDir);
      runCmd('npx node-gyp rebuild', mod.buildDir);
    } else if (mod.type === 'rust') {
      runCmd('cargo build --release', mod.buildDir);
    }

    // B. Copy and patch wrapper JS files from macOS source
    for (const wrapper of mod.wrappers) {
      console.log(`📄 Patching & copying JS wrapper for ${mod.name} (${wrapper.relPath})...`);
      patchFile(wrapper.relPath, wrapper.dest);
    }

    // C. Copy built native binary
    if (!fs.existsSync(mod.srcBinary)) {
      throw new Error(`Compiled binary not found: ${mod.srcBinary}`);
    }
    console.log(`🚀 Copying compiled binary to destination path...`);
    copyFile(mod.srcBinary, mod.destBinary);

    // D. Custom Shared Libraries handling for specific modules
    if (mod.name === 'zimage') {
      handleZimageSO(mod.srcBinary, path.dirname(mod.destBinary));
    } else if (mod.name === 'zjxl') {
      handleZjxlSO(mod.destBinary, path.dirname(mod.destBinary));
    }

    console.log(`✅ Module ${mod.name} build & package completed successfully.`);
  } catch (error) {
    console.error(`💥 Failed to build module ${mod.name}:`, error.message);
    process.exit(1);
  }
}

console.log('\n🎉 ==================================================');
console.log('🎉 All native libraries built & structured successfully!');
console.log(`📁 Output located in: ${DEST_DIR}`);
console.log('=====================================================\n');

/**
 * Copies libvips shared libraries from the system for zimage
 */
function handleZimageSO(builtNode, destDir) {
  console.log('🔍 Gathering libvips shared library dependencies for zimage...');

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
        console.warn(`  Failed to copy libvips file ${f}: ${e.message}`);
      }
    }
  }
  console.log(`✓ Copied ${copied} libvips dependency files to ${destDir}`);
}

/**
 * Copies libjxl, opencv, brotli, hwy, etc. dependencies recursively using ldd
 */
function handleZjxlSO(builtNode, destDir) {
  console.log('🔍 Gathering libjxl and OpenCV shared library dependencies for zjxl...');

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
            console.warn(`  Failed to copy dependency ${libName}: ${err.message}`);
          }
        }
      }
    } catch (e) {
      console.warn(`  Failed to trace dependencies on ${currentFile}: ${e.message}`);
    }
  }
  console.log(`✓ Copied ${copied} JXL/OpenCV dependency files to ${destDir}`);
}
