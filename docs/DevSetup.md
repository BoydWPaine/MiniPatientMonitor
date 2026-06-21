# Developer Environment Setup

**Project:** MiniPatientMonitor  
**Version:** 1.1  
**Date:** 2026-06-21

This guide explains how to install prerequisites, build, and run the project on **Linux** and **Windows**.  
For architecture and requirements, see [Architecture.md](Architecture.md) and [SRS.md](SRS.md).

---

## 1. Tool Overview by Milestone

| Tool | Version | M1 (current) | M2+ | M5+ | Notes |
|------|---------|:------------:|:---:|:---:|-------|
| CMake | ≥ 3.20 | **Required** | | | |
| C++ compiler (GCC / MSVC / MinGW) | C++17 | **Required** | | | |
| Git | any recent | **Required** | | | FetchContent clones protobuf |
| zlib dev headers | — | **Required** | | | Linux: `zlib1g-dev` |
| Protobuf (`protoc` + lib) | ≥ 3.21 | **Bundled** | | | CMake FetchContent builds v21.12; system `protoc` optional |
| Qt6 (Widgets, Network) | ≥ 6.5 | — | **Required** | | Host UI (M2) |
| LVGL + SDL2 | ≥ 9.0 / 2.28 | — | **Required** | | Device config UI — all parameters (M2) |
| Google Test | ≥ 1.14 | — | — | **Required** | Unit tests + CI (M5) |

**M1 builds today without Qt6, LVGL, or GTest.** The repo produces console executables `mini_device` and `mini_host` with TCP/Protobuf heartbeat demo.

---

## 2. Quick Start (Linux — M1)

On **Debian / Ubuntu / Linux Mint / Pop!_OS**:

```bash
git clone https://github.com/BoydWPaine/MiniPatientMonitor.git
cd MiniPatientMonitor

# Install M1 build tools (cmake, g++, git, zlib, protobuf dev packages)
./tools_install.sh

# Configure and build
./build.sh debug

# Run (two terminals — Device first, then Host)
./build/device/mini_device
./build/host/mini_host
```

Expected output:
- Device: `listening on 127.0.0.1:5000`, sends heartbeat frames every 1 s
- Host: connects, receives 3 `NullPacket` heartbeat frames, prints `connection OK`

---

## 3. Linux — Detailed Setup

### 3.1 M1 — Required (automated)

`tools_install.sh` installs:

| Package | Purpose |
|---------|---------|
| `build-essential` | `g++`, `make` |
| `cmake` | Build system (≥ 3.20) |
| `git` | FetchContent for protobuf |
| `pkg-config` | Future dependency detection |
| `zlib1g-dev` | Protobuf / compression |
| `protobuf-compiler`, `libprotobuf-dev` | Optional system protoc; verify step only |

Run:

```bash
./tools_install.sh          # prompts sudo if needed
./tools_install.sh help     # show usage
```

Manual equivalent (Ubuntu 22.04+):

```bash
sudo apt-get update
sudo apt-get install -y \
    build-essential cmake git pkg-config zlib1g-dev \
    protobuf-compiler libprotobuf-dev
```

### 3.2 M2 — Qt6 (Host UI)

Install when starting **M2** (not needed for current M1 build):

```bash
sudo apt-get install -y \
    qt6-base-dev \
    qt6-base-dev-tools \
    libqt6widgets6 \
    libqt6gui6 \
    libqt6core6t64
```

Package names may vary slightly on older Ubuntu (e.g. `libqt6core6` without `t64`). Verify:

```bash
pkg-config --modversion Qt6Widgets
# or
qmake6 --version
```

CMake (M2) will use:

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_PREFIX_PATH=/usr/lib/x86_64-linux-gnu/cmake/Qt6
```

Adjust `CMAKE_PREFIX_PATH` if Qt is installed via the [Qt Online Installer](https://www.qt.io/download-open-source) (common path: `$HOME/Qt/6.5.x/gcc_64`).

### 3.3 M2 — LVGL + SDL2 (Device config UI)

For Device configuration panel (all numeric parameters adjustable):

```bash
sudo apt-get install -y \
    libsdl2-dev \
    libsdl2-image-dev
```

LVGL itself is **not** in apt on most distros. Recommended approach for M2:

- Add LVGL via **CMake FetchContent** (pin tag e.g. `v9.2.0`), or
- Git submodule under `device/third_party/lvgl`

SDL2 provides the desktop display/input backend on x86.

### 3.4 M5 — Google Test

```bash
sudo apt-get install -y libgtest-dev

