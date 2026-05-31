# Re-impl Zalo for Linux Native Libraries Patch Build 🛠️

This repository contains the Re-impl source code and build orchestrator to compile and package the native Node.js addons for the Zalo desktop client on Linux systems.

The orchestrator compiles C++ and Rust codebases directly, copies original macOS Javascript wrappers, and applies live search-and-replace patches to inject Linux platform support.

Thanks **realdtn2** for db-cross-v4 nativelibs: [realdtn2/zalo-linux-2026](https://github.com/realdtn2/zalo-linux-2026)

# 📋 Prerequisites & Dependency Packages

To compile the libraries successfully, you need to install standard compilation tools (GCC/G++, Rust/Cargo, Python) along with the respective library development headers for your distribution:

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

> [!WARNING]
> **Static Linking in `mp4thumb`:** By default, `mp4thumb`'s `binding.gyp` points to static ffmpeg `.a` libraries inside `/usr/local/lib/`. If you wish to link dynamically against system libraries instead, adjust the `libraries` array in `mp4thumb/binding.gyp` to use `pkg-config` similarly to `zjxl` and `zimage`.

---

# 🏃 Build

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
cp -r Zalo_macOS/Zalo.app/Contents/Resources/app.asar.unpacked/native/nativelibs nativelibs-macOS
```

### 5. Compile and Package
Installs dependencies for the build script and orchestrates compiling and packing of all 7 submodules:
```bash
npm install
npm run build
```

Once the process completes, the structured distribution-ready native addons will be located in the destination folder.
The default destination folder is `nativelibs-linux-patch`.

---

# ⚙️ How It Works
## Re-impl nativelibs to Linux from macOS x64
### 1. Decompile macOS x64 nativelibs with Ghidra and export decompiled C code (*-decompile.c)
### 2. Re-impl with C++ or Rust

## Build Process
The orchestration script performs the following automated steps upon running `npm run build`:

1. **Compilation**:
   * Runs `node-gyp rebuild` for C++ modules (`file-utils`, `mp4thumb`, `zimage`, `zjxl`).
   * Runs `cargo build --release` for Rust modules (`file-utilities`).
2. **Live JS Wrapper Patching**:
   * `patch-js-helper.js` reads the original macOS JS wrappers from the `nativelibs-macOS` folder.
   * Modifies them in-memory to inject `process.platform === 'linux'` switches and correct relative `.node` paths before saving them to the destination. This eliminates the need to maintain static pre-patched wrapper files.
3. **Shared Library Asset Gathering**:
   * For complex dependencies in `zimage` and `zjxl`, the builder analyzes the compiled binary with `ldd` and executes a queue-based recursive trace. It copies all shared system library dependencies (`.so` files, e.g., OpenCV, libjxl, libvips) directly into the destination subdirectory, ensuring independent binary loading on client systems.
