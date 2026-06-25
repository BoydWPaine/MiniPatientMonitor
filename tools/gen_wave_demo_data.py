#!/usr/bin/env python3
"""Generate device/src/wave_demo_data.h from data/WaveData.csv."""

from __future__ import annotations

import csv
import sys
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[1]
CSV_PATH = REPO_ROOT / "data" / "WaveData.csv"
OUT_PATH = REPO_ROOT / "device" / "src" / "wave_demo_data.h"

COLUMNS = [
    ("I", "kWaveLeadI"),
    ("II", "kWaveLeadII"),
    ("III", "kWaveLeadIII"),
    ("aVR", "kWaveLeadAvr"),
    ("aVL", "kWaveLeadAvl"),
    ("aVF", "kWaveLeadAvf"),
    ("V1", "kWaveLeadV1"),
    ("V2", "kWaveLeadV2"),
    ("V3", "kWaveLeadV3"),
    ("V4", "kWaveLeadV4"),
    ("V5", "kWaveLeadV5"),
    ("V6", "kWaveLeadV6"),
    ("Pleth", "kWavePleth"),
    ("Resp", "kWaveResp"),
    ("Temp", "kWaveTemp"),
]


def load_columns(path: Path) -> dict[str, list[int]]:
    with path.open(newline="") as handle:
        reader = csv.DictReader(handle)
        if reader.fieldnames is None:
            raise ValueError(f"Missing header in {path}")

        missing = [name for name, _ in COLUMNS if name not in reader.fieldnames]
        if missing:
            raise ValueError(f"Missing columns in {path}: {', '.join(missing)}")

        columns = {name: [] for name, _ in COLUMNS}
        for row in reader:
            for name, _ in COLUMNS:
                columns[name].append(int(row[name]))

    return columns


def format_array(name: str, values: list[int]) -> str:
    lines = [f"inline constexpr int32_t {name}[kDemoSampleCount] = {{"]
    for index in range(0, len(values), 10):
        chunk = ", ".join(str(value) for value in values[index : index + 10])
        lines.append(f"    {chunk},")
    lines.append("};")
    return "\n".join(lines)


def generate_header(columns: dict[str, list[int]]) -> str:
    counts = {len(values) for values in columns.values()}
    if len(counts) != 1:
        raise ValueError(f"Inconsistent sample counts: {counts}")
    sample_count = counts.pop()

    parts = [
        "#ifndef MPM_DEVICE_WAVE_DEMO_DATA_H",
        "#define MPM_DEVICE_WAVE_DEMO_DATA_H",
        "",
        "#include <cstdint>",
        "",
        "namespace mpm::device {",
        "",
        f"inline constexpr uint32_t kDemoSampleCount = {sample_count};",
        "",
    ]

    for column_name, array_name in COLUMNS:
        parts.append(format_array(array_name, columns[column_name]))
        parts.append("")

    parts.extend(
        [
            "inline int32_t demo_wave_sample(const int32_t* data, uint32_t index)",
            "{",
            "    return data[index % kDemoSampleCount];",
            "}",
            "",
            "}  // namespace mpm::device",
            "",
            "#endif",
            "",
        ]
    )
    return "\n".join(parts)


def main() -> int:
    if not CSV_PATH.exists():
        print(f"error: {CSV_PATH} not found", file=sys.stderr)
        return 1

    columns = load_columns(CSV_PATH)
    OUT_PATH.write_text(generate_header(columns))
    print(f"Wrote {OUT_PATH} ({columns['II'].__len__()} samples)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())