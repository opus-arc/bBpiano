#!/usr/bin/env python3

from __future__ import annotations

import csv
import json
import math
import os
import re
import shutil
import subprocess
from dataclasses import dataclass
from datetime import datetime, timezone
from pathlib import Path

import librosa
import numpy as np
import soundfile as sf


ROOT = Path(__file__).resolve().parents[1]
BUILD = ROOT / "hammer_benchmark" / "build"
OUTPUT_ROOT = ROOT / "hammer_benchmark" / "outputs"
REFERENCE_ROOT = (
    ROOT
    / "AcousticLab"
    / "StringFilterLab"
    / "Samples"
    / "Pianoteq 9"
    / "SingleNoteSamples"
    / "Old Version"
    / "3"
    / "v80"
)
MUSICAL_MIDI = (
    ROOT
    / "Cli"
    / "bBpiano Sonic"
    / "data"
    / "midi_samples"
    / "Fancy's selection"
    / "midi"
    / "Sonata in D Minor, K. 141_2018.midi"
)
SAMPLE_RATE = 44_100


@dataclass(frozen=True)
class Candidate:
    name: str
    model: str
    coupling: str
    k_scale: float = 1.0
    p_offset: float = 0.0
    epsilon: float = 0.22
    tau_us: float = 200.0

    def environment(self) -> dict[str, str]:
        return {
            "BBPIANO_HAMMER_MODEL": self.model,
            "BBPIANO_HAMMER_COUPLING": self.coupling,
            "BBPIANO_HAMMER_K_SCALE": str(self.k_scale),
            "BBPIANO_HAMMER_P_OFFSET": str(self.p_offset),
            "BBPIANO_HAMMER_EPSILON": str(self.epsilon),
            "BBPIANO_HAMMER_TAU_US": str(self.tau_us),
        }


CANDIDATES = [
    Candidate("legacy_shared", "legacy", "shared"),
    Candidate("legacy_per_string", "legacy", "per-string"),
    Candidate("power_per_string", "power", "per-string"),
    Candidate("rt425_per_string", "rt425", "per-string"),
    Candidate(
        "maxwell_shared",
        "maxwell1",
        "shared",
        k_scale=2.0,
        epsilon=0.65,
        tau_us=400.0,
    ),
    Candidate(
        "maxwell_per_string_soft",
        "maxwell1",
        "per-string",
        k_scale=1.0,
        p_offset=-0.2,
        epsilon=0.8,
        tau_us=200.0,
    ),
    Candidate(
        "maxwell_per_string_fast",
        "maxwell1",
        "per-string",
        k_scale=1.5,
        p_offset=-0.2,
        epsilon=0.8,
        tau_us=100.0,
    ),
    Candidate(
        "maxwell_per_string_deep",
        "maxwell1",
        "per-string",
        k_scale=1.2,
        p_offset=-0.2,
        epsilon=0.9,
        tau_us=200.0,
    ),
]


def run(
    args: list[str],
    *,
    env: dict[str, str] | None = None,
    check: bool = True,
) -> subprocess.CompletedProcess[str]:
    merged = os.environ.copy()
    if env:
        merged.update(env)
    return subprocess.run(
        args,
        cwd=ROOT,
        env=merged,
        check=check,
        text=True,
        capture_output=True,
    )


def ensure_build() -> None:
    renderer = BUILD / "render_hammer_benchmark"
    coupling = BUILD / "hammer_string_coupling_test"
    if renderer.exists() and coupling.exists():
        return
    result = run([str(ROOT / "hammer_benchmark" / "build.sh")])
    print(result.stdout, end="")


def read_csv(path: Path) -> list[dict[str, str]]:
    with path.open(newline="") as file:
        return list(csv.DictReader(file))


