# MiniPatientMonitor

A **six-parameter patient monitor demonstration** for portfolio and interview purposes.  
**NOT for clinical use.**

## Overview

Dual-process architecture simulating a real patient monitor system:

| Process | Role | Target Platform |
|---------|------|-----------------|
| **Device** | Parameter-module firmware simulator (waveforms + numerics + tech alarms) | Windows (phase 1) → FreeRTOS + STM32 (phase 2) |
| **Host** | Monitor main application (UI, phys alarms, patient/data/config) | Windows (phase 1) → Arm Linux + Qt6 (phase 2) |

### Six Parameters

- **ECG** (Lead II + Lead V) with **HR**
- **SpO2** with **PR** pleth waveform
- **Respiratory** waveform and rate
- **NIBP** (non-invasive blood pressure)
- **Temperature**

### Key Features

- TCP + Protobuf communication
- Cross-platform **OS Abstraction Layer** (Windows / FreeRTOS / Linux)
- Physiological alarming (1 Hz limit check) + technical alarms from Device
- Binary file storage (no database) — simulates EEPROM/FLASH
- Qt6 Host UI at **1024×768**
- LVGL Device configuration UI (e.g. change HR default 72 → 148 to trigger alarm demo)
- IEC 62304-style documentation in `docs/`

## Architecture

```
┌─────────────┐     TCP/Protobuf      ┌─────────────┐
│   Device    │ ───────────────────►  │    Host     │
│ Param Sim   │                       │ Qt Monitor  │
│ LVGL Config │                       │ Alarms/Data │
└──────┬──────┘                       └──────┬──────┘
       │                                     │
       └──────────► OSAL + Protobuf ◄────────┘
```

See [docs/Architecture.md](docs/Architecture.md) for full design.

## Repository Layout

```
MiniPatientMonitor/
├── common/osal/          # OS abstraction (Windows first)
├── common/proto/         # monitor.proto
├── common/storage/       # Binary file I/O
├── device/               # Parameter module simulator
├── host/                 # Monitor main application
├── docs/                 # SRS, architecture, risk, test docs
└── .github/workflows/    # CI (planned M5)
```

## Prerequisites (Windows — Phase 1)

| Tool | Version |
|------|---------|
| CMake | ≥ 3.20 |
| MSVC or MinGW | C++17 |
| Qt6 | ≥ 6.5 (Widgets, Network) |
| Protobuf | ≥ 3.21 |
| LVGL + SDL2 | ≥ 9.0 / 2.28 |
| Google Test | ≥ 1.14 (optional, for tests) |

## Build (Windows — planned, M1)

```bash
git clone https://github.com/BoydWPaine/MiniPatientMonitor.git
cd MiniPatientMonitor
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

Run (two terminals):

```bash
./build/host/mini_host          # starts TCP server on :5000
./build/device/mini_device      # connects and streams vitals
```

> Build scripts will be added in milestone M1. This repo currently contains **planning documentation only**.

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

| Milestone | Goal |
|-----------|------|
| M1 | CMake + OSAL(Win) + Protobuf + TCP connect |
| M2 | Six-parameter waveforms on Host UI |
| M3 | Physiological + technical alarms |
| M4 | Patient / data / config persistence |
| M5 | Unit tests + CI |
| M6 | Demo video + doc finalization |
| M7 | FreeRTOS + Arm Linux port |

**MVP target:** 2026-06-30 (M1–M4 demonstrable)

## Disclaimer

This project is a **software engineering portfolio demonstration**. It does not provide clinically validated measurements and must not be used for patient monitoring, diagnosis, or treatment.

## License

See [LICENSE](LICENSE).

## Author

Jianjun (Boyd) Peng — former Mindray embedded software engineer, 17 years in medical devices.