# Ubuntu may need an extra build step for gtest static libs:
cd /usr/src/gtest
sudo cmake .
sudo make
sudo cp lib/*.a /usr/lib
```

Alternatively, use **CMake FetchContent** for googletest (recommended for CI reproducibility) — no system install required.

### 3.5 Build & run reference

| Command | Action |
|---------|--------|
| `./build.sh` / `./build.sh debug` | Debug build |
| `./build.sh release` | Release build |
| `./build.sh clean` | Remove `build/` |
| `JOBS=8 ./build.sh` | Parallel build |

CMake options (root `CMakeLists.txt`):

| Option | Default | Description |
|--------|---------|-------------|
| `MPM_BUILD_DEVICE` | ON | Build `mini_device` |
| `MPM_BUILD_HOST` | ON | Build `mini_host` |
| `MPM_TCP_PORT` | 5000 | TCP port |
| `MPM_TCP_HOST` | 127.0.0.1 | Bind/connect address |

---

## 4. Windows — Detailed Setup

### 4.1 M1 — Required

1. **Visual Studio 2022** (Community is fine)  
   - Workload: *Desktop development with C++*  
   - Includes MSVC, CMake support, Windows SDK

2. **Git for Windows**  
   - https://git-scm.com/download/win  
   - Required for CMake FetchContent (protobuf)

3. **CMake** (≥ 3.20)  
   - Bundled with VS 2022, or standalone: https://cmake.org/download/

4. **Open a developer shell**  
   - *x64 Native Tools Command Prompt for VS 2022*, or  
   - PowerShell with VS environment

Build:

```bat
git clone https://github.com/BoydWPaine/MiniPatientMonitor.git
cd MiniPatientMonitor

cmake -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Debug

build\device\Debug\mini_device.exe
build\host\Debug\mini_host.exe
```

Or with Ninja (faster):

```bat
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

**Notes:**
- Protobuf is built via FetchContent; no separate install required.
- `ws2_32` is linked automatically for Winsock2 (OSAL TCP).
- M1 has not been CI-verified on Windows; report issues if Winsock or MSVC flags need tweaks.

### 4.2 M2 — Qt6 (Host UI)

**Option A — Qt Online Installer (recommended for Qt Widgets)**

1. Install Qt 6.5+ with *Qt Widgets* and *Qt Network* components.
2. Note install path, e.g. `C:\Qt\6.7.0\msvc2019_64`.
3. Configure:

```bat
cmake -B build -G Ninja ^
    -DCMAKE_PREFIX_PATH=C:\Qt\6.7.0\msvc2019_64 ^
    -DCMAKE_BUILD_TYPE=Debug
```

**Option B — vcpkg**

```bat
vcpkg install qtbase:x64-windows
cmake -B build -DCMAKE_TOOLCHAIN_FILE=%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake
```

### 4.3 M2 — LVGL + SDL2 (Device config UI)

**vcpkg (simplest on Windows):**

```bat
vcpkg install sdl2:x64-windows
```

Add LVGL via FetchContent in CMake (same as Linux).

### 4.4 M5 — Google Test

```bat
vcpkg install gtest:x64-windows
```

Or FetchContent in `common/CMakeLists.txt` when enabling tests.

---

## 5. What `tools_install.sh` Does / Does Not Do

| Installs | Does not install (see §3.2–3.4) |
|----------|----------------------------------|
| gcc/g++, make, cmake, git | Qt6 |
| zlib, protobuf dev packages | LVGL |
| | SDL2 |
| | Google Test |

`tools_install.sh` is intentionally **M1-scoped**. For M2/M5 dependencies, use the manual steps in this document until the script gains optional flags (e.g. `./tools_install.sh --m2`).

---

## 6. Verify Installation

### M1 checklist

```bash
cmake --version          # >= 3.20
g++ --version            # supports C++17
git --version
./build.sh debug
./build/device/mini_device   # terminal 1
./build/host/mini_host       # terminal 2
```

### M2 checklist (when Qt is wired)

```bash
pkg-config --exists Qt6Widgets && echo "Qt6 OK"
./build.sh debug
# Host should open 1024x768 window (not console)
```

---

## 7. Troubleshooting

| Problem | Likely cause | Fix |
|---------|--------------|-----|
| `cmake: not found` | Tools not installed | Run `./tools_install.sh` (Linux) or install CMake (Windows) |
| FetchContent git clone fails | No git / network | Install `git`; check proxy/firewall |
| `protoc: not found` during build | — | Should not happen: build uses `protobuf::protoc` from FetchContent. Run `cmake -B build` from clean `build/` |
| Host `connect failed` | Device not running | Start `mini_device` first; check port 5000 not in use |
| `Address already in use` | Stale `mini_device` | `killall mini_device` or change `MPM_TCP_PORT` |
| Qt6 not found (M2) | CMAKE_PREFIX_PATH | Set path to Qt6 `lib/cmake/Qt6` parent |
| LVGL build errors (M2) | Missing SDL2 | Install `libsdl2-dev` / vcpkg `sdl2` |

---

## 8. Relation to Other Docs

| Document | Role |
|----------|------|
| [README.md](../README.md) | Project overview + quick build/run |
| [Architecture.md](Architecture.md) | Module design, OSAL, protocol |
| [TestPlan.md](TestPlan.md) | Verification after setup |
| `../build.sh` | One-click configure + build |
| `../tools_install.sh` | One-click **M1** Linux deps |

---

## 9. Change History

| Version | Date | Change |
|---------|------|--------|
| 1.0 | 2026-06-21 | Initial DevSetup: M1 quick start, Linux/Windows, M2/M5 optional deps |
| 1.1 | 2026-06-21 | LVGL required for M2 (all parameters configurable) |