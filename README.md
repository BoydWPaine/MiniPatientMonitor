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
| **NIBP** | — | Sys / Mean / Dia |

### Key Features

- TCP + Protobuf communication (`Envelope` with mandatory `Heartbeat` + module `oneof payload`)
- Cross-platform **OS Abstraction Layer** (Windows / Linux / FreeRTOS / ArmLinux)
- Physiological alarming (1 Hz limit check) + technical alarms from Device
- Binary file storage (no database) — simulates EEPROM/FLASH
- Qt6 Host UI at **1024×768** with pixel-defined layout (892:132 waveform:param ratio)
- LVGL Device configuration UI (e.g. change HR default 72 → 148 to trigger alarm demo)
- IEC 62304-style documentation in `docs/`

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
├── common/osal/          # OS abstraction (Windows + Linux first)
├── common/proto/         # monitor.proto (v0.2)
├── common/storage/       # Binary file I/O
├── device/               # Parameter module simulator (TCP server)
├── host/                 # Monitor main application (TCP client)
├── docs/                 # SRS, architecture, risk, test docs
└── .github/workflows/    # CI (planned M5)
```

## Prerequisites (Phase 1 — x86)

| Tool | Version |
|------|---------|
| CMake | ≥ 3.20 |
| MSVC or MinGW / GCC | C++17 |
| Qt6 | ≥ 6.5 (Widgets, Network) |
| Protobuf | ≥ 3.21 |
| LVGL + SDL2 | ≥ 9.0 / 2.28 |
| Google Test | ≥ 1.14 (optional, for tests) |

## Build (planned, M1)

```bash
git clone https://github.com/BoydWPaine/MiniPatientMonitor.git
cd MiniPatientMonitor
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

Run (two terminals — **Device first**, then Host):

```bash
./build/device/mini_device      # starts TCP server on :5000
./build/host/mini_host          # connects and displays vitals
```

> Build scripts will be added in milestone M1. This repo currently contains **planning documentation and proto schema**.

## Documentation

| Document | Description |
|----------|-------------|
| [SRS.md](docs/SRS.md) | Software Requirements Specification |
| [Architecture.md](docs/Architecture.md) | System & module design |
| [RiskAnalysis.md](docs/RiskAnalysis.md) | ISO 14971-style risk table |
| [TestPlan.md](docs/TestPlan.md) | Verification test cases |
| [TestReport.md](docs/TestReport.md) | Test execution report (template) |
| [TraceabilityMatrix.md](docs/TraceabilityMatrix.md) | Requirements ↔ tests |

Chinese planning doc (iteration with AI):  
`/home/walker/Desktop/GrokBuild/Job/Embedded/Project_MiniPatientMonitor/Project_MiniPatientMonitor.md`

## Development Roadmap

### Phase 1 — Application Layer (x86, Windows + Linux)

| Milestone | Goal |
|-----------|------|
| M1 | CMake + OSAL(Win/Linux) + Protobuf + TCP (Device Server) |
| M2 | Five independent module sims + Host 5-waveform UI |
| M3 | Physiological + technical alarms |
| M4 | Patient / data / config persistence |
| M5 | Unit tests + CI |
| M6 | Demo video + doc finalization |

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