def physical_summary(rows: list[dict[str, str]]) -> dict[str, float | bool]:
    energy = [float(row["maximum_total_energy_ratio"]) for row in rows]
    bridge = [float(row["first_pass_bridge_energy_ratio"]) for row in rows]
    release = [float(row["string_energy_at_release_ratio"]) for row in rows]
    contact = [float(row["first_contact_ms"]) for row in rows]
    spread = [
        float(row["force_weighted_string_force_spread"])
        for row in rows
    ]
    finite = all(int(row["finite"]) == 1 for row in rows)

    brightening_count = 0
    for midi in (24, 36, 48, 60, 72, 84, 96, 108):
        soft = next(
            row
            for row in rows
            if int(row["midi"]) == midi
            and float(row["impact_velocity_m_s"]) == 1.0
        )
        loud = next(
            row
            for row in rows
            if int(row["midi"]) == midi
            and float(row["impact_velocity_m_s"]) == 7.0
        )
        if float(loud["force_centroid_hz"]) > float(
            soft["force_centroid_hz"]
        ):
            brightening_count += 1

    max_energy = max(energy)
    return {
        "finite": finite,
        "mechanical_pass": finite and max_energy < 1.05,
        "maximum_observable_energy_ratio": max_energy,
        "mean_first_pass_bridge_energy_ratio": float(np.mean(bridge)),
        "mean_release_string_energy_ratio": float(np.mean(release)),
        "median_contact_ms": float(np.median(contact)),
        "mean_force_weighted_string_force_spread": float(np.mean(spread)),
        "brightening_note_count": brightening_count,
    }


def load_audio(path: Path) -> np.ndarray:
    audio, sample_rate = sf.read(path, always_2d=True)
    mono = np.mean(audio, axis=1).astype(np.float64)
    if sample_rate != SAMPLE_RATE:
        mono = librosa.resample(
            mono,
            orig_sr=sample_rate,
            target_sr=SAMPLE_RATE,
        )
    return mono


def onset_index(audio: np.ndarray) -> int:
    if not np.any(audio):
        return 0
    frame = 256
    hop = 64
    rms = librosa.feature.rms(
        y=audio.astype(np.float32),
        frame_length=frame,
        hop_length=hop,
        center=False,
    )[0]
    threshold = max(float(np.max(rms)) * 0.02, 1.0e-7)
    hits = np.flatnonzero(rms >= threshold)
    return int(hits[0] * hop) if hits.size else 0


def aligned_attack(audio: np.ndarray, seconds: float = 0.6) -> np.ndarray:
    start = onset_index(audio)
    length = int(seconds * SAMPLE_RATE)
    attack = audio[start : start + length]
    if attack.size < length:
        attack = np.pad(attack, (0, length - attack.size))
    rms = math.sqrt(float(np.mean(attack * attack)) + 1.0e-16)
    return attack / rms


def relative_band_distribution(
    audio: np.ndarray,
    fundamental_hz: float,
) -> np.ndarray:
    length = min(audio.size, int(0.35 * SAMPLE_RATE))
    windowed = audio[:length] * np.hanning(length)
    power = np.abs(np.fft.rfft(windowed)) ** 2
    frequency = np.fft.rfftfreq(length, 1.0 / SAMPLE_RATE)
    edges = np.array([0.5, 2, 4, 8, 16, 32, 64, 128]) * fundamental_hz
    values = []
    for low, high in zip(edges[:-1], edges[1:]):
        mask = (frequency >= low) & (
            frequency < min(high, SAMPLE_RATE * 0.5)
        )
        values.append(float(np.sum(power[mask])))
    values = np.asarray(values, dtype=np.float64) + 1.0e-18
    return values / np.sum(values)


