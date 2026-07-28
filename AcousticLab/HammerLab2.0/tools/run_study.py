#!/usr/bin/env python3
"""Run a resumable Pianoteq 8 HammerLab study from one versioned config."""

from __future__ import annotations

import argparse
import datetime as dt
import itertools
import json
import math
import os
import pathlib
import sys
import wave
from collections import defaultdict
from typing import Any, Mapping

from generate_probe_midis import midi_file
from pianoteq8_renderer import (
    PianoteqEngine,
    PianoteqRenderError,
    file_sha256,
    inspect_pcm_wav,
    json_sha256,
    render_wav,
    write_json_atomic,
)


def load_study(path: pathlib.Path) -> dict[str, Any]:
    study = json.loads(path.read_text(encoding="utf-8"))
    required = ("schema_version", "study_id", "engine", "preset", "render", "heartbeat")
    missing = [key for key in required if key not in study]
    if missing:
        raise ValueError(f"study configuration missing: {', '.join(missing)}")
    if study["schema_version"] != 1:
        raise ValueError("unsupported study schema_version")
    if not str(study["study_id"]).strip():
        raise ValueError("study_id must not be empty")
    return study


def _delta_label(delta: float) -> str:
    if delta == 0.0:
        return "base"
    sign = "p" if delta > 0.0 else "m"
    return f"{sign}{abs(delta):.4f}".replace(".", "p")


def expand_heartbeat_cases(study: Mapping[str, Any]) -> list[dict[str, Any]]:
    heartbeat = study["heartbeat"]
    note = int(heartbeat["midi_note"])
    if not 0 <= note <= 127:
        raise ValueError("heartbeat midi_note must be in 0..127")
    delta = float(heartbeat["delta"])
    if not delta > 0.0:
        raise ValueError("heartbeat delta must be positive")
    repeats = int(heartbeat["repeats"])
    if repeats < 1:
        raise ValueError("heartbeat repeats must be positive")
    groups = heartbeat.get("groups", [])
    if not groups:
        raise ValueError("heartbeat groups must not be empty")

    base_parameters = {
        str(key): value for key, value in study.get("fixed_parameters", {}).items()
    }
    cases: list[dict[str, Any]] = []
    identifiers: set[str] = set()
    for group in groups:
        velocity = int(group["midi_velocity"])
        if not 1 <= velocity <= 127:
            raise ValueError("heartbeat midi_velocity must be in 1..127")
        parameter_id = str(group["parameter_id"])
        control_name = str(group["control_name"])
        baseline = float(group["baseline"])
        if not parameter_id.isdigit():
            raise ValueError(f"invalid heartbeat parameter_id: {parameter_id!r}")
        for perturbation in (-delta, 0.0, delta):
            value = baseline + perturbation
            if not 0.0 <= value <= 2.0:
                raise ValueError(
                    f"hardness value outside Pianoteq range 0..2: {value}"
                )
            for repeat in range(1, repeats + 1):
                case_id = (
                    f"n{note:03d}_v{velocity:03d}_{control_name}_"
                    f"{_delta_label(perturbation)}_r{repeat:02d}"
                )
                if case_id in identifiers:
                    raise ValueError(f"duplicate case_id: {case_id}")
                identifiers.add(case_id)
                parameters = dict(base_parameters)
                parameters[parameter_id] = value
                cases.append(
                    {
                        "case_id": case_id,
                        "midi_note": note,
                        "midi_velocity": velocity,
                        "control_name": control_name,
                        "parameter_id": parameter_id,
                        "baseline": baseline,
                        "perturbation": perturbation,
                        "parameter_value": value,
                        "repeat": repeat,
                        "parameters": parameters,
                    }
                )
    return cases


def _render_options(study: Mapping[str, Any]) -> dict[str, Any]:
    render = study["render"]
    channels = int(render["channels"])
    return {
        "sample_rate_hz": int(render["sample_rate_hz"]),
        "bit_depth": int(render["bit_depth"]),
        "channels": channels,
        "dither": bool(render.get("dither", False)),
        "normalize": bool(render.get("normalize", False)),
        "maximum_duration_seconds": (
            float(render["maximum_duration_seconds"])
            if "maximum_duration_seconds" in render
            else None
        ),
        "minimum_duration_seconds": float(
            render.get("minimum_duration_seconds", 0.05)
        ),
        "reject_clipping": bool(render.get("reject_clipping", True)),
    }


