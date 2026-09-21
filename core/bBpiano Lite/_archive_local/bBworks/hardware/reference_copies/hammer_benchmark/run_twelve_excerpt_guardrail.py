#!/usr/bin/env python3

from __future__ import annotations

import json
import os
import re
import shutil
import subprocess
from datetime import datetime, timezone
from pathlib import Path

import numpy as np
import soundfile as sf


ROOT = Path(__file__).resolve().parents[1]
RENDERER = ROOT / "hammer_benchmark" / "build" / "render_hammer_benchmark"
MANIFEST = ROOT / "hammer_benchmark" / "guardrail_manifest.json"
MIDI_ROOT = (
    ROOT
    / "Cli"
    / "bBpiano Sonic"
    / "data"
    / "midi_samples"
    / "Fancy's selection"
    / "midi"
)
REFERENCE_ROOT = (
    ROOT
    / "Cli"
    / "bBpiano Sonic"
    / "data"
    / "wav_snippet"
    / "Yamaha Disklaviers - MAESTRO"
)

BASELINE = {
    "BBPIANO_HAMMER_MODEL": "legacy",
    "BBPIANO_HAMMER_COUPLING": "shared",
}
WINNER = {
    "BBPIANO_HAMMER_MODEL": "maxwell1",
    "BBPIANO_HAMMER_COUPLING": "per-string",
    "BBPIANO_HAMMER_K_SCALE": "1.2",
    "BBPIANO_HAMMER_P_OFFSET": "-0.2",
    "BBPIANO_HAMMER_EPSILON": "0.9",
    "BBPIANO_HAMMER_TAU_US": "200",
}


def run(args: list[str], env: dict[str, str] | None = None):
    merged = os.environ.copy()
    if env:
        merged.update(env)
    return subprocess.run(
        args,
        cwd=ROOT,
        env=merged,
        check=True,
        text=True,
        capture_output=True,
    )


def find_source(root: Path, stem: str, suffixes: tuple[str, ...]) -> Path:
    for suffix in suffixes:
        candidate = root / f"{stem}{suffix}"
        if candidate.exists():
            return candidate
    raise FileNotFoundError(stem)


def write_reference_excerpt(
    source: Path,
    output: Path,
    start_seconds: float,
    end_seconds: float,
) -> None:
    audio, sample_rate = sf.read(source, always_2d=True)
    start = int(round(start_seconds * sample_rate))
    end = int(round(end_seconds * sample_rate))
    sf.write(output, audio[start:end], sample_rate, subtype="PCM_16")


def visqol(reference: Path, test: Path) -> float | None:
    executable = shutil.which("pyvisqol")
    if not executable:
        return None
    environment = os.environ.copy()
    environment.setdefault(
        "NUMBA_CACHE_DIR",
        "/tmp/bbpiano-numba",
    )
    result = subprocess.run(
        [executable, str(reference), str(test)],
        cwd=ROOT,
        env=environment,
        capture_output=True,
        text=True,
    )
    if result.returncode != 0:
        return None
    matches = re.findall(
        r"(?m)^\s*([-+]?\d*\.?\d+(?:[eE][-+]?\d+)?)\s*$",
        result.stdout + "\n" + result.stderr,
    )
    return float(matches[-1]) if matches else None


def main() -> None:
    if not RENDERER.exists():
        run([str(ROOT / "hammer_benchmark" / "build.sh")])

    run_id = datetime.now(timezone.utc).strftime("%Y%m%dT%H%M%SZ")
    output = (
        ROOT
        / "hammer_benchmark"
        / "outputs"
        / f"{run_id}_twelve_excerpt_guardrail"
    )
    reference_output = output / "reference"
    baseline_output = output / "baseline"
    winner_output = output / "winner"
    for directory in (reference_output, baseline_output, winner_output):
        directory.mkdir(parents=True)

    rows = []
    entries = json.loads(MANIFEST.read_text())
    for entry in entries:
        stem = entry["stem"]
        start = float(entry["start_seconds"])
        end = float(entry["end_seconds"])
        duration = end - start
        midi = find_source(MIDI_ROOT, stem, (".midi", ".mid"))
        reference = find_source(REFERENCE_ROOT, stem, (".wav",))
        file_name = f"{stem}.wav"
        reference_excerpt = reference_output / file_name
        baseline_excerpt = baseline_output / file_name
        winner_excerpt = winner_output / file_name

        write_reference_excerpt(reference, reference_excerpt, start, end)
        run(
            [
                str(RENDERER),
                "--midi",
                str(midi),
                str(baseline_excerpt),
                str(start),
                str(duration),
            ],
            BASELINE,
        )
        run(
            [
                str(RENDERER),
                "--midi",
                str(midi),
                str(winner_excerpt),
                str(start),
                str(duration),
            ],
            WINNER,
        )
        baseline_score = visqol(reference_excerpt, baseline_excerpt)
        winner_score = visqol(reference_excerpt, winner_excerpt)
        rows.append(
            {
                "stem": stem,
                "start_seconds": start,
                "end_seconds": end,
                "baseline_visqol": baseline_score,
                "winner_visqol": winner_score,
                "delta": (
                    winner_score - baseline_score
                    if baseline_score is not None
                    and winner_score is not None
                    else None
                ),
            }
        )
        print(
            stem,
            f"baseline={baseline_score}",
            f"winner={winner_score}",
        )

    valid = [
        row
        for row in rows
        if row["baseline_visqol"] is not None
        and row["winner_visqol"] is not None
    ]
    summary = {
        "baseline_configuration": BASELINE,
        "winner_configuration": WINNER,
        "pieces": rows,
        "mean_baseline_visqol": (
            float(np.mean([row["baseline_visqol"] for row in valid]))
            if valid
            else None
        ),
        "mean_winner_visqol": (
            float(np.mean([row["winner_visqol"] for row in valid]))
            if valid
            else None
        ),
        "mean_delta": (
            float(np.mean([row["delta"] for row in valid]))
            if valid
            else None
        ),
    }
    (output / "summary.json").write_text(
        json.dumps(summary, indent=2, ensure_ascii=False) + "\n"
    )
    print(f"Output: {output}")


if __name__ == "__main__":
    main()
