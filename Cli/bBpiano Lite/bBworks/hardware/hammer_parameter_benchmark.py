#!/usr/bin/env python3
"""Structured bBworks hammer-parameter search against paired C4 WAVs."""

from __future__ import annotations

import argparse
import csv
import json
import math
import subprocess
import tempfile
from dataclasses import dataclass, replace
from pathlib import Path

import numpy as np

from hammer_analyzer import analyze, load_force_file, select_first_contact
from hammer_modal_benchmark import (
    StringSpec,
    audio_modal_fingerprint,
    estimate_modal_frequencies,
    force_modal_fingerprint,
)


MIDI_VELOCITIES = (50, 65, 80, 95, 110)
TAKES = {50: 1, 65: 2, 80: 3, 95: 4, 110: 5}
BASELINE_MIDI = 80
REFERENCE_PCM_DIR: Path | None = None


@dataclass(frozen=True)
class Parameters:
    label: str
    hardness: float
    exponent: float
    epsilon: float
    tau0_s: float
    linear_density: float
    velocity_scale: float
    stage: str


def goebl_velocity(midi_velocity: int) -> float:
    u = midi_velocity / 127.0
    travel_time_ms = 200.0 + (20.0 - 200.0) * u
    velocity = (travel_time_ms / 89.16) ** (1.0 / -0.570)
    return float(np.clip(velocity, 0.18, 7.5))


def parse_key_values(text: str) -> dict[str, str]:
    result = {}
    for line in text.splitlines():
        if "=" in line:
            key, value = line.split("=", 1)
            result[key.strip()] = value.strip().strip('"')
    return result


def target_wav(project_root: Path, midi_velocity: int) -> Path:
    take = TAKES[midi_velocity]
    if REFERENCE_PCM_DIR is not None:
        return (
            REFERENCE_PCM_DIR
            / f"C4_take{take:02d}_v{midi_velocity}.wav"
        )
    return (
        project_root
        / "AcousticLab"
        / "StringFilterLab"
        / "Samples"
        / "Pianoteq 9"
        / "SingleNoteSamples"
        / "Old Version"
        / "3"
        / f"v{midi_velocity}"
        / f"C4_take{take:02d}_v{midi_velocity}.wav"
    )


def run_guard(
    guard_binary: Path,
    parameters: Parameters,
    midi_velocity: int,
    force_log: Path,
) -> tuple[dict, dict]:
    physical_velocity = (
        goebl_velocity(midi_velocity) * parameters.velocity_scale
    )
    command = [
        str(guard_binary),
        "--velocity",
        f"{physical_velocity:.17g}",
        "--hardness",
        f"{parameters.hardness:.17g}",
        "--p",
        f"{parameters.exponent:.17g}",
        "--epsilon",
        f"{parameters.epsilon:.17g}",
        "--tau0",
        f"{parameters.tau0_s:.17g}",
        "--linear-density",
        f"{parameters.linear_density:.17g}",
        "--force-log",
        str(force_log),
    ]
    completed = subprocess.run(
        command,
        check=False,
        capture_output=True,
        text=True,
    )
    if completed.returncode != 0:
        raise RuntimeError(
            completed.stderr.strip() or completed.stdout.strip()
        )
    raw = parse_key_values(completed.stdout)
    if raw.get("status") != "PASS":
        raise RuntimeError("Energy guard did not pass.")

    time, force = load_force_file(force_log)
    time, force, event_count = select_first_contact(time, force)
    metrics = analyze(time, force)
    metrics["contact_event_count"] = event_count
    metrics["maximum_mechanical_energy_ratio"] = float(
        raw["maximum_mechanical_energy_ratio"]
    )
    metrics["rebound_velocity_m_s"] = float(
        raw["rebound_velocity_m_s"]
    )
    metrics["physical_velocity_m_s"] = physical_velocity
    return raw, metrics


