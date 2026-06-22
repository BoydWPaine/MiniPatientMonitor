# Requirements Traceability Matrix

**Project:** MiniPatientMonitor  
**Version:** 0.3  
**Date:** 2026-06-21

Maps SRS requirements → verification test cases.

---

## Device Requirements

| Req ID | Requirement Summary | Test Cases | Status |
|--------|---------------------|------------|--------|
| FR-D01 | TCP server; Host connects | TC-UT-04, TC-IT-01, TC-IT-06 | Planned |
| FR-D02 | Five independent module sims; 12-lead ECG; NIBP on-demand | TC-UT-16, TC-UT-40, TC-UT-42, TC-UT-43, TC-IT-02, TC-IT-03, TC-IT-09, TC-IT-10, TC-SYS-03 | Planned |
| FR-D03 | LVGL all params (ranges) | TC-UT-41, TC-SYS-07 | Planned |
| FR-D04 | Technical alarms (repeated Code) | TC-UT-13, TC-IT-05 | Planned |
| FR-D05 | Heartbeat (NullPacket payload) / reconnect | TC-UT-14, TC-IT-06 | Planned |

## Host Requirements

| Req ID | Requirement Summary | Test Cases | Status |
|--------|---------------------|------------|--------|
| FR-H01 | 1024×768 pixel layout (640:384) | TC-SYS-01 | Planned |
| FR-H02 | Top status bar (180/300/64/300/180, 64 px height) | TC-SYS-02 | Planned |
| FR-H03 | Five waveform rows incl. Temp | TC-SYS-03, TC-IT-02 | Planned |
| FR-H04 | Param order + colors + association | TC-SYS-04, TC-SYS-11, TC-SYS-12, TC-SYS-13, TC-IT-03 | Planned |
| FR-H05 | Bottom 8-slot bar | TC-SYS-05 | Planned |
| FR-H06 | Physiological alarming 1Hz | TC-UT-20..23, TC-IT-04 | Planned |
| FR-H07 | Technical alarm display (localized) | TC-IT-05 | Planned |
| FR-H08 | Data management | TC-UT-31, TC-SYS-08 | Planned |
| FR-H09 | Configuration management | TC-UT-30, TC-IT-07, TC-SYS-10 | Planned |
| FR-H10 | Patient management | TC-UT-32, TC-IT-08, TC-SYS-09 | Planned |

## Common Requirements

| Req ID | Requirement Summary | Test Cases | Status |
|--------|---------------------|------------|--------|
| FR-C01 | OS abstraction layer (Win/Linux) | TC-UT-01..04 | Planned |
| FR-C02 | Protobuf module packets + NibpRequest | TC-UT-10..16 | Planned |
| FR-C03 | Binary file storage | TC-UT-30..32 | Planned |

## Non-Functional Requirements

| Req ID | Requirement Summary | Test Cases | Status |
|--------|---------------------|------------|--------|
| NFR-01 | Waveform ≥25 Hz | TC-IT-02 | Planned |
| NFR-02 | Alarm 1 Hz | TC-UT-23 | Planned |
| NFR-03 | Watchdog logging | _Phase 2_ | Deferred |
| NFR-04 | Unit coverage ≥60% | TestReport §2 | Planned |
| NFR-05 | Windows + Linux portability | CI build | Planned |
| NFR-06 | Documentation complete | Doc review | **Done** |

## Risk Controls

| Risk ID | Control | Test Cases |
|---------|---------|------------|
| R-01 | DEMO ONLY disclaimer | TC-SYS-06 |
| R-02 | Phys alarm verification | TC-UT-20..23, TC-IT-04 |
| R-03 | Tech alarm path | TC-IT-05 |
| R-04 | Config validation | TC-UT-30, TC-SYS-10 |
| R-05 | Storage integrity | TC-UT-31 |
| R-06 | Patient admit checks | TC-IT-08 |

---

## Coverage Summary

| Category | Total Reqs | With Tests | Coverage |
|----------|------------|------------|----------|
| Functional (Device) | 5 | 5 | 100% |
| Functional (Host) | 10 | 10 | 100% |
| Functional (Common) | 3 | 3 | 100% |
| Non-Functional | 6 | 5 | 83% (NFR-03 deferred) |
| **Total** | **24** | **23** | **96%** |

---

## Change History

| Version | Date | Change |
|---------|------|--------|
| 0.1 | 2026-06-20 | Initial matrix |
| 0.2 | 2026-06-21 | Comment/05 Adjust01: updated test mappings |
| 0.3 | 2026-06-21 | Comment/05 Adjust02: NibpRequest, colors, TC-IT-10, TC-SYS-12/13 |