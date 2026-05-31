#!/usr/bin/env node
/**
 * postbuild.js — Copy built jxl.node to target linux_x64/ and gather dynamic dependencies (.so files)
 */

'use strict';

const fs   = require('fs');
const path = require('path');
const { execSync } = require('child_process');

const ROOT   = path.resolve(__dirname, '..');
const BUILT  = path.join(ROOT, 'build', 'Release', 'jxl.node');
const OUTDIR = path.resolve(ROOT, '../../nativelibs/zjxl/build/linux_x64');

fs.mkdirSync(OUTDIR, { recursive: true });

// 1. Copy .node
if (!fs.existsSync(BUILT)) {
    console.error('Build output not found:', BUILT);
    process.exit(1);
}
const dstNode = path.join(OUTDIR, 'jxl.node');
fs.copyFileSync(BUILT, dstNode);
console.log(`✓ jxl.node → zjxl/build/linux_x64/`);

// 2. Discover and copy dependencies (.so) recursively using ldd
const copiedDeps = new Set();
const queue = [dstNode];

const targets = [
    'libjxl',
    'libopencv_core',
    'libopencv_imgproc',
    'libjpeg',
    'libturbojpeg',
    'libbrotli',
    'libhwy'
];

let copied = 0;

while (queue.length > 0) {
    const currentFile = queue.shift();
    try {
        const lddOut = execSync(`ldd "${currentFile}" 2>/dev/null || true`).toString();
        for (const line of lddOut.split('\n')) {
            const m = line.match(/=>\s+(\S+)/);
            if (m) {
                const srcPath = m[1];
                if (!fs.existsSync(srcPath)) continue;
                
                const libName = path.basename(srcPath);
                
                // Only copy target libraries of interest
                const isTarget = targets.some(t => libName.includes(t));
                if (!isTarget) continue;
                
                const realSrc = fs.realpathSync(srcPath);
                if (copiedDeps.has(realSrc)) continue;
                
                copiedDeps.add(realSrc);
                
                const dstPath = path.join(OUTDIR, libName);
                if (fs.existsSync(dstPath)) {
                    const realDst = fs.realpathSync(dstPath);
                    if (realSrc === realDst) continue;
                }
                
                try {
                    if (fs.existsSync(dstPath)) {
                        fs.unlinkSync(dstPath);
                    }
                    fs.copyFileSync(realSrc, dstPath);
                    console.log(`✓ ${libName} (copied from ${realSrc})`);
                    copied++;
                    
                    queue.push(dstPath);
                } catch (err) {
                    console.warn(`  Failed to copy ${libName}: ${err.message}`);
                }
            }
        }
    } catch (e) {
        console.warn(`  Failed to run ldd on ${currentFile}: ${e.message}`);
    }
}

console.log(`\npostbuild complete → zjxl/build/linux_x64/ (${copied} .so files copied)`);
