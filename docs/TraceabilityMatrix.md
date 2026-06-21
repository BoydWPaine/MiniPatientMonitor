# Requirements Traceability Matrix

**Project:** MiniPatientMonitor  
**Version:** 0.1  
**Date:** 2026-06-20

Maps SRS requirements → verification test cases.

---

## Device Requirements

| Req ID | Requirement Summary | Test Cases | Status |
|--------|---------------------|------------|--------|
| FR-D01 | TCP streaming to Host | TC-UT-04, TC-IT-01, TC-IT-06 | Planned |
| FR-D02 | Six-parameter simulation | TC-UT-40, TC-IT-02, TC-IT-03, TC-SYS-03 | Planned |
| FR-D03 | LVGL config UI | TC-UT-41, TC-SYS-07 | Planned |
| FR-D04 | Technical alarms | TC-UT-13, TC-IT-05 | Planned |
| FR-D05 | Heartbeat / reconnect | TC-IT-06 | Planned |

## Host Requirements

| Req ID | Requirement Summary | Test Cases | Status |
|--------|---------------------|------------|--------|
| FR-H01 | 1024×768 layout | TC-SYS-01 | Planned |
| FR-H02 | Top status bar | TC-SYS-02 | Planned |
| FR-H03 | Waveform area | TC-SYS-03, TC-IT-02 | Planned |
| FR-H04 | Parameter area + association | TC-SYS-04, TC-IT-03 | Planned |
| FR-H05 | Bottom shortcut bar | TC-SYS-05 | Planned |
| FR-H06 | Physiological alarming 1Hz | TC-UT-20..23, TC-IT-04 | Planned |
| FR-H07 | Technical alarm display | TC-IT-05 | Planned |
| FR-H08 | Data management | TC-UT-31, TC-SYS-08 | Planned |
| FR-H09 | Configuration management | TC-UT-30, TC-IT-07, TC-SYS-10 | Planned |
| FR-H10 | Patient management | TC-UT-32, TC-IT-08, TC-SYS-09 | Planned |

## Common Requirements

| Req ID | Requirement Summary | Test Cases | Status |
|--------|---------------------|------------|--------|
| FR-C01 | OS abstraction layer | TC-UT-01..04 | Planned |
| FR-C02 | Protobuf definitions | TC-UT-10..13 | Planned |
| FR-C03 | Binary file storage | TC-UT-30..32 | Planned |

## Non-Functional Requirements

| Req ID | Requirement Summary | Test Cases | Status |
|--------|---------------------|------------|--------|
| NFR-01 | Waveform ≥25 Hz | TC-IT-02 | Planned |
| NFR-02 | Alarm 1 Hz | TC-UT-23 | Planned |
| NFR-03 | Watchdog logging | _Phase 2_ | Deferred |
| NFR-04 | Unit coverage ≥60% | TestReport §2 | Planned |
| NFR-05 | Windows portability | CI build | Planned |
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