def _make_probe(study: Mapping[str, Any], case: Mapping[str, Any], path: pathlib.Path) -> None:
    midi = study["midi"]
    payload = midi_file(
        int(case["midi_note"]),
        int(case["midi_velocity"]),
        float(midi["pre_roll_seconds"]),
        float(midi["hold_seconds"]),
        float(midi["tail_seconds"]),
    )
    path.parent.mkdir(parents=True, exist_ok=True)
    if not path.exists() or path.read_bytes() != payload:
        path.write_bytes(payload)


def _case_signature(
    study: Mapping[str, Any],
    case: Mapping[str, Any],
    midi_path: pathlib.Path,
) -> str:
    return json_sha256(
        {
            "study_schema_version": study["schema_version"],
            "engine": study["engine"],
            "preset": study["preset"],
            "render": study["render"],
            "midi": study["midi"],
            "case": case,
            "midi_sha256": file_sha256(midi_path),
        }
    )


def _resume_record(
    record_path: pathlib.Path,
    audio_path: pathlib.Path,
    signature: str,
    render_options: Mapping[str, Any],
    *,
    pianoteq_version: str,
    executable_sha256: str | None,
) -> dict[str, Any] | None:
    if not record_path.is_file() or not audio_path.is_file():
        return None
    try:
        record = json.loads(record_path.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError):
        return None
    if record.get("case_signature") != signature:
        return None
    if record.get("pianoteq_version") != pianoteq_version:
        return None
    if record.get("executable_sha256") != executable_sha256:
        return None
    validation = inspect_pcm_wav(
        audio_path,
        expected_sample_rate_hz=int(render_options["sample_rate_hz"]),
        expected_bit_depth=int(render_options["bit_depth"]),
        expected_channels=int(render_options["channels"]),
        minimum_duration_seconds=float(render_options["minimum_duration_seconds"]),
        reject_clipping=bool(render_options["reject_clipping"]),
    )
    if not validation["valid"]:
        return None
    if validation["file_sha256"] != record.get("validation", {}).get("file_sha256"):
        return None
    # A study directory may be moved outside the Xcode workspace.  The cached
    # record's original absolute path is provenance, but the active report must
    # always read the currently resolved case path.
    record["output_path"] = str(audio_path.resolve())
    record["validation"] = validation
    return record


def _read_pcm_samples(path: pathlib.Path) -> list[int]:
    """Decode integer PCM samples without adding a NumPy dependency to capture."""
    with wave.open(str(path), "rb") as stream:
        if stream.getcomptype() != "NONE":
            raise ValueError(f"compressed WAV is unsupported: {path}")
        width = stream.getsampwidth()
        if width not in (1, 2, 3, 4):
            raise ValueError(f"unsupported PCM width {width}: {path}")
        raw = stream.readframes(stream.getnframes())

    samples: list[int] = []
    for offset in range(0, len(raw), width):
        encoded = raw[offset : offset + width]
        if len(encoded) != width:
            raise ValueError(f"truncated PCM sample: {path}")
        if width == 1:
            samples.append(encoded[0] - 128)
        else:
            samples.append(int.from_bytes(encoded, "little", signed=True))
    return samples


def _rms(samples: list[float] | list[int], length: int | None = None) -> float:
    count = len(samples) if length is None else min(length, len(samples))
    if count == 0:
        return 0.0
    return math.sqrt(math.fsum(float(samples[index]) ** 2 for index in range(count)) / count)


def _normalized_rms_difference(
    left: list[float] | list[int],
    right: list[float] | list[int],
) -> float:
    count = min(len(left), len(right))
    if count == 0:
        return math.inf
    difference_rms = math.sqrt(
        math.fsum(
            (float(left[index]) - float(right[index])) ** 2
            for index in range(count)
        )
        / count
    )
    reference_rms = max(_rms(left, count), _rms(right, count))
    return difference_rms / reference_rms if reference_rms > 0.0 else math.inf


def _centroid(waveforms: list[list[int]]) -> list[float]:
    if not waveforms:
        return []
    count = min(len(waveform) for waveform in waveforms)
    scale = 1.0 / len(waveforms)
    return [
        math.fsum(float(waveform[index]) for waveform in waveforms) * scale
        for index in range(count)
    ]