def build_targets(
    project_root: Path,
    spec: StringSpec,
    minimum_snr_db: float,
) -> tuple[np.ndarray, dict[int, dict], dict[int, np.ndarray]]:
    reference = target_wav(project_root, BASELINE_MIDI)
    frequencies = estimate_modal_frequencies(
        reference,
        spec,
        onset_ms=0.0,
        post_contact_ms=5.0,
    )
    targets = {}
    valid_masks = {}
    for midi_velocity in MIDI_VELOCITIES:
        target = audio_modal_fingerprint(
            target_wav(project_root, midi_velocity),
            spec,
            "microphone",
            onset_ms=0.0,
            post_contact_ms=5.0,
            window_ms=100.0,
            modal_frequencies_hz=frequencies,
        )
        targets[midi_velocity] = target

    baseline_snr = np.asarray(
        targets[BASELINE_MIDI]["snr_db"],
        dtype=float,
    )
    for midi_velocity in MIDI_VELOCITIES:
        target_snr = np.asarray(
            targets[midi_velocity]["snr_db"],
            dtype=float,
        )
        valid_masks[midi_velocity] = (
            (baseline_snr >= minimum_snr_db)
            & (target_snr >= minimum_snr_db)
        )
    return frequencies, targets, valid_masks


def paired_score(
    candidates: dict[int, dict],
    targets: dict[int, dict],
    valid_masks: dict[int, np.ndarray],
) -> tuple[float, float, float, list[dict]]:
    base_candidate = np.asarray(
        candidates[BASELINE_MIDI]["modal_energy_J"],
        dtype=float,
    )
    base_target = np.asarray(
        targets[BASELINE_MIDI]["energy_proxy"],
        dtype=float,
    )
    all_shape_error = []
    rows = []

    for midi_velocity in MIDI_VELOCITIES:
        if midi_velocity == BASELINE_MIDI:
            continue
        candidate = np.asarray(
            candidates[midi_velocity]["modal_energy_J"],
            dtype=float,
        )
        target = np.asarray(
            targets[midi_velocity]["energy_proxy"],
            dtype=float,
        )
        valid = valid_masks[midi_velocity].copy()
        valid &= (
            np.isfinite(candidate)
            & np.isfinite(base_candidate)
            & np.isfinite(target)
            & np.isfinite(base_target)
            & (candidate > np.max(candidate) * 1e-8)
            & (base_candidate > np.max(base_candidate) * 1e-8)
            & (target > 0.0)
            & (base_target > 0.0)
        )
        if np.sum(valid) < 3:
            raise RuntimeError(
                f"Only {np.sum(valid)} modes at MIDI {midi_velocity}."
            )

        candidate_gain_db = 10.0 * np.log10(
            candidate[valid] / base_candidate[valid]
        )
        target_gain_db = 10.0 * np.log10(
            target[valid] / base_target[valid]
        )
        raw_error_db = target_gain_db - candidate_gain_db
        gain_offset_db = float(np.median(raw_error_db))
        shape_error_db = raw_error_db - gain_offset_db
        all_shape_error.extend(shape_error_db.tolist())

        valid_modes = np.arange(1, len(valid) + 1)[valid]
        for index, mode in enumerate(valid_modes):
            rows.append({
                "midi_velocity": midi_velocity,
                "mode": int(mode),
                "candidate_gain_db": float(candidate_gain_db[index]),
                "target_gain_db": float(target_gain_db[index]),
                "shape_error_db": float(shape_error_db[index]),
                "gain_offset_db": gain_offset_db,
            })

    errors = np.asarray(all_shape_error, dtype=float)
    return (
        float(np.sqrt(np.mean(np.square(errors)))),
        float(np.median(np.abs(errors))),
        float(np.percentile(np.abs(errors), 90)),
        rows,
    )


def _range_penalty(
    value: float,
    minimum: float,
    maximum: float,
    lower_scale: float,
    upper_scale: float,
) -> float:
    if value < minimum:
        return (minimum - value) / lower_scale
    if value > maximum:
        return (value - maximum) / upper_scale
    return 0.0


