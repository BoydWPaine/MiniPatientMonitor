# Test Plan

**Project:** MiniPatientMonitor  
**Version:** 0.2  
**Date:** 2026-06-21

---

## 1. Purpose

Define verification activities for MiniPatientMonitor demonstration software per requirements in [SRS.md](SRS.md).

## 2. Test Levels

| Level | Scope | Tool |
|-------|-------|------|
| Unit (UT) | OSAL, codec, alarm logic, storage | Google Test |
| Integration (IT) | Device-Host TCP, alarm E2E | Manual + GTest harness |
| System (ST) | Full UI workflow | Manual checklist |

## 3. Unit Tests

### UT-OSAL (Windows / Linux port)

| ID | Description | Requirement |
|----|-------------|-------------|
| TC-UT-01 | `osal_get_tick_ms` monotonic increase | FR-C01 |
| TC-UT-02 | Mutex lock/unlock mutual exclusion | FR-C01 |
| TC-UT-03 | Queue send/recv bounded buffer | FR-C01 |
| TC-UT-04 | TCP localhost echo round-trip (server/client roles) | FR-D01 |

### UT-Protocol

| ID | Description | Requirement |
|----|-------------|-------------|
| TC-UT-10 | Encode/decode `EcgPacket` in `Envelope` round-trip | FR-C02 |
| TC-UT-11 | Encode/decode `Spo2Packet`, `RespPacket`, `TempPacket`, `NibpPacket` round-trip | FR-C02 |
| TC-UT-12 | Reject oversize length prefix | FR-C02 |
| TC-UT-13 | `TechAlarmEvent` repeated codes serialize; no message field | FR-D04 |
| TC-UT-14 | `Envelope` with `NullPacket` payload (heartbeat-only) round-trip | FR-D05 |
| TC-UT-15 | `TempPacket.temperature` 0.1°C integer encoding (365 → 36.5°C display) | FR-D02 |

### UT-PhysAlarm

| ID | Description | Requirement |
|----|-------------|-------------|
| TC-UT-20 | HR above upper → HighAlarm state | FR-H06 |
| TC-UT-21 | HR below lower → LowAlarm state | FR-H06 |
| TC-UT-22 | Return in range → Normal | FR-H06 |
| TC-UT-23 | Evaluation runs at 1 Hz only | NFR-02 |

### UT-Storage

| ID | Description | Requirement |
|----|-------------|-------------|
| TC-UT-30 | Write/read UserConfig bin round-trip | FR-H09 |
| TC-UT-31 | Append AlarmRecord to alarms.bin | FR-H08 |
| TC-UT-32 | Patient index create/list | FR-H10 |

### UT-ParamSim

| ID | Description | Requirement |
|----|-------------|-------------|
| TC-UT-40 | ECG 12-lead buffer length matches sample rate | FR-D02 |
| TC-UT-41 | HR config change updates `EcgPacket.hr` | FR-D03 |
| TC-UT-42 | Independent module sims emit separate packet types | FR-D02 |

---

## 4. Integration Tests

| ID | Scenario | Steps | Expected | Requirement |
|----|----------|-------|----------|-------------|
| TC-IT-01 | TCP connect | Start Device (server), start Host (client) | Connected ≤5 s | FR-D01 |
| TC-IT-02 | Waveform stream | Run 10 s | Host receives ≥250 Ecg/Spo2/Resp/Temp packets | NFR-01 |
| TC-IT-03 | Numeric stream | Run 5 s | HR/SpO2/NIBP/Temp displayed | FR-H04 |
| TC-IT-04 | HR alarm demo | Set HR=148, limit=120 | Phys alarm text visible | FR-H06 |
| TC-IT-05 | Tech alarm | Inject LEAD_OFF on Device | Tech alarm area shows localized text | FR-H07 |
| TC-IT-06 | Disconnect/reconnect | Kill Device, restart | Host reconnects ≤10 s | FR-D05 |
| TC-IT-07 | Config persist | Change alarm limit, restart | Limit retained | FR-H09 |
| TC-IT-08 | Admit patient | Admit dialog → save | Patient info in top bar | FR-H10 |
| TC-IT-09 | Module independence | Disable SpO2 sim only | Other modules still stream | FR-D02 |

---

## 5. System Tests (Manual)

| ID | Scenario | Expected | Requirement |
|----|----------|----------|-------------|
| TC-SYS-01 | Launch 1024×768 window | Layout matches pixel coordinates (892:132) | FR-H01 |
| TC-SYS-02 | Top bar sections visible | All 5 regions at correct widths | FR-H02 |
| TC-SYS-03 | Five waveforms scrolling | ECG II/V1, PR, Resp, Temp animate | FR-H03 |
| TC-SYS-04 | Param order HR/NIBP/SpO2+PR/Resp/Temp | Visual check | FR-H04 |
| TC-SYS-05 | Bottom 8 slots (6 function + page buttons) | Each dialog functional | FR-H05 |
| TC-SYS-06 | DEMO ONLY watermark | Visible on main screen | R-01 |
| TC-SYS-07 | Device LVGL change HR | Host updates within 2 s | FR-D03 |
| TC-SYS-08 | Data review shows trend | After 1 min run, data present | FR-H08 |
| TC-SYS-09 | Export patient data | File created on disk | FR-H10 |
| TC-SYS-10 | Factory reset | User config cleared | FR-H09 |
| TC-SYS-11 | Temp display | Shows 36.5°C from integer 365 | FR-H04 |

---

## 6. Test Environment

| Item | Phase 1 |
|------|---------|
| OS | Windows 10/11 x64; Ubuntu 22.04 x64 |
| Host display | 1024×768 minimum |
| Network | localhost TCP; Device=Server, Host=Client |
| Test data | Synthetic only |

---

## 7. Entry / Exit Criteria

### Entry (M2+)

- CMake build produces `mini_device` and `mini_host`
- SRS requirements IDs frozen for milestone

### Exit (MVP 2026-06-30)

- All P0 integration tests pass
- ≥60% line coverage on alarm + protocol modules
- All TC-SYS-01..06 pass
- No open Critical defects

---

## 8. Traceability

See [TraceabilityMatrix.md](TraceabilityMatrix.md).

---

## 9. Change History

| Version | Date | Change |
|---------|------|--------|
| 0.1 | 2026-06-20 | Initial test plan |
| 0.2 | 2026-06-21 | Comment/05: Device=Server, module packets, 5-waveform UI, new TCs |