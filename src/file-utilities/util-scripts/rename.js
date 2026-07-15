const fs = require('fs');
const path = require('path');

const srcPath = path.join(__dirname, '../target/release/libfile_utilities.so');
const srcDebugPath = path.join(__dirname, '../target/debug/libfile_utilities.so');
const destDir = path.join(__dirname, '../../../nativelibs/file-utilities/linux');
const destPath = path.join(destDir, 'file-utilities.node');

if (!fs.existsSync(destDir)) {
  fs.mkdirSync(destDir, { recursive: true });
}

let src = srcPath;
if (!fs.existsSync(srcPath)) {
  if (fs.existsSync(srcDebugPath)) {
    src = srcDebugPath;
  } else {
    console.error('Error: Built library not found at ' + srcPath + ' or ' + srcDebugPath);
    process.exit(1);
  }
}

try {
  fs.copyFileSync(src, destPath);
  console.log('Successfully copied ' + src + ' to ' + destPath);
} catch (e) {
  console.error('Failed to copy built library:', e);
  process.exit(1);
}