def build_heartbeat_report(
    records: list[Mapping[str, Any]],
    *,
    expected_repeats: int = 3,
    expected_perturbations: int = 3,
    maximum_repeat_normalized_rms: float = 0.005,
    minimum_effect_normalized_rms: float = 0.01,
    minimum_effect_to_repeat_ratio: float = 10.0,
) -> dict[str, Any]:
    if expected_repeats < 1 or expected_perturbations < 2:
        raise ValueError("invalid heartbeat group cardinality")
    if (
        maximum_repeat_normalized_rms < 0.0
        or minimum_effect_normalized_rms <= 0.0
        or minimum_effect_to_repeat_ratio <= 0.0
    ):
        raise ValueError("invalid heartbeat residual threshold")

    repeats: dict[tuple[Any, ...], list[Mapping[str, Any]]] = defaultdict(list)
    perturbations: dict[
        tuple[Any, ...], dict[float, list[Mapping[str, Any]]]
    ] = defaultdict(
        lambda: defaultdict(list)
    )
    for record in records:
        case = record["case"]
        repeat_key = (
            case["midi_note"],
            case["midi_velocity"],
            case["control_name"],
            case["perturbation"],
        )
        repeats[repeat_key].append(record)
        sensitivity_key = (
            case["midi_note"],
            case["midi_velocity"],
            case["control_name"],
        )
        perturbations[sensitivity_key][float(case["perturbation"])].append(record)

    sample_cache: dict[str, list[int]] = {}

    def samples_for(record: Mapping[str, Any]) -> list[int]:
        path = str(record["output_path"])
        if path not in sample_cache:
            sample_cache[path] = _read_pcm_samples(pathlib.Path(path))
        return sample_cache[path]

    repeat_details = []
    for key, group in sorted(repeats.items()):
        hashes = sorted({item["validation"]["pcm_sha256"] for item in group})
        pairwise = [
            _normalized_rms_difference(samples_for(left), samples_for(right))
            for left, right in itertools.combinations(group, 2)
        ]
        maximum_residual = max(pairwise, default=0.0)
        repeat_details.append(
            {
                "midi_note": key[0],
                "midi_velocity": key[1],
                "control_name": key[2],
                "perturbation": key[3],
                "repeat_count": len(group),
                "distinct_pcm_sha256_count": len(hashes),
                "pcm_sha256": hashes,
                "pcm_identical": len(hashes) == 1,
                "pairwise_normalized_rms": pairwise,
                "maximum_pairwise_normalized_rms": maximum_residual,
                "within_tolerance": (
                    len(group) == expected_repeats
                    and maximum_residual <= maximum_repeat_normalized_rms
                ),
            }
        )

    sensitivity_details = []
    for key, by_delta in sorted(perturbations.items()):
        representative_hashes = {
            str(delta): sorted(
                {
                    item["validation"]["pcm_sha256"]
                    for item in group
                }
            )
            for delta, group in sorted(by_delta.items())
        }
        centroids = {
            delta: _centroid([samples_for(item) for item in group])
            for delta, group in by_delta.items()
        }
        effect_pairs = [
            {
                "left_perturbation": left,
                "right_perturbation": right,
                "normalized_rms": _normalized_rms_difference(
                    centroids[left], centroids[right]
                ),
            }
            for left, right in itertools.combinations(sorted(centroids), 2)
        ]
        minimum_effect = min(
            (item["normalized_rms"] for item in effect_pairs), default=0.0
        )
        matching_repeats = [
            item
            for item in repeat_details
            if (
                item["midi_note"],
                item["midi_velocity"],
                item["control_name"],
            )
            == key
        ]
        local_repeat_floor = max(
            (
                item["maximum_pairwise_normalized_rms"]
                for item in matching_repeats
            ),
            default=0.0,
        )
        effect_to_repeat_ratio = (
            minimum_effect / local_repeat_floor
            if local_repeat_floor > 0.0
            else None
        )
        complete = (
            len(by_delta) == expected_perturbations
            and all(len(group) == expected_repeats for group in by_delta.values())
        )
        effect_above_noise_floor = (
            complete
            and minimum_effect >= minimum_effect_normalized_rms
            and (
                effect_to_repeat_ratio is None
                or effect_to_repeat_ratio >= minimum_effect_to_repeat_ratio
            )
        )
        sensitivity_details.append(
            {
                "midi_note": key[0],
                "midi_velocity": key[1],
                "control_name": key[2],
                "delta_pcm_sha256": representative_hashes,
                "effect_pairwise_normalized_rms": effect_pairs,
                "minimum_effect_normalized_rms": minimum_effect,
                "maximum_repeat_normalized_rms": local_repeat_floor,
                "effect_to_repeat_ratio": effect_to_repeat_ratio,
                "complete": complete,
                "effect_above_noise_floor": effect_above_noise_floor,
            }
        )

    all_repeats_within_tolerance = bool(repeat_details) and all(
        item["within_tolerance"] for item in repeat_details
    )
    all_effects_above_noise_floor = bool(sensitivity_details) and all(
        item["effect_above_noise_floor"] for item in sensitivity_details
    )
    return {
        "schema_version": 1,
        "case_count": len(records),
        "repeat_group_count": len(repeat_details),
        "sensitivity_group_count": len(sensitivity_details),
        "thresholds": {
            "expected_repeats": expected_repeats,
            "expected_perturbations": expected_perturbations,
            "maximum_repeat_normalized_rms": maximum_repeat_normalized_rms,
            "minimum_effect_normalized_rms": minimum_effect_normalized_rms,
            "minimum_effect_to_repeat_ratio": minimum_effect_to_repeat_ratio,
        },
        "all_repeats_within_tolerance": all_repeats_within_tolerance,
        "all_hardness_effects_above_noise_floor": all_effects_above_noise_floor,
        "pass": (
            all_repeats_within_tolerance and all_effects_above_noise_floor
        ),
        "repeatability": repeat_details,
        "sensitivity": sensitivity_details,
    }