def attack_metrics(
    reference_path: Path,
    test_path: Path,
    fundamental_hz: float,
) -> dict[str, float]:
    reference = aligned_attack(load_audio(reference_path))
    test = aligned_attack(load_audio(test_path))

    n_fft = 2048
    hop = 256
    reference_spectrum = np.abs(
        librosa.stft(reference, n_fft=n_fft, hop_length=hop)
    )
    test_spectrum = np.abs(
        librosa.stft(test, n_fft=n_fft, hop_length=hop)
    )
    reference_db = librosa.amplitude_to_db(
        reference_spectrum,
        ref=np.max,
        top_db=80.0,
    )
    test_db = librosa.amplitude_to_db(
        test_spectrum,
        ref=np.max,
        top_db=80.0,
    )
    log_spectral_distance = float(
        np.sqrt(np.mean((reference_db - test_db) ** 2))
    )

    reference_rms = librosa.feature.rms(
        y=reference.astype(np.float32),
        frame_length=512,
        hop_length=128,
    )[0]
    test_rms = librosa.feature.rms(
        y=test.astype(np.float32),
        frame_length=512,
        hop_length=128,
    )[0]
    reference_envelope_db = librosa.amplitude_to_db(
        reference_rms,
        ref=np.max,
        top_db=60.0,
    )
    test_envelope_db = librosa.amplitude_to_db(
        test_rms,
        ref=np.max,
        top_db=60.0,
    )
    envelope_rmse = float(
        np.sqrt(np.mean((reference_envelope_db - test_envelope_db) ** 2))
    )

    reference_centroid = float(
        np.mean(
            librosa.feature.spectral_centroid(
                y=reference.astype(np.float32),
                sr=SAMPLE_RATE,
                n_fft=n_fft,
                hop_length=hop,
            )
        )
    )
    test_centroid = float(
        np.mean(
            librosa.feature.spectral_centroid(
                y=test.astype(np.float32),
                sr=SAMPLE_RATE,
                n_fft=n_fft,
                hop_length=hop,
            )
        )
    )
    centroid_log_error = abs(
        math.log2((test_centroid + 1.0) / (reference_centroid + 1.0))
    )

    reference_bands = relative_band_distribution(
        reference,
        fundamental_hz,
    )
    test_bands = relative_band_distribution(test, fundamental_hz)
    band_l1 = float(np.sum(np.abs(reference_bands - test_bands)))

    composite = (
        0.45 * min(log_spectral_distance / 35.0, 2.0)
        + 0.25 * min(envelope_rmse / 30.0, 2.0)
        + 0.20 * min(band_l1, 2.0)
        + 0.10 * min(centroid_log_error / 2.0, 2.0)
    )
    return {
        "log_spectral_distance_db": log_spectral_distance,
        "attack_envelope_rmse_db": envelope_rmse,
        "centroid_log2_error": centroid_log_error,
        "relative_band_l1": band_l1,
        "attack_proxy_loss": composite,
    }


def reference_for(note_name: str) -> Path:
    octave = note_name.split("_", 1)[0]
    matches = sorted(REFERENCE_ROOT.glob(f"{octave}_take*_v80.wav"))
    if not matches:
        raise FileNotFoundError(f"No Pianoteq reference for {octave}")
    return matches[0]


def render_single_notes(candidate: Candidate, output_dir: Path) -> None:
    env = candidate.environment()
    result = run(
        [
            str(BUILD / "render_hammer_benchmark"),
            "--single-notes",
            str(output_dir),
            "80",
            "4.0",
        ],
        env=env,
    )
    print(result.stdout, end="")


def run_visqol(reference: Path, test: Path) -> float | None:
    executable = shutil.which("pyvisqol")
    if not executable:
        return None
    result = run(
        [executable, str(reference), str(test)],
        check=False,
    )
    if result.returncode != 0:
        return None
    matches = re.findall(
        r"(?m)^\s*([-+]?\d*\.?\d+(?:[eE][-+]?\d+)?)\s*$",
        result.stdout + "\n" + result.stderr,
    )
    return float(matches[-1]) if matches else None


