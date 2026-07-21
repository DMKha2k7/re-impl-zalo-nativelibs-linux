const fs = require('fs');
const path = require('path');
const https = require('https');
const { execSync } = require('child_process');

// Electron target configuration
const ELECTRON_VERSION = '22.3.27';
const ELECTRON_DIST_URL = 'https://electronjs.org/headers';
const ELECTRON_ARCH = 'x64';

const PROJECT_ROOT = __dirname;
const TEMP_DIR = path.join(PROJECT_ROOT, 'temp');
const VIPS_TAR_URL = 'https://github.com/lovell/sharp-libvips/releases/download/v8.14.5/libvips-8.14.5-linux-x64.tar.gz';
const VIPS_TAR_PATH = path.join(TEMP_DIR, 'libvips.tar.gz');


function downloadFile(url, destPath) {
  return new Promise((resolve, reject) => {
    https.get(url, (response) => {
      if (response.statusCode === 301 || response.statusCode === 302) {
        return downloadFile(response.headers.location, destPath).then(resolve).catch(reject);
      }

      if (response.statusCode !== 200) {
        return reject(new Error(`Download failed. Status Code: ${response.statusCode}`));
      }

      const fileStream = fs.createWriteStream(destPath);
      response.pipe(fileStream);

      fileStream.on('finish', () => {
        fileStream.close();
        resolve();
      });

      fileStream.on('error', (err) => {
        fs.unlink(destPath, () => {});
        reject(err);
      });
    }).on('error', reject);
  });
}
function cleanUp() {
  console.log('Cleaning up...');
  if (fs.existsSync(TEMP_DIR)) {
    fs.rmSync(TEMP_DIR, { recursive: true, force: true });
  }
  if (fs.existsSync("include")) {
    fs.rmSync("include", { recursive: true, force: true });
  }
  if (fs.existsSync("lib")) {
    fs.rmSync("lib", { recursive: true, force: true });
  }
  if (fs.existsSync("build")) {
    fs.rmSync("build", { recursive: true, force: true });
  }
}

async function main() {
  try {
    cleanUp();
    if (!fs.existsSync(TEMP_DIR)) {
      fs.mkdirSync(TEMP_DIR, { recursive: true });
    }

    console.log(`Downloading libvips from: ${VIPS_TAR_URL}`);
    await downloadFile(VIPS_TAR_URL, VIPS_TAR_PATH);

    execSync(`tar -zxvf "${VIPS_TAR_PATH}" -C "${PROJECT_ROOT}" include lib `, { stdio: 'inherit' });

    console.log('Building zimage...');
    execSync(`npx node-gyp rebuild --target=${ELECTRON_VERSION} --arch=${ELECTRON_ARCH} --dist-url=${ELECTRON_DIST_URL}`, { stdio: 'inherit', cwd: PROJECT_ROOT });
    console.log('🎉 Build completed!');
    process.exit(0);
  } catch (error) {
    console.error('❌ Build failed :', error.message);
    process.exit(1);
  }
}

main();