# Re-impl Zalo for Linux Native Libraries 🛠️

This repository contains the Re-impl source code from C decompile code with Ghidar and build orchestrator to compile and package the native Node.js addons for the Zalo for Linux client.

# 🔴 Status
- ✅ db-cross-v4: working, thanks for realdtn2's RE db-cross-v4 ([realdtn2/zalo-linux-2026](https://github.com/realdtn2/zalo-linux-2026))
- ✅ file-utilities: working, Experimental (need to refresh in Storage Management Settings after patch)
- ✅ file-utils: working, Experimental
- ✅ logger: working (native support)
- ❌ mp4thumb: Planned, In Progress
- ✅ sqlite3: working (add native Linux libs to Zalo's nativelibs)
- ❌ v8-profiles: Not planned
- ❌ zcall: Not planned (requires ZallCall from the Qt ZaloHelper Process)
- ❌ zfile: Not needed (Only Windows needs this nativelib)
- ❌ zimage: Planned, In Progress
- ❌ zjxl: Planned, In Progress (jxl nativelibs shipped with Linux `.so` libs)
- ❌ zwalker: Planned for later

# 📋 Prerequisites & Dependency Packages

## 🔴 Debian / Ubuntu (apt)
```bash
# Install core build tools, nodejs & rust
sudo apt update
sudo apt install -y build-essential python3 pkg-config rustc cargo nodejs npm

# Install library development headers
sudo apt install -y \
  libssl-dev \
  liblzma-dev \
  libbz2-dev \
  zlib1g-dev \
  libvips-dev \
  libjxl-dev \
  libopencv-dev \
  libjpeg-dev \
  libavcodec-dev \
  libavformat-dev \
  libswscale-dev \
  libavutil-dev
```

## 🔵 Fedora (dnf)
```bash
# Install core build tools, nodejs & rust
sudo dnf groupinstall -y "Development Tools"
sudo dnf install -y python3 pkgconf-pkg-config rust cargo nodejs npm

# Install library development headers
sudo dnf install -y \
  openssl-devel \
  xz-devel \
  bzip2-devel \
  zlib-devel \
  vips-devel \
  libjxl-devel \
  opencv-devel \
  libjpeg-turbo-devel \
  ffmpeg-free-devel
```

## 🟢 Arch Linux (pacman)
```bash
# Install core build tools, nodejs & rust
sudo pacman -Syu --needed base-devel python rust nodejs npm

# Install library dependencies
sudo pacman -S --needed \
  openssl \
  xz \
  bzip2 \
  zlib \
  libvips \
  libjxl \
  opencv \
  libjpeg-turbo \
  ffmpeg
```
---

# 🏃 Build
## Do not perform steps 2, 3, and 4 if skipping the js patch (Add Linux support to the original loader js)
### 1. Clean Up Build Artifacts
Deletes all `build/`, `target/`, and `node_modules/` folders, as well as temporary `.node` and generated test files in all subdirectories:
```bash
npm run clean
```
### 2. Download Zalo for macOS .dmg file and move to this directory repository

### 3. Extract the Zalo .dmg file
```bash
7z x Zalo_*.dmg -o temp/Zalo_macOS
```

### 4. Copy original macOS nativelibs
```bash
cp -r temp/Zalo_macOS/Zalo.app/Contents/Resources/app.asar.unpacked/native/nativelibs nativelibs-macOS
```

### 5. Compile and Package
Installs dependencies for the build script and orchestrates compiling and packing of all submodules:
```bash
npm install
npm run build
```

**With skipping js patch, run `npm run build-skip-patch` instead of `npm run build`.**

Once the process completes, the structured distribution-ready native addons will be located in the destination folder.
The default destination folder is `nativelibs`.

---

# ⚙️ How It Works
## Re-impl nativelibs to Linux from macOS x64
### 1. Decompile macOS x64 nativelibs with Ghidra and export decompiled C code (decompile/*-decompile.c)
### 2. Re-impl with C++ or Rust (by the original language)

## Build Process
The orchestration script performs the following automated steps upon running `npm run build`:

1. **Compilation**:
   * Runs `node-gyp rebuild` for C++ modules (`mp4thumb`, `zimage`, `zjxl`).
   * Runs `cargo build --release` for Rust modules (`file-utilities`, `file-utils`).
2. **Live JS Wrapper Patching**:
   * `patch-js-helper.js` reads the original macOS JS wrappers from the `nativelibs-macOS` folder.
   * Modifies them in-memory to inject `process.platform === 'linux'` switches and correct relative `.node` paths before saving them to the destination. This eliminates the need to maintain static pre-patched wrapper files.
3. **Shared Library Asset Gathering**:
   * For complex dependencies in `zimage` and `zjxl`, the builder analyzes the compiled binary with `ldd` and executes a queue-based recursive trace. It copies all shared system library dependencies (`.so` files, e.g., OpenCV, libjxl, libvips) directly into the destination subdirectory, ensuring independent binary loading on client systems.
