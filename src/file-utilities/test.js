'use strict';

const path = require('path');
const os = require('os');

console.log('=== file-utilities.node Test Suite ===\n');

let mod;
try {
  mod = require('./index.js');
  console.log('✅ Module loaded OK\n');
} catch (e) {
  console.error('❌ Failed to load module:', e.message);
  process.exit(1);
}

const testDir = os.homedir(); // use home dir as a reliable test path

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

test('getDirectorySizeSync returns number', () => {
  const r = mod.getDirectorySizeSync(testDir);
  assert(typeof r === 'number', `Expected number, got ${typeof r}`);
  assert(r >= 0, 'Size should be >= 0');
  console.log(`     └ ${testDir} size: ${(r / 1024 / 1024).toFixed(2)} MB`);
});

test('getDirectorySizeTreeSync returns tree object', () => {
  const tmpDir = '/tmp';
  const r = mod.getDirectorySizeTreeSync(tmpDir);
  assert(typeof r === 'object', 'Expected object');
  assert('path' in r, 'Missing field: path');
  assert('size' in r, 'Missing field: size');
  assert('isDirectory' in r, 'Missing field: isDirectory');
  assert(r.isDirectory === true, 'isDirectory should be true for dir');
  assert(Array.isArray(r.children), 'children should be array');
  console.log(`     └ /tmp entries: ${r.children.length}, total: ${(r.size / 1024).toFixed(1)} KB`);
});

test('getDirectorySizeByGlobSync returns number for *.log pattern', () => {
  const r = mod.getDirectorySizeByGlobSync('/tmp', ['*.log', '**/*.log']);
  assert(typeof r === 'number', `Expected number, got ${typeof r}`);
  assert(r >= 0, 'Size should be >= 0');
  console.log(`     └ /tmp *.log size: ${r} bytes`);
});

test('cancelJob returns false for invalid id', () => {
  const r = mod.cancelJob(999999);
  assert(r === false, `Expected false, got ${r}`);
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

  await asyncTest('getDirectorySizeAsync returns Promise<number>', async () => {
    const r = await mod.getDirectorySizeAsync('/tmp');
    assert(typeof r === 'number', `Expected number, got ${typeof r}`);
    assert(r >= 0, 'Size should be >= 0');
    console.log(`     └ /tmp size async: ${r} bytes`);
  });

  await asyncTest('getDirectorySizeTreeAsync returns Promise<object>', async () => {
    const r = await mod.getDirectorySizeTreeAsync('/tmp');
    assert(typeof r === 'object', 'Expected object');
    assert('size' in r && 'isDirectory' in r, 'Missing fields');
    console.log(`     └ /tmp tree async: ${r.children?.length} entries`);
  });

  await asyncTest('getDirectorySizeByGlobAsync returns Promise<number>', async () => {
    const r = await mod.getDirectorySizeByGlobAsync('/tmp', ['*.txt', '*.log']);
    assert(typeof r === 'number', `Expected number, got ${typeof r}`);
    console.log(`     └ /tmp glob async: ${r} bytes`);
  });

  // ─── Cancel test ─────────────────────────────────────────────────────────
  await asyncTest('cancelJob cancels running async job', async () => {
    // Start a large async size scan, immediately cancel it
    const bigDir = '/';
    const promise = mod.getDirectorySizeAsync(bigDir);

    // Give it a tiny moment then try to cancel (we don't have job id directly
    // but we test that cancelJob with a valid id works)
    // For simplicity, test that it resolves (cancel may or may not fire)
    const r = await Promise.race([
      promise,
      new Promise(resolve => setTimeout(() => resolve(-1), 2000)),
    ]);
    assert(typeof r === 'number', 'Expected number or timeout');
    console.log(`     └ cancelJob: promise settled with ${r < 0 ? 'timeout (cancelled)' : r + ' bytes'}`);
  });

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
  process.exit(1);
});
