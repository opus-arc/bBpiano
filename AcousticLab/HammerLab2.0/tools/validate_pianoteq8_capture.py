#!/usr/bin/env python3
"""Reject incomplete or accidentally mixed Pianoteq reference datasets."""

import argparse
import csv
import json
import pathlib
import wave


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("manifest", type=pathlib.Path)
    parser.add_argument("audio_directory", type=pathlib.Path)
    args = parser.parse_args()
    manifest = json.loads(args.manifest.read_text())
    errors = []
    if manifest.get("reference_engine") != "Pianoteq 8":
        errors.append("reference_engine must be exactly 'Pianoteq 8'")
    exact_version = str(manifest.get("pianoteq_version_exact", ""))
    if not exact_version.startswith("8.") or "x" in exact_version.lower():
        errors.append("pianoteq_version_exact must be a concrete 8.x.x version")
    for key, value in manifest.items():
        if isinstance(value, str) and "FILL_ME" in value:
            errors.append(f"manifest field is incomplete: {key}")

    project = pathlib.Path(__file__).resolve().parents[1]
    matrix_path = pathlib.Path(manifest.get("probe_matrix", ""))
    if not matrix_path.is_absolute():
        matrix_path = project / matrix_path
    try:
        with matrix_path.open(newline="", encoding="utf-8") as stream:
            rows = list(csv.DictReader(stream))
    except OSError as error:
        errors.append(f"cannot read probe matrix: {error}")
        rows = []

    expected_rate = int(manifest.get("sample_rate_hz", 0))
    expected_channels = int(manifest.get("channels", 0))
    for row in rows:
        path = args.audio_directory / f"{row['case_id']}.wav"
        if not path.is_file():
            errors.append(f"missing capture: {path.name}")
            continue
        try:
            with wave.open(str(path), "rb") as stream:
                if stream.getframerate() != expected_rate:
                    errors.append(f"{path.name}: sample rate mismatch")
                if stream.getnchannels() != expected_channels:
                    errors.append(f"{path.name}: channel count mismatch")
                if stream.getnframes() == 0:
                    errors.append(f"{path.name}: empty WAV")
        except (wave.Error, EOFError) as error:
            errors.append(f"{path.name}: invalid PCM WAV ({error})")

    if errors:
        print("Pianoteq 8 capture validation FAILED")
        for error in errors:
            print(f"- {error}")
        return 1
    print(f"Pianoteq 8 capture validation PASS: {len(rows)} WAV files")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