def physical_gate_penalty(row: dict) -> float:
    if row.get("status") != "PASS":
        return math.inf
    try:
        duration = float(row["midi80_contact_duration_ms"])
        maximum_force = float(row["midi80_Fmax_N"])
        peak_position = float(row["midi80_peak_position"])
        rise_fall = float(row["midi80_rise_fall_ratio"])
        rebound_velocity = float(
            row["midi80_rebound_velocity_m_s"]
        )
        energy_ratio = float(row["max_energy_ratio"])
    except (KeyError, TypeError, ValueError):
        return math.inf

    components = [
        _range_penalty(duration, 1.2, 3.5, 1.2, 2.0),
        _range_penalty(maximum_force, 5.0, 60.0, 5.0, 60.0),
        _range_penalty(peak_position, 0.15, 0.85, 0.15, 0.15),
        max(0.0, energy_ratio - 1.000001) / 0.01,
    ]
    if not math.isfinite(rise_fall):
        components.append(2.0)
    if rebound_velocity >= 0.0:
        components.append(
            1.0 + rebound_velocity / max(
                float(row["midi80_velocity_m_s"]),
                1.0e-12,
            )
        )
    return float(sum(component * component for component in components))


def fallback_curve_is_admissible(row: dict) -> bool:
    if row.get("status") != "PASS":
        return False
    try:
        return bool(
            5.0 <= float(row["midi80_Fmax_N"]) <= 60.0
            and 0.15 <= float(row["midi80_peak_position"]) <= 0.85
            and math.isfinite(float(row["midi80_rise_fall_ratio"]))
            and float(row["midi80_rebound_velocity_m_s"]) < 0.0
            and float(row["max_energy_ratio"]) <= 1.000001
        )
    except (KeyError, TypeError, ValueError):
        return False


def evaluate(
    index: int,
    parameters: Parameters,
    guard_binary: Path,
    spec: StringSpec,
    frequencies: np.ndarray,
    targets: dict[int, dict],
    valid_masks: dict[int, np.ndarray],
) -> tuple[dict, list[dict]]:
    row = {
        "index": index,
        "label": parameters.label,
        "stage": parameters.stage,
        "hardness": parameters.hardness,
        "p": parameters.exponent,
        "epsilon": parameters.epsilon,
        "tau0_s": parameters.tau0_s,
        "linear_density_kg_m": parameters.linear_density,
        "tension_N": (
            parameters.linear_density
            * (2.0 * spec.length_m * spec.f0) ** 2
        ),
        "velocity_scale": parameters.velocity_scale,
        "midi80_velocity_m_s": (
            goebl_velocity(BASELINE_MIDI)
            * parameters.velocity_scale
        ),
        "status": "FAIL",
    }
    detail_rows = []
    try:
        with tempfile.TemporaryDirectory(
            prefix="bbpiano-hammer-search-"
        ) as temporary:
            temporary = Path(temporary)
            candidate_fingerprints = {}
            force_metrics = {}
            for midi_velocity in MIDI_VELOCITIES:
                force_log = temporary / f"force_{midi_velocity}.txt"
                _, metrics = run_guard(
                    guard_binary,
                    parameters,
                    midi_velocity,
                    force_log,
                )
                force_metrics[midi_velocity] = metrics
                run_spec = replace(
                    spec,
                    impact_velocity_m_s=metrics["physical_velocity_m_s"],
                    linear_density_kg_m=parameters.linear_density,
                )
                candidate_fingerprints[midi_velocity] = (
                    force_modal_fingerprint(
                        force_log,
                        run_spec,
                        modal_frequencies_hz=frequencies,
                    )
                )

            rmse, median_error, p90_error, detail_rows = paired_score(
                candidate_fingerprints,
                targets,
                valid_masks,
            )
            v80 = force_metrics[BASELINE_MIDI]
            duration_ms = 1000.0 * v80["contact_duration_s"]
            force_curve_gate = bool(
                1.2 <= duration_ms <= 3.5
                and 5.0 <= v80["Fmax_N"] <= 60.0
                and 0.15 <= v80["peak_position_ratio"] <= 0.85
                and math.isfinite(v80["rise_fall_ratio"])
                and v80["contact_event_count"] == 1
                and v80["rebound_velocity_m_s"] < 0.0
            )
            row.update({
                "status": "PASS",
                "paired_shape_rmse_db": rmse,
                "median_abs_shape_error_db": median_error,
                "p90_abs_shape_error_db": p90_error,
                "force_curve_gate_pass": force_curve_gate,
                "midi80_contact_duration_ms": duration_ms,
                "midi80_contact_span_ms": (
                    1000.0 * v80["contact_span_s"]
                ),
                "midi80_Fmax_N": v80["Fmax_N"],
                "midi80_peak_position": v80["peak_position_ratio"],
                "midi80_impulse_Ns": v80["impulse_Ns"],
                "midi80_rise_fall_ratio": v80["rise_fall_ratio"],
                "midi80_rebound_velocity_m_s": (
                    v80["rebound_velocity_m_s"]
                ),
                "max_energy_ratio": max(
                    metrics["maximum_mechanical_energy_ratio"]
                    for metrics in force_metrics.values()
                ),
            })
            row["physical_gate_penalty"] = physical_gate_penalty(row)
    except Exception as error:
        row["error"] = str(error)
    return row, detail_rows


