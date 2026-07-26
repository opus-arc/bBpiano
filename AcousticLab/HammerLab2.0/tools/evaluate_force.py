#!/usr/bin/env python3
"""Compare two hammer-force traces after time-domain interpolation."""

import argparse
import csv
import json
import math
import pathlib


TIME_NAMES = ("time_sec", "time_seconds", "t")
FORCE_NAMES = ("force_n", "force", "F")


def load_trace(path: pathlib.Path) -> tuple[list[float], list[float]]:
    with path.open(newline="", encoding="utf-8-sig") as stream:
        reader = csv.DictReader(stream)
        if not reader.fieldnames:
            raise ValueError(f"{path}: missing header")
        time_name = next((name for name in TIME_NAMES if name in reader.fieldnames), None)
        force_name = next((name for name in FORCE_NAMES if name in reader.fieldnames), None)
        if not time_name or not force_name:
            raise ValueError(f"{path}: expected time_sec and force_n columns")
        pairs = [(float(row[time_name]), float(row[force_name])) for row in reader]
    if len(pairs) < 2:
        raise ValueError(f"{path}: trace is too short")
    pairs.sort()
    return [item[0] for item in pairs], [item[1] for item in pairs]


def interpolate(times: list[float], values: list[float], target: float, cursor: int) -> tuple[float, int]:
    while cursor + 1 < len(times) and times[cursor + 1] < target:
        cursor += 1
    if target <= times[0]:
        return values[0], 0
    if cursor + 1 >= len(times):
        return values[-1], len(times) - 1
    left_t, right_t = times[cursor], times[cursor + 1]
    alpha = (target - left_t) / (right_t - left_t)
    return values[cursor] + alpha * (values[cursor + 1] - values[cursor]), cursor


def contact_duration(times: list[float], forces: list[float], threshold: float) -> float:
    indices = [index for index, force in enumerate(forces) if force >= threshold]
    return 0.0 if not indices else times[indices[-1]] - times[indices[0]]


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("reference", type=pathlib.Path)
    parser.add_argument("candidate", type=pathlib.Path)
    parser.add_argument("--output", type=pathlib.Path)
    parser.add_argument("--threshold-ratio", type=float, default=0.01)
    args = parser.parse_args()

    ref_t, ref_f = load_trace(args.reference)
    can_t, can_f = load_trace(args.candidate)
    start, end = max(ref_t[0], can_t[0]), min(ref_t[-1], can_t[-1])
    grid = [time for time in ref_t if start <= time <= end]
    if len(grid) < 2:
        raise ValueError("traces do not overlap")
    cursor = 0
    aligned_candidate = []
    for time in grid:
        value, cursor = interpolate(can_t, can_f, time, cursor)
        aligned_candidate.append(value)
    aligned_reference = [ref_f[index] for index, time in enumerate(ref_t) if start <= time <= end]

    errors = [candidate - reference for reference, candidate in zip(aligned_reference, aligned_candidate)]
    peak_ref, peak_can = max(aligned_reference), max(aligned_candidate)
    rmse = math.sqrt(sum(value * value for value in errors) / len(errors))
    mean_ref = sum(aligned_reference) / len(aligned_reference)
    mean_can = sum(aligned_candidate) / len(aligned_candidate)
    numerator = sum((a - mean_ref) * (b - mean_can) for a, b in zip(aligned_reference, aligned_candidate))
    denominator = math.sqrt(
        sum((a - mean_ref) ** 2 for a in aligned_reference)
        * sum((b - mean_can) ** 2 for b in aligned_candidate)
    )
    threshold_ref = args.threshold_ratio * peak_ref
    threshold_can = args.threshold_ratio * peak_can
    report = {
        "schema_version": 1,
        "reference": str(args.reference.resolve()),
        "candidate": str(args.candidate.resolve()),
        "samples_compared": len(grid),
        "peak_reference_n": peak_ref,
        "peak_candidate_n": peak_can,
        "peak_relative_error": (peak_can - peak_ref) / peak_ref if peak_ref else None,
        "rmse_n": rmse,
        "normalized_rmse": rmse / peak_ref if peak_ref else None,
        "correlation": numerator / denominator if denominator else None,
        "reference_contact_ms": 1000.0 * contact_duration(grid, aligned_reference, threshold_ref),
        "candidate_contact_ms": 1000.0 * contact_duration(grid, aligned_candidate, threshold_can),
        "reference_impulse_ns": sum(aligned_reference) * (grid[-1] - grid[0]) / (len(grid) - 1),
        "candidate_impulse_ns": sum(aligned_candidate) * (grid[-1] - grid[0]) / (len(grid) - 1),
    }
    payload = json.dumps(report, indent=2) + "\n"
    if args.output:
        args.output.parent.mkdir(parents=True, exist_ok=True)
        args.output.write_text(payload)
    print(payload, end="")


if __name__ == "__main__":
    main()
