const fs = require('fs');
const path = require('path');
const { execSync } = require('child_process');

// Electron target configuration
const ELECTRON_VERSION = '22.3.27';
const ELECTRON_DIST_URL = 'https://electronjs.org/headers';
const ELECTRON_ARCH = 'x64';

const VIPS_TAR_URL = 'https://github.com/lovell/sharp-libvips/releases/download/v8.14.5/libvips-8.14.5-linux-x64.tar.gz';
const TAR_FILE_NAME = 'libvips-8.14.5-linux-x64.tar.gz';

const PROJECT_ROOT = __dirname;
const TEMP_DIR = path.join(PROJECT_ROOT, 'temp');
const OUTPUT_DIR = path.join(PROJECT_ROOT, 'linux_x64');
const TAR_PATH = path.join(PROJECT_ROOT, TAR_FILE_NAME);
const INCLUDE_DIR = path.join(PROJECT_ROOT, 'include');
const LIB_DIR = path.join(PROJECT_ROOT, 'lib');
const NODE_FILE = path.join(PROJECT_ROOT, 'build', 'Release', 'zimage.node');
const VIPS_SO_FILE = path.join(LIB_DIR, 'libvips-cpp.so.42');

function cleanUp() {
  console.log('Cleaning up old build artifacts...');
  const itemsToClean = [TEMP_DIR, INCLUDE_DIR, LIB_DIR, 'build', OUTPUT_DIR, TAR_PATH];
  for (const item of itemsToClean) {
    const fullPath = path.isAbsolute(item) ? item : path.join(PROJECT_ROOT, item);
    if (fs.existsSync(fullPath)) {
      fs.rmSync(fullPath, { recursive: true, force: true });
    }
  }
}

function downloadTar() {
  console.log(`Downloading ${VIPS_TAR_URL}...`);
  execSync(`curl -L -s -S -o "${TAR_PATH}" "${VIPS_TAR_URL}"`, { cwd: PROJECT_ROOT });
}

function extractTar() {
  console.log('Extracting include and lib folders from tar archive...');
  execSync(`tar -xzf "${TAR_PATH}" include lib`, { cwd: PROJECT_ROOT });
}

function buildAddon() {
  console.log('Building zimage using libvips-cpp...');
  execSync(
    `npx node-gyp rebuild --target=${ELECTRON_VERSION} --arch=${ELECTRON_ARCH} --dist-url=${ELECTRON_DIST_URL}`,
    { stdio: 'inherit', cwd: PROJECT_ROOT }
  );
}

function shipArtifacts() {
  if (!fs.existsSync(NODE_FILE)) {
    throw new Error(`Built addon file not found at ${NODE_FILE}`);
  }
  if (!fs.existsSync(VIPS_SO_FILE)) {
    throw new Error(`libvips-cpp.so.42 not found at ${VIPS_SO_FILE}`);
  }

  if (!fs.existsSync(OUTPUT_DIR)) {
    fs.mkdirSync(OUTPUT_DIR, { recursive: true });
  }

  const targetNodePath = path.join(OUTPUT_DIR, 'zimage.node');
  const targetSoPath = path.join(OUTPUT_DIR, 'libvips-cpp.so.42');

  console.log(`Shipping ${NODE_FILE} -> ${targetNodePath}`);
  fs.copyFileSync(NODE_FILE, targetNodePath);

  console.log(`Shipping ${VIPS_SO_FILE} -> ${targetSoPath}`);
  fs.copyFileSync(VIPS_SO_FILE, targetSoPath);
}

async function main() {
  try {
    cleanUp();
    downloadTar();
    extractTar();
    buildAddon();
    shipArtifacts();

    console.log('🎉 Build completed successfully!');
    console.log(`- Shipped Addon: ${path.join(OUTPUT_DIR, 'zimage.node')}`);
    console.log(`- Shipped Shared Library: ${path.join(OUTPUT_DIR, 'libvips-cpp.so.42')}`);
  } catch (error) {
    console.error('❌ Build failed:', error.message);
    process.exit(1);
  }
}

main();