def write_dict_csv(path: Path, rows: list[dict[str, object]]) -> None:
    if not rows:
        return
    with path.open("w", newline="") as file:
        writer = csv.DictWriter(file, fieldnames=list(rows[0]))
        writer.writeheader()
        writer.writerows(rows)


def make_audition(
    source_dir: Path,
    output_path: Path,
) -> None:
    segments = []
    gap = np.zeros(int(0.35 * SAMPLE_RATE), dtype=np.float64)
    for note_path in sorted(source_dir.glob("A*_2s.wav")):
        audio = load_audio(note_path)
        segments.append(audio[: int(2.4 * SAMPLE_RATE)])
        segments.append(gap)
    sf.write(
        output_path,
        np.concatenate(segments).astype(np.float32),
        SAMPLE_RATE,
        subtype="FLOAT",
    )


def write_loudness_matched(source: Path, output: Path) -> None:
    audio = load_audio(source)
    rms = math.sqrt(float(np.mean(audio * audio)) + 1.0e-16)
    target_rms = 0.1
    scaled = audio * (target_rms / rms)
    peak = float(np.max(np.abs(scaled)))
    if peak > 0.98:
        scaled *= 0.98 / peak
    sf.write(
        output,
        scaled.astype(np.float32),
        SAMPLE_RATE,
        subtype="FLOAT",
    )


