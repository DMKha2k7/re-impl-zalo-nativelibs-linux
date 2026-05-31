const fs = require('fs');
const path = require('path');
const zjxl = require('./index.js');

async function runTests() {
  console.log('--- Starting zjxl tests ---');

  // 1. Test moduleReady
  const ready = await zjxl.moduleReady();
  console.log('moduleReady:', ready);
  if (!ready) {
    console.error('FAIL: Module is not ready');
    process.exit(1);
  }

  // Create sample RGB24 bitmap (gradient image 200x200)
  const width = 200;
  const height = 200;
  const rgbBuffer = Buffer.alloc(width * height * 3);
  for (let y = 0; y < height; y++) {
    for (let x = 0; x < width; x++) {
      const idx = (y * width + x) * 3;
      rgbBuffer[idx] = (x * 255) / width;      // R
      rgbBuffer[idx + 1] = (y * 255) / height;  // G
      rgbBuffer[idx + 2] = 128;                 // B
    }
  }

  // 2. Test bitmapToJxl
  console.log('Testing bitmapToJxl...');
  const encodeRes = await zjxl.bitmapToJxl(rgbBuffer, width, height);
  console.log('bitmapToJxl success, JXL buffer size:', encodeRes.data.length, 'status_code:', encodeRes.status_code);
  const jxlBuffer = encodeRes.data;

  // 3. Test getJxlInfo
  console.log('Testing getJxlInfo...');
  const info = await zjxl.getJxlInfo(jxlBuffer);
  console.log('getJxlInfo:', info);
  if (info.width !== width || info.height !== height) {
    console.error(`FAIL: expected ${width}x${height}, got ${info.width}x${info.height}`);
    process.exit(1);
  }

  // 4. Test decodeToJpeg (no resize)
  console.log('Testing decodeToJpeg (no resize)...');
  const jpegRes = await zjxl.decodeToJpeg(jxlBuffer, 0.85);
  console.log('decodeToJpeg success, JPEG buffer size:', jpegRes.data.length, 'status_code:', jpegRes.status_code);
  fs.writeFileSync(path.join(__dirname, 'output_test.jpg'), jpegRes.data);
  console.log('Saved output_test.jpg');

  // 5. Test decodeToJpeg (with resize)
  console.log('Testing decodeToJpeg (with resize to 100x100)...');
  const jpegResResize = await zjxl.decodeToJpeg(jxlBuffer, 0.85, { outputWidth: 100, outputHeight: 100 });
  console.log('decodeToJpeg with resize success, JPEG buffer size:', jpegResResize.data.length, 'status_code:', jpegResResize.status_code);
  fs.writeFileSync(path.join(__dirname, 'output_test_100x100.jpg'), jpegResResize.data);

  // 6. Test resizeJxl
  console.log('Testing resizeJxl to 50x50...');
  const resizeRes = await zjxl.resizeJxl(jxlBuffer, 50, 50);
  console.log('resizeJxl success, resized JXL buffer size:', resizeRes.data.length, 'status_code:', resizeRes.status_code);
  
  // Verify resized info
  const resizedInfo = await zjxl.getJxlInfo(resizeRes.data);
  console.log('Resized JXL info:', resizedInfo);
  if (resizedInfo.width !== 50 || resizedInfo.height !== 50) {
    console.error(`FAIL: expected 50x50, got ${resizedInfo.width}x${resizedInfo.height}`);
    process.exit(1);
  }

  // 7. Test resizeJxlLimit
  console.log('Testing resizeJxlLimit to 80x80...');
  const resizeLimitRes = await zjxl.resizeJxlLimit(jxlBuffer, 80, 80, 4);
  console.log('resizeJxlLimit success, status_code:', resizeLimitRes.status_code);

  // 8. Test jxlDecompressMulti (Buffer tasks)
  console.log('Testing jxlDecompressMulti (Buffer tasks)...');
  const multiRes = await zjxl.jxlDecompressMulti({
    buffer: jxlBuffer,
    quality: 0.9,
    tasks: [
      { width: 150, height: 150 },
      { maxWidth: 100, maxHeight: 100 }
    ]
  });
  console.log('jxlDecompressMulti success, tasks returned:', multiRes.data.length, 'status_code:', multiRes.status_code);
  multiRes.data.forEach((taskRes, index) => {
    console.log(`Task ${index}: width=${taskRes.width}, height=${taskRes.height}, size=${taskRes.size}, hasBuffer=${!!taskRes.data}`);
  });

  // 9. Test jxlDecompressMulti (File tasks)
  console.log('Testing jxlDecompressMulti (File tasks)...');
  const localJxlPath = path.join(__dirname, 'temp_test.jxl');
  fs.writeFileSync(localJxlPath, jxlBuffer);
  
  const outPath1 = path.join(__dirname, 'multi_out_1.jpg');
  const outPath2 = path.join(__dirname, 'multi_out_2.jpg');

  const multiFileRes = await zjxl.jxlDecompressMulti({
    localPath: localJxlPath,
    quality: 0.85,
    tasks: [
      { width: 120, height: 120, outputPath: outPath1 },
      { maxWidth: 60, maxHeight: 60, outputPath: outPath2 }
    ]
  });
  console.log('jxlDecompressMulti File success, status_code:', multiFileRes.status_code);
  
  // Clean up
  try {
    fs.unlinkSync(localJxlPath);
    fs.unlinkSync(outPath1);
    fs.unlinkSync(outPath2);
    fs.unlinkSync(path.join(__dirname, 'output_test.jpg'));
    fs.unlinkSync(path.join(__dirname, 'output_test_100x100.jpg'));
  } catch (e) {}

  console.log('--- All tests passed successfully ---');
}

runTests().catch(err => {
  console.error('Tests failed:', err);
  process.exit(1);
});
