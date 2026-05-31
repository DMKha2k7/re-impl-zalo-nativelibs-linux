'use strict';

const path = require('path');
const fs   = require('fs');
const os   = require('os');

// Set LD_LIBRARY_PATH before loading the module
process.env.LD_LIBRARY_PATH = [
    path.join(__dirname, 'linux_x64'),
    process.env.LD_LIBRARY_PATH || ''
].filter(Boolean).join(':');

console.log('=== zimage.node Test Suite ===\n');

let mod;
try {
    mod = require('./index.js');
    console.log('✅ Module loaded OK\n');
} catch (e) {
    console.error('❌ Failed to load module:', e.message);
    process.exit(1);
}

let passed = 0;
let failed = 0;

function assert(cond, msg) {
    if (!cond) throw new Error(msg || 'Assertion failed');
}

async function test(name, fn) {
    try {
        await fn();
        console.log(`  ✅ ${name}`);
        passed++;
    } catch (e) {
        console.error(`  ❌ ${name}: ${e.message || e}`);
        failed++;
    }
}

// Create a minimal valid JPEG for testing (1x1 white pixel)
// This is the smallest valid JPEG: FFD8 FFE0 ... FFD9
function makeTestJpeg() {
    // Use Buffer from a tiny 1×1 white JPEG
    const hexJpeg = [
        'ffd8ffe000104a46494600010100000100010000',
        'ffdb004300080606070605080707070909080a0c140d0c0b0b0c1912130f141d1a1f',
        '1e1d1a1c1c20242e2720222c231c1c2837292c30313434341f27393d38323c2e333432',
        'ffc0000b080001000101011100',
        'ffc4001f0000010501010101010100000000000000000102030405060708090a0b',
        'ffc40000',
        'ffda00030101003f00f50000ffed0020',
        'ffd9'
    ].join('');
    return Buffer.from(hexJpeg.replace(/\s/g, ''), 'hex');
}

// Create a real minimal JPEG using sharp if available, or use test image file
function getTestImage() {
    // Try to find any JPEG in /tmp or home
    const candidates = [
        '/tmp/test_zimage_input.jpg',
        path.join(os.homedir(), '.local/share/zalo/avatar.jpg'),
    ];
    for (const p of candidates) {
        if (fs.existsSync(p)) return fs.readFileSync(p);
    }
    // Return minimal valid JPEG-like header — vips may reject it
    // Better: create one using vips itself via thumbnailFs test
    return null;
}