def anchor_and_axis_candidates() -> list[Parameters]:
    current = Parameters(
        "current_baseline",
        400.0,
        2.4,
        0.51,
        4.0e-4,
        0.00623958204318,
        1.0,
        "anchor",
    )
    candidates = [
        current,
        Parameters(
            "stulov_published_anchor",
            8800.0,
            3.95,
            0.992,
            2.0e-6,
            0.00623958204318,
            1.0,
            "anchor",
        ),
        Parameters(
            "requested_original_anchor",
            40000.0,
            5.0,
            0.998,
            6.0e-4,
            0.00623958204318,
            1.0,
            "anchor",
        ),
    ]
    axes = {
        "hardness": [
            200.0, 400.0, 800.0, 1600.0, 3200.0,
            8000.0, 16000.0, 40000.0,
        ],
        "exponent": [1.5, 2.0, 2.4, 3.0, 3.95, 5.0, 6.0, 8.0],
        "epsilon": [0.10, 0.30, 0.51, 0.75, 0.90, 0.97, 0.992, 0.998],
        "tau0_s": [
            1e-6, 2e-6, 1e-5, 5e-5, 1e-4,
            2e-4, 4e-4, 8e-4, 1.5e-3, 2e-3,
        ],
        "linear_density": [
            0.0057, 0.0059, 0.00623958204318,
            0.0065, 0.0068, 0.007,
        ],
        "velocity_scale": [0.6, 0.7, 0.85, 1.0, 1.15, 1.3, 1.5, 1.6],
    }
    for field, values in axes.items():
        for value in values:
            arguments = {
                "label": f"axis_{field}_{value:g}",
                "hardness": current.hardness,
                "exponent": current.exponent,
                "epsilon": current.epsilon,
                "tau0_s": current.tau0_s,
                "linear_density": current.linear_density,
                "velocity_scale": current.velocity_scale,
                "stage": "one_factor",
            }
            arguments[field] = value
            candidates.append(Parameters(**arguments))
    return candidates


def latin_hypercube_candidates(
    count: int,
    seed: int,
) -> list[Parameters]:
    rng = np.random.default_rng(seed)
    unit = np.empty((count, 6), dtype=float)
    for dimension in range(unit.shape[1]):
        unit[:, dimension] = (
            rng.permutation(count) + rng.random(count)
        ) / count
    lower = np.array([
        math.log10(100.0),
        1.5,
        0.05,
        math.log10(1.0e-6),
        0.0057,
        0.6,
    ])
    upper = np.array([
        math.log10(40000.0),
        8.0,
        0.999,
        math.log10(2.0e-3),
        0.0068,
        1.6,
    ])
    values = lower + unit * (upper - lower)
    return [
        Parameters(
            f"lhs_{index:04d}",
            10.0 ** row[0],
            row[1],
            row[2],
            10.0 ** row[3],
            row[4],
            row[5],
            "space_filling",
        )
        for index, row in enumerate(values)
    ]


