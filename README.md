# MiniPatientMonitor

A **six-parameter patient monitor demonstration** for portfolio and interview purposes.  
**NOT for clinical use.**

## Overview

Dual-process architecture simulating a real patient monitor system:

| Process | Role | Phase 1 | Phase 2 |
|---------|------|---------|---------|
| **Device** | Parameter-module simulator (TCP **Server**) | x86 Windows + Linux | STM32 + FreeRTOS |
| **Host** | Monitor main application (TCP **Client**) | x86 Windows + Linux | AM3358 + ArmLinux + Qt6 |

### Six Parameters (Five Independent Modules)

| Module | Waveforms | Numerics |
|--------|-----------|----------|
| **ECG** | 12 leads (I, II, III, aVR, aVL, aVF, V1–V6); UI default II + V1 | HR |
| **SpO2** | Pleth waveform | SpO2, PR |
| **Resp** | Respiratory waveform | Resp rate |
| **Temp** | Temperature waveform | Temp (0.1°C integer, displayed as °C) |
| **NIBP** | — | Sys / Mean / Dia (Host `NibpRequest`; manual + STAT on Host) |

### Key Features

- TCP + Protobuf communication (`Envelope` with mandatory `Heartbeat` + module `oneof payload`)
- Cross-platform **OS Abstraction Layer** (Windows / Linux / FreeRTOS / ArmLinux)
- Physiological alarming (1 Hz limit check) + technical alarms from Device *(M3)*
- Binary file storage (no database) — simulates EEPROM/FLASH *(M4)*
- Qt6 Host UI at **1024×768** *(M2)*
- LVGL Device configuration UI — all numeric params adjustable *(M2)*
- Bidirectional TCP: Host sends `NibpRequest` for NIBP measure *(M2)*
- IEC 62304-style documentation in `docs/`

## Current Status

| Milestone | Status |
|-----------|--------|
| **M1** — CMake + OSAL + Protobuf + TCP (Device Server) | **Done** |
| M2 — Five module sims + Host 5-waveform UI | Planned |
| M3 — Alarms + TopBar | Planned |

M1 delivers console `mini_device` / `mini_host` with heartbeat (`NullPacket`) over TCP. See [docs/DevSetup.md](docs/DevSetup.md) for full setup.

## Architecture

```
┌─────────────┐     TCP/Protobuf      ┌─────────────┐
│   Device    │ ◄──────────────────  │    Host     │
│ TCP Server  │   (Host connects)    │ TCP Client  │
│ 5 Module    │                       │ Qt Monitor  │
│ Simulators  │                       │ Alarms/Data │
│ LVGL Config │                       │             │
└──────┬──────┘                       └──────┬──────┘
       │                                     │
       └──────────► OSAL + Protobuf ◄────────┘
```

See [docs/Architecture.md](docs/Architecture.md) for full design.

## Repository Layout

```
MiniPatientMonitor/
├── build.sh              # One-click configure + build
├── tools_install.sh      # Linux M1 dependency installer
├── common/osal/          # OS abstraction (Windows + Linux)
├── common/proto/         # monitor.proto + frame encoder
├── device/               # TCP server (parameter simulator)
├── host/                 # TCP client (monitor app)
├── docs/                 # SRS, architecture, DevSetup, tests
└── .github/workflows/    # CI (planned M5)
```

## Quick Start

**Full prerequisites and platform-specific steps:** [docs/DevSetup.md](docs/DevSetup.md)

### Linux (M1)

```bash
git clone https://github.com/BoydWPaine/MiniPatientMonitor.git
cd MiniPatientMonitor

./tools_install.sh    # cmake, g++, git, zlib, protobuf dev
./build.sh debug      # builds mini_device + mini_host
```

### Run (two terminals — Device first)

```bash
./build/device/mini_device    # TCP server on 127.0.0.1:5000
./build/host/mini_host        # connects, receives heartbeat frames
```

### Windows (M1)

Visual Studio 2022 + Git + CMake. See [DevSetup.md §4](docs/DevSetup.md#4-windows--detailed-setup).

```bat
cmake -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Debug
```

## Prerequisites Summary

| Tool | M1 | M2+ | Install guide |
|------|:--:|:---:|---------------|
| CMake ≥ 3.20 | ✓ | | [DevSetup.md](docs/DevSetup.md) |
| C++17 compiler | ✓ | | |
| Git | ✓ | | |
| Protobuf ≥ 3.21 | ✓* | | *Built via CMake FetchContent |
| Qt6 ≥ 6.5 | | ✓ | DevSetup §3.2 / §4.2 |
| LVGL + SDL2 | | ✓ | DevSetup §3.3 / §4.3 |
| Google Test ≥ 1.14 | | | M5 — DevSetup §3.4 |

`tools_install.sh` covers **M1 only**. Qt6, LVGL, and GTest require manual steps documented in DevSetup.

## Documentation

| Document | Description |
|----------|-------------|
| [DevSetup.md](docs/DevSetup.md) | **Developer environment setup** (Linux / Windows) |
| [SRS.md](docs/SRS.md) | Software Requirements Specification |
| [Architecture.md](docs/Architecture.md) | System & module design |
| [RiskAnalysis.md](docs/RiskAnalysis.md) | ISO 14971-style risk table |
| [TestPlan.md](docs/TestPlan.md) | Verification test cases |
| [TestReport.md](docs/TestReport.md) | Test execution report (template) |
| [TraceabilityMatrix.md](docs/TraceabilityMatrix.md) | Requirements ↔ tests |

Chinese planning docs:  
`/home/walker/Desktop/GrokBuild/Job/Embedded/Project_MiniPatientMonitor/`

## Development Roadmap

### Phase 1 — Application Layer (x86, Windows + Linux)

| Milestone | Goal | Status |
|-----------|------|--------|
| M1 | CMake + OSAL(Win/Linux) + Protobuf + TCP (Device Server) | **Done** |
| M2 | Five independent module sims + Host 5-waveform UI | |
| M3 | Physiological + technical alarms | |
| M4 | Patient / data / config persistence | |
| M5 | Unit tests + CI | |
| M6 | Demo video + doc finalization | |

### Phase 2 — Hardware Integration

| Milestone | Goal |
|-----------|------|
| M7 | FreeRTOS + STM32F407 (Device) |
| M8 | ArmLinux + AM3358 + Qt6 (Host) |
| M9 | Board-level integration + watchdog logging |

**MVP target:** 2026-06-30 (M1–M4 demonstrable)

## Disclaimer

This project is a **software engineering portfolio demonstration**. It does not provide clinically validated measurements and must not be used for patient monitoring, diagnosis, or treatment.

## License

See [LICENSE](LICENSE).

## Author

Jianjun (Boyd) Peng — former Mindray embedded software engineer, 17 years in medical devices.