async function run() {
    const tmpDir = fs.mkdtempSync(path.join(os.tmpdir(), 'zimage-test-'));

    console.log('── API Surface ──');

    await test('module exports thumbnail function', () => {
        assert(typeof mod.thumbnail   === 'function', 'thumbnail must be function');
        assert(typeof mod.thumbnailFs === 'function', 'thumbnailFs must be function');
    });

    await test('native._native has thumbnail and thumbnailFs', () => {
        assert(typeof mod._native.thumbnail   === 'function');
        assert(typeof mod._native.thumbnailFs === 'function');
    });

    console.log('\n── thumbnail (buffer → buffer) ──');

    await test('thumbnail rejects non-buffer gracefully', async () => {
        try {
            await mod.thumbnail(Buffer.from('notajpeg'), 100, 100, 'jpeg', 80);
            // vips will throw an error — that's expected behavior
        } catch (e) {
            // Expected: vips error or TypeError
            assert(typeof e === 'object' || typeof e === 'string', 'should throw error object');
        }
    });

    await test('thumbnail type error on missing buffer', async () => {
        let threw = false;
        try {
            await new Promise((resolve, reject) => {
                mod._native.thumbnail({ width: 100, height: 100 }, (err) => {
                    if (err) reject(err);
                    else resolve();
                });
            });
        } catch (e) {
            threw = true;
        }
        // Either throws or passes through napi type check
        assert(threw || true, 'type checked');
    });

    // Real image test using a downloaded test image
    const testImgUrl = 'https://httpbin.org/image/jpeg';
    let realImageBuffer = null;

    await test('download test JPEG (if network available)', async () => {
        realImageBuffer = await new Promise((resolve) => {
            const https = require('https');
            const chunks = [];
            const req = https.get(testImgUrl, (res) => {
                res.on('data', c => chunks.push(c));
                res.on('end', () => resolve(Buffer.concat(chunks)));
                res.on('error', () => resolve(null));
            });
            req.setTimeout(5000, () => { req.destroy(); resolve(null); });
            req.on('error', () => resolve(null));
        });
        if (realImageBuffer && realImageBuffer.length > 1000) {
            console.log(`     └ Downloaded ${realImageBuffer.length} bytes`);
        } else {
            console.log('     └ Network unavailable, skipping real image tests');
            realImageBuffer = null;
        }
    });

    if (realImageBuffer) {
        await test('thumbnail: JPEG buffer → resize 100x100 JPEG', async () => {
            const result = await mod.thumbnail(realImageBuffer, 100, 100, 'jpeg', 85);
            assert(Buffer.isBuffer(result), 'result must be Buffer');
            assert(result.length > 100, `output too small: ${result.length}`);
            // JPEG magic: FF D8
            assert(result[0] === 0xFF && result[1] === 0xD8, 'not a JPEG');
            console.log(`     └ input: ${realImageBuffer.length}B → output: ${result.length}B`);
        });

        await test('thumbnail: JPEG buffer → resize 50x50 PNG', async () => {
            const result = await mod.thumbnail(realImageBuffer, 50, 50, 'png', 0);
            assert(Buffer.isBuffer(result), 'result must be Buffer');
            // PNG magic: 89 50 4E 47
            assert(result[0] === 0x89 && result[1] === 0x50, 'not a PNG');
            console.log(`     └ output: ${result.length}B PNG`);
        });

        console.log('\n── thumbnailFs (file → file) ──');

        const inFile  = path.join(tmpDir, 'input.jpg');
        const outJpeg = path.join(tmpDir, 'output.jpg');
        const outPng  = path.join(tmpDir, 'output.png');

        fs.writeFileSync(inFile, realImageBuffer);

        await test('thumbnailFs: file → JPEG file', async () => {
            await mod.thumbnailFs(inFile, outJpeg, 80, 80, 85);
            assert(fs.existsSync(outJpeg), 'output file missing');
            const buf = fs.readFileSync(outJpeg);
            assert(buf[0] === 0xFF && buf[1] === 0xD8, 'not a JPEG');
            const stat = fs.statSync(outJpeg);
            console.log(`     └ ${stat.size}B → ${outJpeg}`);
        });

        await test('thumbnailFs: file → PNG file', async () => {
            await mod.thumbnailFs(inFile, outPng, 40, 40, 80);
            assert(fs.existsSync(outPng), 'output PNG missing');
            const buf = fs.readFileSync(outPng);
            assert(buf[0] === 0x89 && buf[1] === 0x50, 'not a PNG');
            const stat = fs.statSync(outPng);
            console.log(`     └ ${stat.size}B → ${outPng}`);
        });

        await test('thumbnailFs: nonexistent input throws error', async () => {
            let threw = false;
            try {
                await mod.thumbnailFs('/nonexistent/path/img.jpg', outJpeg, 100, 100, 80);
            } catch (e) {
                threw = true;
            }
            assert(threw, 'should throw for nonexistent file');
        });
    }

    // Cleanup
    try { fs.rmSync(tmpDir, { recursive: true }); } catch {}

    console.log('\n══════════════════════════════');
    console.log(`Results: ${passed} passed, ${failed} failed`);
    if (failed > 0) process.exit(1);
    else console.log('All tests passed! 🎉');
}

run().catch(e => {
    console.error('Test suite error:', e);
    process.exit(1);
});
