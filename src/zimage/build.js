const fs = require('fs');
const path = require('path');
const { execSync } = require('child_process');

// Electron target configuration
const ELECTRON_VERSION = '22.3.27';
const ELECTRON_DIST_URL = 'https://electronjs.org/headers';
const ELECTRON_ARCH = 'x64';

const PROJECT_ROOT = __dirname;
const TEMP_DIR = path.join(PROJECT_ROOT, 'temp');
const OUTPUT_DIR = path.join(PROJECT_ROOT, 'linux_x64');
const LIB_DIR = path.join(OUTPUT_DIR, 'lib');
const NODE_FILE = path.join(PROJECT_ROOT, 'build', 'Release', 'zimage.node');

function cleanUp() {
  console.log('Cleaning up old build artifacts...');
  const dirsToClean = [TEMP_DIR, 'include', 'lib', 'build', OUTPUT_DIR];
  for (const item of dirsToClean) {
    const fullPath = path.isAbsolute(item) ? item : path.join(PROJECT_ROOT, item);
    if (fs.existsSync(fullPath)) {
      fs.rmSync(fullPath, { recursive: true, force: true });
    }
  }
}

// System base libraries to exclude from copying
const IGNORED_PREFIXES = [
  'linux-vdso',
  'libc.',
  'libm.',
  'libpthread',
  'libdl.',
  'librt.',
  'ld-linux',
  'libgcc_s',
  'libstdc++',
  'libselinux',
  'libmount',
  'libblkid',
  'libseccomp',
  'libcrypt.',
  'libresolv',
  'libudev'
];

function isIgnored(basename) {
  return IGNORED_PREFIXES.some((prefix) => basename.startsWith(prefix));
}

function getLddDependencies(filePath) {
  const deps = [];
  try {
    const output = execSync(`ldd "${filePath}"`, { encoding: 'utf8' });
    const lines = output.split('\n');
    for (const line of lines) {
      const match = line.match(/=>\s+(\/\S+)/) || line.trim().match(/^(\/\S+)/);
      if (match && match[1]) {
        deps.push(match[1]);
      }
    }
  } catch (err) {
    console.warn(`Warning: Could not run ldd on ${filePath}: ${err.message}`);
  }
  return deps;
}

function collectUniqueSharedLibs(rootNodeFile) {
  const visited = new Set();
  const queue = [rootNodeFile];
  const sharedLibsMap = new Map(); // soname -> real absolute file path

  while (queue.length > 0) {
    const currentFile = queue.shift();
    if (visited.has(currentFile)) continue;
    visited.add(currentFile);

    const deps = getLddDependencies(currentFile);
    for (const depPath of deps) {
      const soname = path.basename(depPath);
      if (isIgnored(soname)) continue;

      try {
        const realPath = fs.realpathSync(depPath);
        if (!sharedLibsMap.has(soname)) {
          sharedLibsMap.set(soname, realPath);
        }
        if (!visited.has(realPath)) {
          queue.push(realPath);
        }
      } catch (err) {
        console.warn(`Could not resolve real path for ${depPath}: ${err.message}`);
      }
    }
  }

  return sharedLibsMap;
}

function copySharedLibrariesToLibDir(sharedLibsMap, destLibDir) {
  if (!fs.existsSync(destLibDir)) {
    fs.mkdirSync(destLibDir, { recursive: true });
  }

  console.log(`Copying ${sharedLibsMap.size} unique shared libraries into ${destLibDir}...`);

  for (const [soname, realPath] of sharedLibsMap.entries()) {
    try {
      const destPath = path.join(destLibDir, soname);
      fs.copyFileSync(realPath, destPath);
    } catch (err) {
      console.error(`Failed to copy library ${soname} (from ${realPath}): ${err.message}`);
    }
  }
}

async function main() {
  try {
    cleanUp();

    console.log('Building zimage using system libvips...');
    execSync(
      `npx node-gyp rebuild --target=${ELECTRON_VERSION} --arch=${ELECTRON_ARCH} --dist-url=${ELECTRON_DIST_URL}`,
      { stdio: 'inherit', cwd: PROJECT_ROOT }
    );

    if (!fs.existsSync(NODE_FILE)) {
      throw new Error(`Built addon file not found at ${NODE_FILE}`);
    }

    if (!fs.existsSync(OUTPUT_DIR)) {
      fs.mkdirSync(OUTPUT_DIR, { recursive: true });
    }

    // 1. Copy zimage.node into linux_x64/
    const targetNodePath = path.join(OUTPUT_DIR, 'zimage.node');
    console.log(`Copying ${NODE_FILE} -> ${targetNodePath}`);
    fs.copyFileSync(NODE_FILE, targetNodePath);

    // 2. Collect unique .so dependencies and copy directly into linux_x64/lib/
    const sharedLibs = collectUniqueSharedLibs(NODE_FILE);
    copySharedLibrariesToLibDir(sharedLibs, LIB_DIR);

    console.log('🎉 Build completed successfully!');
    console.log(`- Shipped Addon: ${targetNodePath}`);
    const libFiles = fs.readdirSync(LIB_DIR);
    console.log(`- Shipped Libraries: ${libFiles.length} files in ${LIB_DIR}`);

  } catch (error) {
    console.error('❌ Build failed:', error.message);
    process.exit(1);
  }
}

main();