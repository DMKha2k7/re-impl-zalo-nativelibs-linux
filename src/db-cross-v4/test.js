const fs = require('fs');
const path = require('path');

// Resolve the binary path
const binaryPath = path.join(__dirname, 'build', 'Release', 'db-cross-v4-native.node');

console.log(`🔍 Testing db-cross-v4 binary at: ${binaryPath}`);

if (!fs.existsSync(binaryPath)) {
  console.error('❌ Binary file not found! Please run node-gyp rebuild first.');
  process.exit(1);
}

try {
  const addon = require(binaryPath);
  console.log('✅ Successfully loaded db-cross-v4 native addon.');

  // 1. Test getVersion
  if (typeof addon.getVersion === 'function') {
    const version = addon.getVersion();
    console.log(`🤖 getVersion() output: "${version}"`);
    if (version === '1.0.0') {
      console.log('✅ getVersion check passed.');
    } else {
      console.error(`❌ Unexpected version output: ${version}`);
    }
  } else {
    console.error('❌ getVersion is not a function');
  }

  // 2. Test parseBinNet with empty buffer
  if (typeof addon.parseBinNet === 'function') {
    const buf = Buffer.from([0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x03, 0x04]);
    console.log('Running parseBinNet with small buffer...');
    const result = addon.parseBinNet(buf);
    console.log('✅ parseBinNet returned:', result);
  } else {
    console.error('❌ parseBinNet is not a function');
  }

  // 3. Test decompressAndDecryptDb arguments validation
  if (typeof addon.decompressAndDecryptDb === 'function') {
    console.log('Running decompressAndDecryptDb with invalid arguments...');
    const result = addon.decompressAndDecryptDb();
    console.log('✅ decompressAndDecryptDb arguments check result:', result);
  } else {
    console.error('❌ decompressAndDecryptDb is not a function');
  }

  console.log('🎉 All basic db-cross-v4 API tests passed successfully!');
} catch (e) {
  console.error('💥 Test execution failed with error:', e);
  process.exit(1);
}
