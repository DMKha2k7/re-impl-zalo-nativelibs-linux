'use strict';

const path = require('path');
const os = require('os');
const fs = require('fs');

console.log('=== file-utilities.node Test Suite ===\n');

let mod;
try {
  mod = require('./index.js');
  console.log('✅ Module loaded OK\n');
} catch (e) {
  console.error('❌ Failed to load module:', e.message);
  process.exit(1);
}

// Create a temp directory for reliable, fast testing
const testDir = fs.mkdtempSync(path.join(os.tmpdir(), 'file-utilities-test-'));
const subDir = path.join(testDir, 'subdir');
fs.mkdirSync(subDir);

// Total size: 5 + 7 + 15 = 27 bytes
fs.writeFileSync(path.join(testDir, 'file1.txt'), 'Hello'); // 5 bytes
fs.writeFileSync(path.join(testDir, 'file2.log'), 'World!!'); // 7 bytes
fs.writeFileSync(path.join(subDir, 'file3.txt'), 'Rust & Node-API'); // 15 bytes

let passed = 0;
let failed = 0;

function test(name, fn) {
  try {
    fn();
    console.log(`  ✅ ${name}`);
    passed++;
  } catch (e) {
    console.error(`  ❌ ${name}: ${e.message}`);
    failed++;
  }
}

function assert(cond, msg) {
  if (!cond) throw new Error(msg || 'Assertion failed');
}

// ─── Sync tests ─────────────────────────────────────────────────────────────

console.log('── Sync Functions ──');

test('detectHardlinksSync returns boolean', () => {
  const r = mod.detectHardlinksSync(testDir);
  assert(typeof r === 'boolean', `Expected boolean, got ${typeof r}`);
  console.log(`     └ ${testDir} → hardlinks: ${r}`);
});

test('detectFilesystemSync returns object with required fields', () => {
  const r = mod.detectFilesystemSync(testDir);
  assert(typeof r === 'object' && r !== null, 'Expected object');
  assert('path' in r, 'Missing field: path');
  assert('filesystemType' in r, 'Missing field: filesystemType');
  assert('maxFilenameLength' in r, 'Missing field: maxFilenameLength');
  assert('supportsCaseSensitiveNames' in r, 'Missing field: supportsCaseSensitiveNames');
  assert('supportsUnicodeFilenames' in r, 'Missing field: supportsUnicodeFilenames');
  assert('supportsCompression' in r, 'Missing field: supportsCompression');
  assert('supportsEncryption' in r, 'Missing field: supportsEncryption');
  assert(r.maxFilenameLength > 0, 'maxFilenameLength should be > 0');
  console.log(`     └ fs type: ${r.filesystemType}, maxFilename: ${r.maxFilenameLength}`);
});

test('getDirectorySizeSync returns correct total size', () => {
  const r = mod.getDirectorySizeSync(testDir);
  assert(typeof r === 'number', `Expected number, got ${typeof r}`);
  assert(r === 27, `Expected size 27, got ${r}`);
  console.log(`     └ ${testDir} size: ${r} bytes`);
});

test('getDirectorySizeSync with deep option returns tree structure', () => {
  const r = mod.getDirectorySizeSync(testDir, { deep: true });
  assert(typeof r === 'object', 'Expected object');
  assert('path' in r, 'Missing field: path');
  assert('size' in r, 'Missing field: size');
  assert('isDirectory' in r, 'Missing field: isDirectory');
  assert(r.isDirectory === true, 'isDirectory should be true for dir');
  assert(r.size === 27, `Expected size 27, got ${r.size}`);
  assert(Array.isArray(r.children), 'children should be array');
  console.log(`     └ Tree children count: ${r.children.length}`);
});

