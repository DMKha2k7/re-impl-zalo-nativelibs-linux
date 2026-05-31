#!/usr/bin/env node
/**
 * preinstall.js — Download prebuilt libvips 42 from sharp project
 * Places headers in vendor/include/vips/ and .so in vendor/lib/
 */

'use strict';

const https = require('https');
const http  = require('http');
const fs    = require('fs');
const path  = require('path');
const { execSync } = require('child_process');
const zlib  = require('zlib');

const ROOT    = path.resolve(__dirname, '..');
const VENDOR  = path.join(ROOT, 'vendor');
const VHEADER = path.join(VENDOR, 'include', 'vips');
const VLIB    = path.join(VENDOR, 'lib');
const OUTDIR  = path.join(ROOT, 'linux_x64');

// sharp uses @img/sharp-libvips-linux-x64
// https://github.com/lovell/sharp-libvips
const LIBVIPS_VERSION = '8.16.0';
const LIBVIPS_URL = `https://github.com/lovell/sharp-libvips/releases/download/v${LIBVIPS_VERSION}/libvips-${LIBVIPS_VERSION}-linux-x64.tar.br`;
const LIBVIPS_URL_GZ = `https://github.com/lovell/sharp-libvips/releases/download/v${LIBVIPS_VERSION}/libvips-${LIBVIPS_VERSION}-linux-x64.tar.gz`;

// Check if already extracted
const MARKER = path.join(VENDOR, '.extracted');

function ensureDirs() {
    for (const d of [VENDOR, VHEADER, VLIB, OUTDIR]) {
        fs.mkdirSync(d, { recursive: true });
    }
}

function download(url, destFile) {
    return new Promise((resolve, reject) => {
        console.log(`Downloading ${url} → ${destFile}`);
        const proto = url.startsWith('https') ? https : http;
        const file  = fs.createWriteStream(destFile);
        const req = proto.get(url, (res) => {
            if (res.statusCode === 301 || res.statusCode === 302) {
                file.close();
                fs.unlinkSync(destFile);
                return download(res.headers.location, destFile).then(resolve).catch(reject);
            }
            if (res.statusCode !== 200) {
                file.close();
                reject(new Error(`HTTP ${res.statusCode} for ${url}`));
                return;
            }
            res.pipe(file);
            file.on('finish', () => { file.close(); resolve(destFile); });
            file.on('error', reject);
        });
        req.on('error', reject);
    });
}

async function main() {
    ensureDirs();

    if (fs.existsSync(MARKER)) {
        console.log('libvips vendor already extracted, skipping download.');
        return;
    }

    const tarGz = path.join(VENDOR, 'libvips.tar.gz');

    // Try npm package first (faster)
    let downloaded = false;
    try {
        // Use @img/sharp-libvips-linux-x64 if available
        const pkgDir = path.join(ROOT, 'node_modules', '@img', 'sharp-libvips-linux-x64');
        if (fs.existsSync(pkgDir)) {
            console.log('Found @img/sharp-libvips-linux-x64, using it...');
            const libDir = path.join(pkgDir, 'lib');
            const incDir = path.join(pkgDir, 'include');
            if (fs.existsSync(libDir)) {
                execSync(`cp -r "${libDir}"/* "${VLIB}/"`, { stdio: 'inherit' });
            }
            if (fs.existsSync(incDir)) {
                execSync(`cp -r "${incDir}"/* "${path.join(VENDOR, 'include')}/"`, { stdio: 'inherit' });
            }
            downloaded = true;
        }
    } catch (e) {
        console.warn('npm package unavailable:', e.message);
    }

    if (!downloaded) {
        // Try GitHub release
        try {
            await download(LIBVIPS_URL_GZ, tarGz);
            console.log('Extracting...');
            execSync(`tar -xzf "${tarGz}" -C "${VENDOR}" --strip-components=1`, { stdio: 'inherit' });
            fs.unlinkSync(tarGz);
            downloaded = true;
        } catch (e) {
            console.error('Failed to download libvips:', e.message);
            process.exit(1);
        }
    }

    // Copy .so files to linux_x64/
    const soFiles = fs.readdirSync(VLIB).filter(f => f.includes('vips') && f.endsWith('.so') || f.match(/libvips.*\.so\./));
    for (const f of soFiles) {
        const src = path.join(VLIB, f);
        const dst = path.join(OUTDIR, f);
        if (!fs.existsSync(dst)) {
            fs.copyFileSync(src, dst);
            console.log(`Copied ${f} → linux_x64/`);
        }
    }

    fs.writeFileSync(MARKER, new Date().toISOString());
    console.log('libvips vendor setup complete.');
}

main().catch(e => { console.error(e); process.exit(1); });
