# Risk Analysis (ISO 14971 Style)

**Project:** MiniPatientMonitor  
**Version:** 0.1  
**Date:** 2026-06-20  
**Scope:** Demonstration software — not for clinical use

> This analysis supports portfolio documentation of risk-aware MedTech software practices.  
> Severity / probability scales are qualitative for demo software.

**Severity:** 1 Negligible · 2 Minor · 3 Serious · 4 Critical · 5 Catastrophic  
**Probability:** A Frequent · B Probable · C Occasional · D Remote · E Improbable

---

## 1. Hazard Identification

| ID | Hazard | Hazardous Situation | Harm (if misused clinically) |
|----|--------|---------------------|------------------------------|
| H-01 | Incorrect vital-sign values displayed | User treats demo as real monitor | Wrong clinical decision |
| H-02 | Alarm failure (missed or false) | Abnormal condition not annunciated | Delayed response |
| H-03 | Alarm limit misconfiguration | Limits set incorrectly persist | False sense of safety |
| H-04 | Data loss / corruption | Trend or alarm history lost | Incomplete record |
| H-05 | Misidentification of patient | Wrong patient admitted | Data attributed to wrong subject |
| H-06 | Demo software used clinically | Deployed without validation | Multiple harms |
| H-07 | Network interference (future LAN) | Corrupted packets | Erratic display / missed alarms |
| H-08 | Watchdog failure undetected (MCU) | Silent hang without recovery | Frozen display |

---

## 2. Risk Estimation & Control

| Risk ID | Hazard | Sev | Prob | Risk Control Measure | Residual |
|---------|--------|-----|------|-------------------|----------|
| R-01 | H-01 + H-06 | 5 | D | README/SRS/UI watermark **DEMO ONLY**; synthetic data labeled; no clinical claims | Low |
| R-02 | H-02 phys alarm | 4 | C | Unit tests for `PhysAlarmEngine`; 1 Hz timer audit; integration test HR>limit | Low |
| R-03 | H-02 tech alarm | 3 | C | Integration test Device→Host tech alarm path | Low |
| R-04 | H-03 config | 3 | D | Factory reset; config checksum in protobuf; validation ranges on save | Low |
| R-05 | H-04 data | 2 | C | Append-only with length prefix; file flush; export verification test | Low |
| R-06 | H-05 patient | 3 | D | Single active patient in MVP; admit dialog confirms name/bed | Low |
| R-07 | H-07 network | 3 | E | Phase 1 localhost only; framing length check; discard malformed packets | Low |
| R-08 | H-08 watchdog | 3 | E | Phase 2: IWDG + reset reason log (FR-NFR-03) | Low |

---

## 3. Software Risk Control Categories (IEC 62304 mapping)

| Control | Implementation |
|---------|----------------|
| Risk control by design | Synthetic signal generator; alarm engine isolated module |
| Protective measures | Alarm limit validation; protobuf parse failures dropped |
| Information for safety | README disclaimer; UI demo watermark; SRS §1.2 scope |
| Verification | Test cases TC-INT-03, TC-INT-04, TC-SYS-06 in TestPlan |

---

## 4. SOUP Considerations (future)

| SOUP | Function | Risk | Mitigation |
|------|----------|------|------------|
| Qt6 | Host UI | UI rendering defects | Version pin; smoke tests |
| LVGL | Device UI | Config input errors | Range validation |
| Protobuf | Serialization | Parse errors | try/catch; length limits |
| FreeRTOS | Scheduling (phase 2) | Priority inversion | Mutex priority inheritance |

SOUP list to be maintained in `docs/SOUP.md` (phase M5).

---

## 5. Residual Risk Evaluation

After controls, **all identified risks are acceptable for demonstration/portfolio use** provided:

1. Software is not used for patient monitoring
2. Disclaimer is visible in UI and documentation
3. Verification tests TC-SYS-01 through TC-SYS-10 pass before public demo

---

## 6. Risk Management Review

| Review Point | Trigger |
|--------------|---------|
| Initial | SRS approval (this document) |
| M3 complete | Alarm module integrated |
| M6 complete | Pre-demo release |
| Phase 2 MCU | Watchdog + hardware hazards added |