def refinement_candidates(
    winners: list[dict],
    per_winner: int,
    seed: int,
) -> list[Parameters]:
    rng = np.random.default_rng(seed)
    bounds = np.array([
        [math.log10(100.0), math.log10(40000.0)],
        [1.5, 8.0],
        [0.05, 0.999],
        [math.log10(1.0e-6), math.log10(2.0e-3)],
        [0.0057, 0.0068],
        [0.6, 1.6],
    ])
    span = bounds[:, 1] - bounds[:, 0]
    candidates = []
    for winner_index, winner in enumerate(winners):
        center = np.array([
            math.log10(winner["hardness"]),
            winner["p"],
            winner["epsilon"],
            math.log10(winner["tau0_s"]),
            winner["linear_density_kg_m"],
            winner["velocity_scale"],
        ])
        for sample_index in range(per_winner):
            point = center + rng.normal(0.0, 0.06, size=6) * span
            point = np.clip(point, bounds[:, 0], bounds[:, 1])
            candidates.append(Parameters(
                f"refine_{winner_index:02d}_{sample_index:03d}",
                10.0 ** point[0],
                point[1],
                point[2],
                10.0 ** point[3],
                point[4],
                point[5],
                "refinement",
            ))
    return candidates


def select_refinement_seeds(
    rows: list[dict],
    count: int,
) -> tuple[list[dict], str]:
    full_gate = sorted(
        (
            row for row in rows
            if row.get("status") == "PASS"
            and row.get("force_curve_gate_pass")
        ),
        key=lambda row: row["paired_shape_rmse_db"],
    )
    if full_gate:
        return full_gate[:count], "full_physical_gate"

    successful = [
        row for row in rows
        if row.get("status") == "PASS"
    ]
    admissible = sorted(
        (row for row in successful if fallback_curve_is_admissible(row)),
        key=lambda row: row["paired_shape_rmse_db"],
    )
    near_gate = sorted(
        successful,
        key=lambda row: (
            physical_gate_penalty(row),
            row["paired_shape_rmse_db"],
        ),
    )
    shortest_rebound = sorted(
        (
            row for row in successful
            if fallback_curve_is_admissible(row)
        ),
        key=lambda row: (
            row["midi80_contact_duration_ms"],
            row["paired_shape_rmse_db"],
        ),
    )

    selected = []
    seen = set()
    pools = (admissible, near_gate, shortest_rebound)
    while len(selected) < count:
        added = False
        for pool in pools:
            if not pool:
                continue
            row = pool.pop(0)
            index = int(row["index"])
            if index in seen:
                continue
            seen.add(index)
            selected.append(row)
            added = True
            if len(selected) == count:
                break
        if not added:
            break
    return selected, "near_gate_fallback"


def write_csv(path: Path, rows: list[dict]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    fieldnames = sorted({key for row in rows for key in row})
    with path.open("w", newline="", encoding="utf-8") as stream:
        writer = csv.DictWriter(stream, fieldnames=fieldnames)
        writer.writeheader()
        writer.writerows(rows)


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--guard-binary",
        default="/private/tmp/bbpiano_hammer_energy_guard",
    )
    parser.add_argument(
        "--output-dir",
        default="hardware/parameter_benchmark_results/search",
    )
    parser.add_argument("--samples", type=int, default=180)
    parser.add_argument("--refine-winners", type=int, default=8)
    parser.add_argument("--refine-per-winner", type=int, default=15)
    parser.add_argument("--seed", type=int, default=20260728)
    parser.add_argument("--minimum-snr-db", type=float, default=0.0)
    parser.add_argument("--modes", type=int, default=13)
    parser.add_argument(
        "--reference-dir",
        help=(
            "Directory containing PCM C4_takeNN_vVV.wav target copies. "
            "When omitted, read the original project recordings."
        ),
    )
    return parser


