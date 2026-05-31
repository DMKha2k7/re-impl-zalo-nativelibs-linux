#!/usr/bin/env node
/**
 * postbuild.js — Copy built zimage.node to linux_x64/ and copy libvips .so files
 *
 * This script:
 *  1. Copies build/Release/zimage.node → linux_x64/zimage.node
 *  2. Finds libvips .so on the system and copies to linux_x64/
 *     so that zimage.node is self-contained for distribution.
 */

'use strict';

const fs   = require('fs');
const path = require('path');
const { execSync } = require('child_process');

const ROOT   = path.resolve(__dirname, '..');
const BUILT  = path.join(ROOT, 'build', 'Release', 'zimage.node');
const OUTDIR = path.join(ROOT, 'linux_x64');

fs.mkdirSync(OUTDIR, { recursive: true });

// 1. Copy .node
if (!fs.existsSync(BUILT)) {
    console.error('Build output not found:', BUILT);
    process.exit(1);
}
const dstNode = path.join(OUTDIR, 'zimage.node');
fs.copyFileSync(BUILT, dstNode);
console.log(`✓ zimage.node → linux_x64/`);

// 2. Find libvips and dependencies via ldconfig/ldd
let libvipsPath = null;
try {
    // ldd zimage.node | grep vips → extract path
    const lddOut = execSync(`ldd "${BUILT}" 2>/dev/null || true`).toString();
    for (const line of lddOut.split('\n')) {
        if (line.includes('libvips')) {
            const m = line.match(/=>\s+(\S+)/);
            if (m) {
                libvipsPath = m[1];
                break;
            }
        }
    }
} catch (e) { /* ignore */ }

// 3. Also find via pkg-config
let vipsLibDir = null;
try {
    const libFlags = execSync('pkg-config --libs-only-L vips 2>/dev/null').toString().trim();
    if (libFlags) {
        vipsLibDir = libFlags.replace('-L', '').trim();
    }
} catch (e) { /* ignore */ }

// 4. Find all related .so files
const soSearch = [
    '/usr/lib',
    '/usr/lib64',
    '/usr/local/lib',
    vipsLibDir
].filter(Boolean);

const soPatterns = [
    /^libvips/,
];

let copied = 0;
for (const dir of soSearch) {
    if (!fs.existsSync(dir)) continue;
    let files;
    try {
        files = fs.readdirSync(dir);
    } catch { continue; }

    for (const f of files) {
        if (!soPatterns.some(p => p.test(f))) continue;
        if (!f.includes('.so')) continue;

        const src = path.join(dir, f);
        const dst = path.join(OUTDIR, f);

        try {
            const st = fs.lstatSync(src);
            if (st.isSymbolicLink()) {
                const link = fs.readlinkSync(src);
                try { fs.unlinkSync(dst); } catch {}
                fs.symlinkSync(link, dst);
                console.log(`✓ symlink ${f}`);
            } else {
                fs.copyFileSync(src, dst);
                console.log(`✓ ${f}`);
            }
            copied++;
        } catch (e) {
            console.warn(`  skip ${f}: ${e.message}`);
        }
    }
}

if (copied === 0) {
    console.log('⚠  No libvips .so found to copy. zimage.node will use system libvips at runtime.');
    console.log('   If Zalo is run on a machine without libvips, install it:');
    console.log('   sudo dnf install vips  (Fedora/Nobara)');
    console.log('   sudo apt install libvips42  (Ubuntu/Debian)');
}

console.log(`\npostbuild complete → linux_x64/ (${copied} .so files copied)`);