test('getDirectorySizeByGlobSync returns matched size', () => {
  const globPattern = path.join(testDir, '*.txt');
  const r = mod.getDirectorySizeByGlobSync(globPattern);
  assert(typeof r === 'number', `Expected number, got ${typeof r}`);
  assert(r === 20, `Expected matched size 20 (*.txt), got ${r}`);
  console.log(`     └ Glob matching *.txt size: ${r} bytes`);
});

// ─── Async tests ─────────────────────────────────────────────────────────────

console.log('\n── Async Functions ──');

async function runAsyncTests() {
  async function asyncTest(name, fn) {
    try {
      await fn();
      console.log(`  ✅ ${name}`);
      passed++;
    } catch (e) {
      console.error(`  ❌ ${name}: ${e.message}`);
      failed++;
    }
  }

  await asyncTest('detectHardlinksAsync returns Promise<boolean>', async () => {
    const r = await mod.detectHardlinksAsync(testDir);
    assert(typeof r === 'boolean', `Expected boolean, got ${typeof r}`);
    console.log(`     └ hardlinks async: ${r}`);
  });

  await asyncTest('detectFilesystemAsync returns Promise<object>', async () => {
    const r = await mod.detectFilesystemAsync(testDir);
    assert(typeof r === 'object' && r !== null, 'Expected object');
    assert('filesystemType' in r, 'Missing filesystemType');
    console.log(`     └ fs type async: ${r.filesystemType}`);
  });

  await asyncTest('getDirectorySizeAsync returns correct total size', async () => {
    const r = await mod.getDirectorySizeAsync(testDir);
    assert(typeof r === 'number', `Expected number, got ${typeof r}`);
    assert(r === 27, `Expected size 27, got ${r}`);
    console.log(`     └ size async: ${r} bytes`);
  });

  await asyncTest('getDirectorySizeAsync with deep option returns tree structure', async () => {
    const r = await mod.getDirectorySizeAsync(testDir, { deep: true });
    assert(typeof r === 'object', 'Expected object');
    assert(r.size === 27, `Expected size 27, got ${r.size}`);
    assert(Array.isArray(r.children), 'children should be array');
    console.log(`     └ tree async: ${r.children.length} entries`);
  });

  await asyncTest('getDirectorySizeByGlobAsync returns matched size', async () => {
    const globPattern = path.join(testDir, '*.txt');
    const r = await mod.getDirectorySizeByGlobAsync(globPattern);
    assert(typeof r === 'number', `Expected number, got ${typeof r}`);
    assert(r === 20, `Expected matched size 20 (*.txt), got ${r}`);
    console.log(`     └ glob async: ${r} bytes`);
  });

  // ─── Cancel test ─────────────────────────────────────────────────────────
  await asyncTest('cancelJob cancels running async job', async () => {
    const abortController = new AbortController();
    const promise = mod.getDirectorySizeAsync(testDir, { abortSignal: abortController.signal });

    // Instantly abort
    abortController.abort();

    try {
      await promise;
      // It might complete if it's too fast, but shouldn't throw an error other than abort
      console.log('     └ cancelJob: Completed before abort took effect');
    } catch (err) {
      assert(err.name === 'AbortError' || err.message.includes('abort') || err.message.includes('cancel'), `Unexpected abort error: ${err.message}`);
      console.log('     └ cancelJob: Correctly aborted');
    }
  });

  // ─── Cleanup ─────────────────────────────────────────────────────────────
  try {
    fs.rmSync(testDir, { recursive: true, force: true });
  } catch (e) {
    // Ignore cleanup errors
  }

  // ─── Results ─────────────────────────────────────────────────────────────
  console.log('\n══════════════════════════════');
  console.log(`Results: ${passed} passed, ${failed} failed`);
  if (failed > 0) {
    process.exit(1);
  } else {
    console.log('All tests passed! 🎉');
  }
}

runAsyncTests().catch(e => {
  console.error('Async test suite error:', e);
  try {
    fs.rmSync(testDir, { recursive: true, force: true });
  } catch (_) {}
  process.exit(1);
});