def main() -> None:
    arguments = build_parser().parse_args()
    global REFERENCE_PCM_DIR
    if arguments.reference_dir:
        REFERENCE_PCM_DIR = Path(arguments.reference_dir).resolve()
        if not REFERENCE_PCM_DIR.is_dir():
            raise RuntimeError(
                f"Missing reference directory: {REFERENCE_PCM_DIR}"
            )
    script = Path(__file__).resolve()
    project_root = script.parents[4]
    output_dir = Path(arguments.output_dir).resolve()
    output_dir.mkdir(parents=True, exist_ok=True)
    guard_binary = Path(arguments.guard_binary).resolve()
    if not guard_binary.exists():
        raise RuntimeError(f"Missing guard binary: {guard_binary}")

    spec = StringSpec(
        f0=261.626,
        length_m=0.65,
        linear_density_kg_m=0.00623958204318,
        mode_count=arguments.modes,
        hammer_mass_kg=0.015,
    )
    frequencies, targets, valid_masks = build_targets(
        project_root,
        spec,
        arguments.minimum_snr_db,
    )

    candidates = anchor_and_axis_candidates()
    candidates.extend(
        latin_hypercube_candidates(arguments.samples, arguments.seed)
    )
    rows = []
    details_by_index = {}
    for index, parameters in enumerate(candidates):
        row, details = evaluate(
            index,
            parameters,
            guard_binary,
            spec,
            frequencies,
            targets,
            valid_masks,
        )
        rows.append(row)
        details_by_index[index] = details

    refinement_seeds, refinement_basis = select_refinement_seeds(
        rows,
        arguments.refine_winners,
    )
    refinement = refinement_candidates(
        refinement_seeds,
        arguments.refine_per_winner,
        arguments.seed + 1,
    )
    start_index = len(rows)
    for offset, parameters in enumerate(refinement):
        index = start_index + offset
        row, details = evaluate(
            index,
            parameters,
            guard_binary,
            spec,
            frequencies,
            targets,
            valid_masks,
        )
        rows.append(row)
        details_by_index[index] = details

    full_gate = sorted(
        (
            row for row in rows
            if row.get("status") == "PASS"
            and row.get("force_curve_gate_pass")
        ),
        key=lambda row: row["paired_shape_rmse_db"],
    )
    write_csv(output_dir / "all_candidates.csv", rows)
    if full_gate:
        ranked = full_gate
        selection_basis = "full_physical_gate_then_rmse"
    else:
        fallback = sorted(
            (
                row for row in rows
                if fallback_curve_is_admissible(row)
            ),
            key=lambda row: (
                row["paired_shape_rmse_db"],
                physical_gate_penalty(row),
            ),
        )
        if fallback:
            ranked = fallback
            selection_basis = (
                "complete_rebound_curve_then_rmse"
            )
        else:
            ranked = sorted(
                (
                    row for row in rows
                    if row.get("status") == "PASS"
                ),
                key=lambda row: (
                    physical_gate_penalty(row),
                    row["paired_shape_rmse_db"],
                ),
            )
            selection_basis = "minimum_physical_gate_penalty_then_rmse"
    if not ranked:
        raise RuntimeError("No candidate completed evaluation.")
    winner = ranked[0]

    write_csv(output_dir / "top_candidates.csv", ranked[:30])
    write_csv(
        output_dir / "winner_paired_residuals.csv",
        details_by_index[int(winner["index"])],
    )
    payload = {
        "winner": winner,
        "candidate_count": len(rows),
        "passing_candidate_count": len(full_gate),
        "selection_basis": selection_basis,
        "refinement_basis": refinement_basis,
        "minimum_snr_db": arguments.minimum_snr_db,
        "mode_count": spec.mode_count,
        "modal_frequencies_hz": frequencies.tolist(),
        "goebl_velocity_m_s": {
            str(midi): goebl_velocity(midi)
            for midi in MIDI_VELOCITIES
        },
        "target_wavs": {
            str(midi): str(target_wav(project_root, midi))
            for midi in MIDI_VELOCITIES
        },
    }
    (output_dir / "winner.json").write_text(
        json.dumps(payload, indent=2, ensure_ascii=False),
        encoding="utf-8",
    )
    print(json.dumps(payload, indent=2, ensure_ascii=False))


if __name__ == "__main__":
    main()