def main() -> None:
    ensure_build()
    run_id = datetime.now(timezone.utc).strftime("%Y%m%dT%H%M%SZ")
    output = OUTPUT_ROOT / run_id
    output.mkdir(parents=True)

    physical_rows: list[dict[str, object]] = []
    physical_by_name: dict[str, dict[str, float | bool]] = {}
    audio_rows: list[dict[str, object]] = []

    for candidate in CANDIDATES:
        candidate_dir = output / candidate.name
        candidate_dir.mkdir()
        physical_csv = candidate_dir / "physical.csv"
        env = candidate.environment() | {
            "BBPIANO_HAMMER_BENCHMARK_CSV": str(physical_csv)
        }
        result = run(
            [str(BUILD / "hammer_string_coupling_test")],
            env=env,
            check=False,
        )
        (candidate_dir / "physical_stdout.txt").write_text(
            result.stdout + result.stderr
        )
        summary = physical_summary(read_csv(physical_csv))
        physical_by_name[candidate.name] = summary
        physical_rows.append(
            {
                "candidate": candidate.name,
                **candidate.environment(),
                **summary,
            }
        )

        notes_dir = candidate_dir / "single_notes"
        render_single_notes(candidate, notes_dir)
        for note_path in sorted(notes_dir.glob("A*_2s.wav")):
            note_name = note_path.name
            octave = int(note_name[1])
            fundamental = 27.5 * (2.0 ** octave)
            reference_path = reference_for(note_name)
            metrics = attack_metrics(
                reference_path,
                note_path,
                fundamental,
            )
            audio_rows.append(
                {
                    "candidate": candidate.name,
                    "note": note_name,
                    "reference": str(reference_path),
                    **metrics,
                }
            )

    aggregate_audio: dict[str, dict[str, float]] = {}
    for candidate in CANDIDATES:
        rows = [
            row
            for row in audio_rows
            if row["candidate"] == candidate.name
        ]
        aggregate_audio[candidate.name] = {
            key: float(np.mean([float(row[key]) for row in rows]))
            for key in (
                "log_spectral_distance_db",
                "attack_envelope_rmse_db",
                "centroid_log2_error",
                "relative_band_l1",
                "attack_proxy_loss",
            )
        }

    mechanically_valid = [
        candidate
        for candidate in CANDIDATES
        if bool(physical_by_name[candidate.name]["mechanical_pass"])
    ]
    ranked = sorted(
        mechanically_valid,
        key=lambda candidate: aggregate_audio[candidate.name][
            "attack_proxy_loss"
        ],
    )
    if not ranked:
        raise RuntimeError("No candidate passed the mechanical gate.")

    for candidate in ranked[:3]:
        rows = [
            row
            for row in audio_rows
            if row["candidate"] == candidate.name
        ]
        scores = []
        for row in rows:
            score = run_visqol(
                Path(str(row["reference"])),
                output
                / candidate.name
                / "single_notes"
                / str(row["note"]),
            )
            if score is not None:
                scores.append(score)
        aggregate_audio[candidate.name]["mean_visqol"] = (
            float(np.mean(scores)) if scores else math.nan
        )

    ranking_rows: list[dict[str, object]] = []
    for rank, candidate in enumerate(ranked, start=1):
        ranking_rows.append(
            {
                "rank": rank,
                "candidate": candidate.name,
                **aggregate_audio[candidate.name],
                **physical_by_name[candidate.name],
            }
        )

    winner = ranked[0]
    baseline = next(
        candidate
        for candidate in CANDIDATES
        if candidate.name == "legacy_shared"
    )

    make_audition(
        output / baseline.name / "single_notes",
        output / "audition_baseline.wav",
    )
    make_audition(
        output / winner.name / "single_notes",
        output / "audition_winner.wav",
    )

    for candidate, label in ((baseline, "baseline"), (winner, "winner")):
        result = run(
            [
                str(BUILD / "render_hammer_benchmark"),
                "--midi",
                str(MUSICAL_MIDI),
                str(output / f"scarlatti_30s_{label}.wav"),
                "30.0",
            ],
            env=candidate.environment(),
        )
        print(result.stdout, end="")

    for stem in (
        "audition_baseline",
        "audition_winner",
        "scarlatti_30s_baseline",
        "scarlatti_30s_winner",
    ):
        write_loudness_matched(
            output / f"{stem}.wav",
            output / f"{stem}_loudness_matched.wav",
        )

    write_dict_csv(output / "physical_summary.csv", physical_rows)
    write_dict_csv(output / "per_note_audio_metrics.csv", audio_rows)
    write_dict_csv(output / "ranking.csv", ranking_rows)
    (output / "winner.json").write_text(
        json.dumps(
            {
                "winner": winner.name,
                "configuration": winner.environment(),
                "physical": physical_by_name[winner.name],
                "audio": aggregate_audio[winner.name],
            },
            indent=2,
        )
        + "\n"
    )

    report = f"""# Hammer benchmark {run_id}

## Decision

Winner: `{winner.name}`

The winner passed the mechanical gate and had the lowest isolated-attack proxy
loss among passing candidates. The attack score is a relative engineering
ranking against Pianoteq v80 single notes, not proof of real-hammer identity.

## Winner configuration

```json
{json.dumps(winner.environment(), indent=2)}
```

## Benchmark contract

- Hard gate: finite output and maximum observable hammer-plus-string energy
  ratio below 1.05.
- Primary diagnostics: contact duration, force impulse, rebound, bridge-bound
  first-pass energy, velocity-dependent force brightening, and multi-string
  force spread.
- Ranking proxy: onset-aligned spectrum, envelope, centroid, and
  fundamental-relative band balance for A1–A7 at MIDI velocity 80.
- ViSQOL: reported for the top three when the local executable succeeds.
- CLAP and the twelve MAESTRO excerpts are final-system guardrails, not fitted
  here, because they are dominated by the unfinished soundboard, damping,
  pedal, radiation, and note-lifetime path.

## Audition

- `audition_baseline_loudness_matched.wav`
- `audition_winner_loudness_matched.wav`
- `scarlatti_30s_baseline_loudness_matched.wav`
- `scarlatti_30s_winner_loudness_matched.wav`

Raw engine-level files are retained beside the level-matched versions.

See `ranking.csv`, `physical_summary.csv`, and
`per_note_audio_metrics.csv` for the complete result.
"""
    (output / "report.md").write_text(report)
    print(f"Winner: {winner.name}")
    print(f"Output: {output}")


if __name__ == "__main__":
    main()
