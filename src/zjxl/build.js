'use strict';

const { copyFileSync, existsSync, mkdirSync, rmSync } = require('fs');
const { dirname, join } = require('path');
const { execFileSync } = require('child_process');

const PROJECT_DIR = __dirname;
const ARTIFACT = join(PROJECT_DIR, 'target', 'release', 'libzjxl.so');
const ADDON = join(PROJECT_DIR,'linux_x64', 'jxl.node');

function clean() {
  rmSync(join(PROJECT_DIR, 'target'), { recursive: true, force: true });
  rmSync(ADDON, { force: true });
}

function build() {
  execFileSync('cargo', ['build', '--release'], { cwd: PROJECT_DIR, stdio: 'inherit' });
  if (!existsSync(ARTIFACT)) throw new Error(`Missing build artifact: ${ARTIFACT}`);
  mkdirSync(dirname(ADDON), { recursive: true });
  copyFileSync(ARTIFACT, ADDON);
  console.log(`Built ${ADDON}`);
  return ADDON;
}

module.exports = { ADDON, build, clean };

if (require.main === module) {
  if (process.argv.includes('clean')) clean();
  else build();
}
