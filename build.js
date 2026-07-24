/**
 * build.js - build all native libraries for Linux 
 **/

'use strict';

const fs = require('fs');
const path = require('path');
const { execSync } = require('child_process');
const { patchFile } = require('./src/patch-js-helper');

const PROJECT_DIR = path.resolve(__dirname, '.');
const DEST_DIR = process.env.DEST_DIR || path.resolve(PROJECT_DIR, 'nativelibs');

// Electron target configuration
const ELECTRON_VERSION = '22.3.27';
const ELECTRON_DIST_URL = 'https://electronjs.org/headers';
const ELECTRON_ARCH = 'x64';

// Handle clean command
function clean() {
  console.log(`🧹 Cleaning up old build artifacts in ${PROJECT_DIR}...`);
  const subdirs = [
    'file-utilities',
    'file-utils',
    'mp4thumb',
    'zimage',
    'zjxl',
  ];

  let cleanedCount = 0;
  for (const dir of subdirs) {
    const fullPath = path.join(PROJECT_DIR, 'src', dir);
    if (!fs.existsSync(fullPath)) continue;

    // Folders to clean
    const cleanDirs = ['build', 'target', 'node_modules', 'linux_x64', 'linux'];
    for (const d of cleanDirs) {
      const p = path.join(fullPath, d);
      if (fs.existsSync(p)) {
        console.log(`  Deleting folder: ${path.relative(PROJECT_DIR, p)}`);
        fs.rmSync(p, { recursive: true, force: true });
        cleanedCount++;
      }
    }

    // Files to clean
    try {
      const files = fs.readdirSync(fullPath);
      for (const f of files) {
        if (f === 'Cargo.lock' || f === 'package-lock.json' || f.endsWith('.node')) {
          const p = path.join(fullPath, f);
          console.log(`  Deleting file: ${path.relative(PROJECT_DIR, p)}`);
          fs.unlinkSync(p);
          cleanedCount++;
        }
      }
    } catch (e) {
      // ignore
    }
  }
  console.log(`✨ Cleanup complete! Removed ${cleanedCount} folders/files.`);
}

console.log('🏁 Starting native libraries build process...');
console.log(`📂 Source Build Dir: ${path.join(PROJECT_DIR, 'src')}`);
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

// Define modules to build
const modules = [
  {
    name: 'file-utilities',
    type: 'rust',
    destBinaryDir: path.join(DEST_DIR, 'file-utilities', 'linux'),
    wrappers: [
      { relPath: 'file-utilities/index.js', dest: path.join(DEST_DIR, 'file-utilities', 'index.js') }
    ]
  },
  {
    name: 'file-utils',
    type: 'rust',
    destBinaryDir: path.join(DEST_DIR, 'file-utils', 'linux'),
    wrappers: [
      { relPath: 'file-utils/index.js', dest: path.join(DEST_DIR, 'file-utils', 'index.js') }
    ]
  },
  {
    name: 'mp4thumb',
    type: 'rust',
    destBinaryDir: path.join(DEST_DIR, 'mp4thumb', 'linux-x64'),
    wrappers: [
      { relPath: 'mp4thumb/index.js', dest: path.join(DEST_DIR, 'mp4thumb', 'index.js') }
    ]
  },
  {
    name: 'zimage',
    type: 'rust',
    destBinaryDir: path.join(DEST_DIR, 'zimage', 'linux_x64'),
    wrappers: [
      { relPath: 'zimage/index.js', dest: path.join(DEST_DIR, 'zimage', 'index.js') }
    ]
  },
  {
    name: 'zjxl',
    type: 'rust',
    destBinaryDir: path.join(DEST_DIR, 'zjxl', 'build', 'linux_x64'),
    wrappers: [
      { relPath: 'zjxl/index.js', dest: path.join(DEST_DIR, 'zjxl', 'index.js') }
    ]
  }
];