def run_study(
    study_path: pathlib.Path,
    *,
    output_override: pathlib.Path | None = None,
    force: bool = False,
    limit: int | None = None,
    dry_run: bool = False,
) -> int:
    study_path = study_path.resolve()
    study = load_study(study_path)
    project = pathlib.Path(__file__).resolve().parents[1]
    if output_override is not None:
        output = output_override.resolve()
    elif dry_run:
        output = (project / "runs" / "dry-run" / str(study["study_id"])).resolve()
    else:
        configured_output = pathlib.Path(
            os.path.expanduser(os.path.expandvars(str(study["output_directory"])))
        )
        output = (
            configured_output
            if configured_output.is_absolute()
            else project / configured_output
        ).resolve()
    output.mkdir(parents=True, exist_ok=True)
    engine = PianoteqEngine.from_mapping(study["engine"])
    all_cases = expand_heartbeat_cases(study)
    cases = all_cases
    if limit is not None:
        if limit < 1:
            raise ValueError("--limit must be positive")
        cases = cases[:limit]
    partial_run = len(cases) != len(all_cases)
    render_options = _render_options(study)
    pianoteq_version = "not-queried" if dry_run else engine.query_version()
    executable_sha256 = (
        file_sha256(engine.executable_native_path)
        if engine.executable_native_path is not None
        else None
    )
    study_hash = json_sha256(study)
    resolved_study = {
        "source": str(study_path),
        "study_sha256": study_hash,
        "configuration": study,
        "expanded_case_count": len(cases),
    }
    write_json_atomic(output / "study.resolved.json", resolved_study)

    manifest: dict[str, Any] = {
        "schema_version": 1,
        "study_id": study["study_id"],
        "study_sha256": study_hash,
        "created_at_utc": dt.datetime.now(dt.timezone.utc).isoformat(),
        "source_study": str(study_path),
        "output_directory": str(output),
        "expected_case_count": len(cases),
        "full_study_case_count": len(all_cases),
        "partial_run": partial_run,
        "pianoteq_version": pianoteq_version,
        "pianoteq_executable_sha256": executable_sha256,
        "completed_case_count": 0,
        "rendered_case_count": 0,
        "resumed_case_count": 0,
        "failed_case_count": 0,
        "all_valid": False,
        "cases": [],
    }
    records: list[dict[str, Any]] = []
    print(f"study={study['study_id']} cases={len(cases)} output={output}")

    for index, case in enumerate(cases, start=1):
        case_id = case["case_id"]
        midi_path = output / "midi" / (
            f"n{case['midi_note']:03d}_v{case['midi_velocity']:03d}.mid"
        )
        audio_path = output / "audio" / f"{case_id}.wav"
        record_path = output / "cases" / case_id / "render.json"
        _make_probe(study, case, midi_path)
        signature = _case_signature(study, case, midi_path)
        print(f"[{index:02d}/{len(cases):02d}] {case_id}", flush=True)

        if dry_run:
            manifest["cases"].append(
                {
                    "case_id": case_id,
                    "case_signature": signature,
                    "status": "dry-run",
                }
            )
            continue

        resumed = None if force else _resume_record(
            record_path,
            audio_path,
            signature,
            render_options,
            pianoteq_version=pianoteq_version,
            executable_sha256=executable_sha256,
        )
        if resumed is not None:
            resumed["case"] = case
            resumed["case_signature"] = signature
            resumed["status"] = "resumed"
            records.append(resumed)
            manifest["resumed_case_count"] += 1
            print("  resume: valid cached render", flush=True)
        else:
            try:
                record = render_wav(
                    engine,
                    preset=str(study["preset"]),
                    midi_path=midi_path,
                    output_path=audio_path,
                    parameters=case["parameters"],
                    verified_version=pianoteq_version,
                    **render_options,
                )
                record["case"] = case
                record["case_signature"] = signature
                record["status"] = "rendered"
                write_json_atomic(record_path, record)
                records.append(record)
                manifest["rendered_case_count"] += 1
                print(
                    "  render: "
                    f"peak={record['validation']['peak_normalized']:.6f} "
                    f"sha={record['validation']['pcm_sha256'][:12]}",
                    flush=True,
                )
            except (OSError, ValueError, PianoteqRenderError) as error:
                failure = {
                    "schema_version": 1,
                    "case": case,
                    "case_signature": signature,
                    "status": "failed",
                    "error": str(error),
                    "failed_at_utc": dt.datetime.now(dt.timezone.utc).isoformat(),
                }
                write_json_atomic(
                    output / "cases" / case_id / "failure.json", failure
                )
                manifest["failed_case_count"] += 1
                manifest["cases"].append(failure)
                write_json_atomic(output / "manifest.json", manifest)
                raise

        manifest["completed_case_count"] = len(records)
        manifest["cases"] = [
            {
                "case_id": item["case"]["case_id"],
                "case_signature": item["case_signature"],
                "status": item["status"],
                "record": str(
                    output / "cases" / item["case"]["case_id"] / "render.json"
                ),
                "audio": item["output_path"],
                "file_sha256": item["validation"]["file_sha256"],
                "pcm_sha256": item["validation"]["pcm_sha256"],
                "valid": item["validation"]["valid"],
            }
            for item in records
        ]
        manifest["all_valid"] = (
            len(records) == len(cases)
            and all(item["validation"]["valid"] for item in records)
        )
        write_json_atomic(output / "manifest.json", manifest)

    if dry_run:
        manifest["all_valid"] = False
        write_json_atomic(output / "manifest.json", manifest)
        print("PASS: dry-run expansion completed")
        return 0

    validation = study["heartbeat"].get("validation", {})
    report = build_heartbeat_report(
        records,
        expected_repeats=int(study["heartbeat"]["repeats"]),
        expected_perturbations=3,
        maximum_repeat_normalized_rms=float(
            validation.get("maximum_repeat_normalized_rms", 0.005)
        ),
        minimum_effect_normalized_rms=float(
            validation.get("minimum_effect_normalized_rms", 0.01)
        ),
        minimum_effect_to_repeat_ratio=float(
            validation.get("minimum_effect_to_repeat_ratio", 10.0)
        ),
    )
    report["study_id"] = study["study_id"]
    report["study_sha256"] = study_hash
    write_json_atomic(output / "heartbeat_report.json", report)
    manifest["heartbeat_pass"] = None if partial_run else report["pass"]
    manifest["pipeline_pass"] = manifest["all_valid"] and (
        partial_run or report["pass"]
    )
    manifest["finished_at_utc"] = dt.datetime.now(dt.timezone.utc).isoformat()
    write_json_atomic(output / "manifest.json", manifest)
    if not manifest["pipeline_pass"]:
        print(
            "FAIL: renders completed but heartbeat repeatability/sensitivity gate failed",
            file=sys.stderr,
        )
        return 2
    if partial_run:
        print(
            "PASS: partial render/validation "
            f"cases={len(records)}/{len(all_cases)} "
            "(heartbeat gate intentionally not evaluated)"
        )
        return 0
    print(
        "PASS: heartbeat "
        f"cases={len(records)} rendered={manifest['rendered_case_count']} "
        f"resumed={manifest['resumed_case_count']}"
    )
    return 0


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("study", type=pathlib.Path)
    parser.add_argument("--output", type=pathlib.Path)
    parser.add_argument("--force", action="store_true")
    parser.add_argument("--limit", type=int)
    parser.add_argument("--dry-run", action="store_true")
    args = parser.parse_args()
    try:
        return run_study(
            args.study,
            output_override=args.output,
            force=args.force,
            limit=args.limit,
            dry_run=args.dry_run,
        )
    except (OSError, ValueError, json.JSONDecodeError, PianoteqRenderError) as error:
        print(f"run_study: {error}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