// Cleanup
if (process.env.CLEAN_BUILD === 'true') {
  clean();
}
// Build and package each module
let moduleCompleted = 0;
for (const mod of modules) {
  const srcDir = path.join(PROJECT_DIR, 'src', mod.name);
  let releaseDir;
  switch (mod.type) {
    case 'rust':
      releaseDir = path.join(srcDir, 'target', 'release');
      break;
    case 'gyp':
      releaseDir = path.join(srcDir, 'build', 'Release');
      break;
    default:
      throw new Error(`❌ Unknown module type: ${mod.type}`);
      continue;
  }

  if (!fs.existsSync(srcDir)) {
    console.warn(`⚠️ Directory for module "${mod.name}" does not exist (${srcDir}). Skipping...`);
    continue;
  }

  console.log(`\n📦 ==================== Building ${mod.name} ====================`);
  let nodeFiles = [];
  try {
    // A. Compilation
    switch (mod.type) {
      case 'gyp':
        if (!fs.existsSync(path.join(srcDir, 'binding.gyp'))) {
          console.warn(`⚠️ binding.gyp does not exist in ${srcDir}. Skipping...`);
          continue;
        }
        try {
          runCmd('npm install --ignore-scripts --no-audit --no-fund', srcDir);
        } catch (error) {
          console.error(`❌ Failed to install dependencies for module "${mod.name}":`, error.message);
          continue;
        }

        try {
          runCmd(`npx node-gyp rebuild --target=${ELECTRON_VERSION} --arch=${ELECTRON_ARCH} --dist-url=${ELECTRON_DIST_URL}`, srcDir);
        } catch (error) {
          console.error(`❌ Failed to build module "${mod.name}":`, error.message);
          continue;
        }

        nodeFiles = fs.readdirSync(releaseDir).filter(file => file.endsWith('.node'));
        if (nodeFiles.length === 0) {
          console.error(`❌ Build succeeded for module "${mod.name}" but binary not found in ${releaseDir}.`);
          continue;
        }

        break;
      case 'rust':
        if (!fs.existsSync(path.join(srcDir, 'Cargo.toml'))) {
          logger.warn(`⚠️ Cargo.toml does not exist in ${srcDir}. Skipping...`);
          continue;
        }
        try {
          runCmd('cargo build --release', srcDir);
        } catch (error) {
          console.error(`❌ Failed to build module "${mod.name}":`, error.message);
          continue;
        }

        const soFiles = fs.readdirSync(path.join(releaseDir)).filter(file => file.includes('lib') && file.endsWith('.so'));
        if (soFiles.length === 0) {
          console.error(`❌ Build succeeded for module "${mod.name}" but binary not found in ${releaseDir}.`);
          continue;
        }

        for (const file of soFiles) {
          const newName = file.replace(/^lib/, '').replace(/\.so$/, '.node').replace(/_/g, '-');
          fs.renameSync(
            path.join(releaseDir, file),
            path.join(releaseDir, newName)
          );
        }
        nodeFiles = fs.readdirSync(releaseDir).filter(file => file.endsWith('.node'));
        break;
      default:
        console.log(`⚠️ Wrong module type or missing module source "${mod.name}". Skipping...`);
        continue;
    }

    // B. Copy and patch wrapper JS files from macOS source
    if (process.env.SKIP_PATCH !== 'true') {
      for (const wrapper of mod.wrappers) {
        console.log(`📄 Patching & copying JS wrapper for ${mod.name} (${wrapper.relPath})...`);
        try {
          patchFile(wrapper.relPath, wrapper.dest);
        } catch (err) {
          console.warn(`⚠️ Warning: Failed to patch JS wrapper for ${mod.name}: ${err.message}`);
        }
      }
    }

    // C. Copy built native binary
    console.log(`🚀 Copying compiled binary to destination path...`);
    for (let file of nodeFiles) { 
      copyFile(path.join(releaseDir, file), path.join(mod.destBinaryDir, file));
    }
    const shippedSOs = fs.readdirSync(releaseDir).filter(file => file.endsWith('.so'));
    if (shippedSOs.length > 0) {
      for (const so of shippedSOs) {
        copyFile(path.join(releaseDir, so), path.join(mod.destBinaryDir, so));
      }
    }

    console.log(`✅ Module ${mod.name} build & package completed successfully.`);
    moduleCompleted++
  } catch (error) {
    console.error(`💥 Failed to build module ${mod.name}:`, error.message);
    continue
  }
}

console.log('\n🎉 ==================================================');
console.log(`🎉 ${moduleCompleted} native libraries built & structured successfully!`);
console.log(`📁 Output located in: ${DEST_DIR}`);
console.log('=====================================================\n');
