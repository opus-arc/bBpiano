#!/usr/bin/env python3
"""
Generate compile-time RT425 dispersion presets for all 88 piano keys.

This script calls fit_allpass.py in --cpp-sos-only mode for each MIDI note,
parses the stable SOS allpass sections, pads lower-order presets with identity
sections, and writes a constexpr C++ header.
"""

from __future__ import annotations

import argparse
import concurrent.futures
import dataclasses
import math
import os
import re
import subprocess
import sys
from pathlib import Path


SCRIPT_DIR = Path(__file__).resolve().parent
DEFAULT_FIT_SCRIPT = SCRIPT_DIR / "fit_allpass.py"
DEFAULT_OUTPUT = (
    SCRIPT_DIR.parent.parent.parent.parent
    / "bBpiano Lite"
    / "core"
    / "piano"
    / "Utils"
    / "RT425DispersionPresets.hpp"
)

MIDI_MIN = 21
MIDI_MAX = 108
SAMPLE_RATE = 44100.0
SECTION_COUNT = 8

BYPASS_SECTION = (1.0, 0.0, 0.0, 0.0, 0.0)

PRIMARY_CONFIGS = [
    (21, 72, 16, 20, 0.995, 0.01),
    (73, 79, 12, 14, 0.990, 0.02),
    (80, 84, 8, 10, 0.985, 0.03),
    (85, 88, 6, 8, 0.980, 0.04),
    (89, 96, 4, 6, 0.970, 0.05),
    (97, 108, 2, 4, 0.950, 0.08),
]

EMERGENCY_CONFIGS = [
    (2, 3, 0.900, 0.12),
    (2, 2, 0.850, 0.20),
    (2, 2, 0.700, 0.35),
]

RT425_WRAPPED_B_VALUES = [
    7.65422867528e-05, 7.49396648865e-05, 7.34193813176e-05, 6.76924110619e-05,
    7.04114631521e-05, 7.02435463694e-05, 6.91483799917e-05, 6.82495996622e-05,
    6.59416402917e-05, 6.31101871706e-05, 5.89555624441e-05, 5.42045766555e-05,
    4.84500422940e-05, 4.20001195757e-05, 3.69823001687e-05, 3.26525804037e-05,
    2.94982205828e-05, 2.72003527387e-05, 2.63323116215e-05, 2.73482644570e-05,
    4.47220343644e-05, 4.82309617505e-05, 5.46638400215e-05, 6.09887392202e-05,
    7.36958261737e-05, 8.65095436745e-05, 1.01521104530e-04, 1.19007588699e-04,
    1.40324713668e-04, 1.65553496769e-04, 1.95895580253e-04, 2.28704837656e-04,
    2.70766388308e-04, 3.20935182010e-04, 3.82518186441e-04, 4.53033494203e-04,
    5.39524838356e-04, 6.42038669212e-04, 7.65610632034e-04, 9.19687041204e-04,
    1.10658019513e-03, 1.31684414206e-03, 1.56750196193e-03, 1.86963263678e-03,
    2.21774123021e-03, 2.62340784426e-03, 3.08057322161e-03, 3.62760766614e-03,
    4.18312976075e-03, 4.84974640760e-03, 5.69648977908e-03, 6.51682988664e-03,
    7.63757806896e-03, 8.90784959011e-03, 1.03354840314e-02, 1.20098344631e-02,
    1.38796361209e-02, 1.62065393215e-02, 1.88724837740e-02, 2.17460534894e-02,
    2.52865339093e-02, 2.91898240240e-02, 3.37806208650e-02, 3.92540679979e-02,
    4.52766527305e-02, 5.29555619648e-02, 6.04250439363e-02, 6.94829412830e-02,
    8.07789299125e-02, 9.47660637069e-02, 1.09287354697e-01, 1.23023756408e-01,
    1.41318081846e-01, 1.62516963556e-01, 1.87664112155e-01, 2.16143284613e-01,
    2.48857496819e-01, 2.78272777224e-01, 3.24660862606e-01, 3.73812410460e-01,
    4.24496470191e-01, 4.84474462934e-01, 5.54465302169e-01, 6.24379024209e-01,
    7.41493224586e-01, 8.94455034069e-01, 1.03254621110e00, 1.19319865264e00,
]

SECTION_RE = re.compile(
    r"\{\s*"
    r"([-+]?(?:\d+(?:\.\d*)?|\.\d+)(?:[eE][-+]?\d+)?)\s*,\s*"
    r"([-+]?(?:\d+(?:\.\d*)?|\.\d+)(?:[eE][-+]?\d+)?)\s*,\s*"
    r"([-+]?(?:\d+(?:\.\d*)?|\.\d+)(?:[eE][-+]?\d+)?)\s*,\s*"
    r"([-+]?(?:\d+(?:\.\d*)?|\.\d+)(?:[eE][-+]?\d+)?)\s*,\s*"
    r"([-+]?(?:\d+(?:\.\d*)?|\.\d+)(?:[eE][-+]?\d+)?)\s*"
    r"\}"
)
LOOP_DELAY_RE = re.compile(
    r"dispersionLoopDelaySamples\s*=\s*"
    r"([-+]?(?:\d+(?:\.\d*)?|\.\d+)(?:[eE][-+]?\d+)?)"
)


@dataclasses.dataclass(frozen=True)
class FitConfig:
    order: int
    max_partial: int
    starts: int
    max_nfev: int
    max_radius: float
    radius_regularization: float


@dataclasses.dataclass(frozen=True)
class Preset:
    midi: int
    f1: float
    b: float
    config: FitConfig
    loop_delay_samples: float
    sections: list[tuple[float, float, float, float, float]]
    warning: str | None = None


def midi_to_f1(midi: int) -> float:
    return 440.0 * math.pow(2.0, (midi - 69) / 12.0)


def format_double(value: float) -> str:
    return f"{value:.17g}"


def parse_sections(stdout: str) -> list[tuple[float, float, float, float, float]]:
    sections: list[tuple[float, float, float, float, float]] = []
    for line in stdout.splitlines():
        match = SECTION_RE.search(line)
        if not match:
            continue
        sections.append(tuple(float(match.group(i)) for i in range(1, 6)))
    return sections


def parse_loop_delay_samples(stdout: str) -> float:
    match = LOOP_DELAY_RE.search(stdout)
    if not match:
        raise RuntimeError(f"missing dispersionLoopDelaySamples in fit output:\n{stdout}")
    return float(match.group(1))


def run_fit(
    fit_script: Path,
    midi: int,
    f1: float,
    b: float,
    config: FitConfig,
) -> tuple[float, list[tuple[float, float, float, float, float]]]:
    cmd = [
        sys.executable,
        str(fit_script),
        "--sample-rate",
        format_double(SAMPLE_RATE),
        "--f1",
        format_double(f1),
        "--B",
        format_double(b),
        "--order",
        str(config.order),
        "--max-partial",
        str(config.max_partial),
        "--max-radius",
        format_double(config.max_radius),
        "--radius-regularization",
        format_double(config.radius_regularization),
        "--starts",
        str(config.starts),
        "--max-nfev",
        str(config.max_nfev),
        "--cpp-sos-only",
    ]
    result = subprocess.run(
        cmd,
        check=False,
        text=True,
        capture_output=True,
    )
    if result.returncode != 0:
        raise RuntimeError(
            f"fit_allpass failed for MIDI {midi} maxPartial={config.max_partial}: "
            f"{result.stderr.strip() or result.stdout.strip()}"
        )

    loop_delay_samples = parse_loop_delay_samples(result.stdout)
    sections = parse_sections(result.stdout)
    expected_sections = config.order // 2
    if len(sections) != expected_sections:
        raise RuntimeError(
            f"parsed {len(sections)} sections for MIDI {midi}; expected {expected_sections}. "
            f"stdout was:\n{result.stdout}"
        )
    return loop_delay_samples, sections


def identity_sections() -> list[tuple[float, float, float, float, float]]:
    return [BYPASS_SECTION for _ in range(SECTION_COUNT)]


def padded_sections(
    sections: list[tuple[float, float, float, float, float]],
) -> list[tuple[float, float, float, float, float]]:
    if len(sections) > SECTION_COUNT:
        raise ValueError(f"cannot fit {len(sections)} sections into {SECTION_COUNT}")
    return sections + [BYPASS_SECTION for _ in range(SECTION_COUNT - len(sections))]


def primary_config_index(midi: int) -> int:
    for index, (lo, hi, *_rest) in enumerate(PRIMARY_CONFIGS):
        if lo <= midi <= hi:
            return index
    raise ValueError(f"MIDI {midi} is outside the RT425 range")


def candidate_configs(
    midi: int,
    starts: int,
    max_nfev: int,
) -> list[FitConfig]:
    configs: list[FitConfig] = []
    for _lo, _hi, order, max_partial, max_radius, radius_regularization in PRIMARY_CONFIGS[
        primary_config_index(midi) :
    ]:
        configs.append(
            FitConfig(
                order=order,
                max_partial=max_partial,
                starts=starts,
                max_nfev=max_nfev,
                max_radius=max_radius,
                radius_regularization=radius_regularization,
            )
        )

    for order, max_partial, max_radius, radius_regularization in EMERGENCY_CONFIGS:
        configs.append(
            FitConfig(
                order=order,
                max_partial=max_partial,
                starts=starts,
                max_nfev=max_nfev,
                max_radius=max_radius,
                radius_regularization=radius_regularization,
            )
        )

    deduped: list[FitConfig] = []
    seen: set[tuple[int, int, float, float]] = set()
    for config in configs:
        key = (
            config.order,
            config.max_partial,
            config.max_radius,
            config.radius_regularization,
        )
        if key in seen:
            continue
        seen.add(key)
        deduped.append(config)
    return deduped


def fit_midi(
    fit_script: Path,
    midi: int,
    starts: int,
    max_nfev: int,
) -> Preset:
    b = RT425_WRAPPED_B_VALUES[midi - MIDI_MIN]
    f1 = midi_to_f1(midi)
    errors: list[str] = []

    for config in candidate_configs(midi, starts, max_nfev):
        try:
            loop_delay_samples, sections = run_fit(fit_script, midi, f1, b, config)
            if loop_delay_samples <= 0.0:
                raise RuntimeError(
                    f"non-positive loopDelaySamples={loop_delay_samples:.12f}"
                )

            section_count = len(sections)
            print(
                f"MIDI {midi:3d} f1={f1:.6f} B={b:.12g} "
                f"ok order={config.order} maxPartial={config.max_partial} "
                f"sections={section_count} loopDelay={loop_delay_samples:.9f}",
                flush=True,
            )
            return Preset(
                midi=midi,
                f1=f1,
                b=b,
                config=config,
                loop_delay_samples=loop_delay_samples,
                sections=padded_sections(sections),
            )
        except Exception as exc:
            message = str(exc)
            errors.append(message)
            print(
                f"MIDI {midi:3d} f1={f1:.6f} B={b:.12g} "
                f"failed order={config.order} maxPartial={config.max_partial} "
                f"maxRadius={config.max_radius} reg={config.radius_regularization}: {message}",
                flush=True,
            )

    fallback_config = FitConfig(
        order=0,
        max_partial=1,
        starts=starts,
        max_nfev=max_nfev,
        max_radius=0.0,
        radius_regularization=0.0,
    )
    warning = "fit failed or produced non-positive loop delay for all configs; bypass emitted"
    print(f"WARNING MIDI {midi}: {warning}", flush=True)
    for error in errors:
        print(f"  {error}", flush=True)
    return Preset(
        midi=midi,
        f1=f1,
        b=b,
        config=fallback_config,
        loop_delay_samples=SAMPLE_RATE / f1,
        sections=identity_sections(),
        warning=warning,
    )


def render_header(presets: list[Preset]) -> str:
    lines: list[str] = [
        "#pragma once",
        "",
        "#include <array>",
        "#include <cstddef>",
        "",
        "namespace Piano::Data {",
        "",
        "struct AllpassBiquad {",
        "    double b0, b1, b2;",
        "    double a1, a2;",
        "};",
        "",
        "struct DispersionPreset {",
        "    int midi;",
        "    double referenceF1;",
        "    double B;",
        "    double sampleRate;",
        "    double loopDelaySamples;",
        "    int order;",
        "    int sectionCount;",
        "    std::array<AllpassBiquad, 8> sections;",
        "};",
        "",
        "inline constexpr int kRT425DispersionMidiMin = 21;",
        "inline constexpr int kRT425DispersionMidiMax = 108;",
        "inline constexpr int kRT425DispersionSectionCount = 8;",
        "",
        "inline constexpr std::array<DispersionPreset, 88> kRT425DispersionPresets = {{",
    ]

    for preset_index, preset in enumerate(presets):
        comma = "," if preset_index != len(presets) - 1 else ""
        lines.append(
            f"    // MIDI {preset.midi}, f1 = {preset.f1:.12f}, "
            f"B = {preset.b:.12g}, maxPartial = {preset.config.max_partial}, "
            f"loopDelaySamples = {preset.loop_delay_samples:.12f}, "
            f"order = {preset.config.order}, starts = {preset.config.starts}, "
            f"maxNfev = {preset.config.max_nfev}, maxRadius = {preset.config.max_radius}, "
            f"radiusRegularization = {preset.config.radius_regularization}, "
            f"sectionCount = {preset.config.order // 2}"
        )
        if preset.warning:
            lines.append(f"    // WARNING: {preset.warning}")
        lines.extend(
            [
                "    DispersionPreset{",
                f"        {preset.midi},",
                f"        {format_double(preset.f1)},",
                f"        {format_double(preset.b)},",
                f"        {format_double(SAMPLE_RATE)},",
                f"        {format_double(preset.loop_delay_samples)},",
                f"        {preset.config.order},",
                f"        {preset.config.order // 2},",
                "        {{",
            ]
        )

        for section_index, section in enumerate(preset.sections):
            section_comma = "," if section_index != len(preset.sections) - 1 else ""
            b0, b1, b2, a1, a2 = section
            lines.append(
                "            "
                f"AllpassBiquad{{{format_double(b0)}, {format_double(b1)}, "
                f"{format_double(b2)}, {format_double(a1)}, {format_double(a2)}}}"
                f"{section_comma}"
            )
        lines.extend(["        }}", f"    }}{comma}"])

    lines.extend(
        [
            "}};",
            "",
            "inline constexpr const DispersionPreset& getRT425DispersionPreset(int midi) {",
            "    return kRT425DispersionPresets[static_cast<std::size_t>(midi - kRT425DispersionMidiMin)];",
            "}",
            "",
            "} // namespace Piano::Data",
            "",
        ]
    )
    return "\n".join(lines)


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Generate RT425DispersionPresets.hpp by fitting 88 stable SOS allpass presets.",
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
    )
    parser.add_argument("--fit-script", type=Path, default=DEFAULT_FIT_SCRIPT)
    parser.add_argument("--output", type=Path, default=DEFAULT_OUTPUT)
    parser.add_argument("--midi-min", type=int, default=MIDI_MIN)
    parser.add_argument("--midi-max", type=int, default=MIDI_MAX)
    parser.add_argument("--starts", type=int, default=24)
    parser.add_argument("--max-nfev", type=int, default=3000)
    parser.add_argument(
        "--jobs",
        type=int,
        default=min(8, max(1, (os.cpu_count() or 4) // 2)),
        help="Number of concurrent fit_allpass.py subprocesses",
    )
    return parser.parse_args()


def main() -> None:
    args = parse_args()
    fit_script = args.fit_script.resolve()
    output = args.output.resolve()

    if len(RT425_WRAPPED_B_VALUES) != 88:
        raise RuntimeError(f"expected 88 B values, found {len(RT425_WRAPPED_B_VALUES)}")
    if not fit_script.exists():
        raise FileNotFoundError(f"fit script not found: {fit_script}")
    if args.midi_min != MIDI_MIN or args.midi_max != MIDI_MAX:
        raise ValueError("This generator writes the full 88-key constexpr array; use MIDI 21..108.")

    if args.jobs < 1:
        raise ValueError("--jobs must be >= 1")

    presets_by_midi: dict[int, Preset] = {}
    print(f"Generating {MIDI_MAX - MIDI_MIN + 1} presets with jobs={args.jobs}", flush=True)

    with concurrent.futures.ThreadPoolExecutor(max_workers=args.jobs) as executor:
        futures = {
            executor.submit(
                fit_midi,
                fit_script,
                midi,
                args.starts,
                args.max_nfev,
            ): midi
            for midi in range(MIDI_MIN, MIDI_MAX + 1)
        }
        for future in concurrent.futures.as_completed(futures):
            midi = futures[future]
            presets_by_midi[midi] = future.result()
            print(
                f"progress {len(presets_by_midi):02d}/88 "
                f"(last completed MIDI {midi})",
                flush=True,
            )

    presets = [presets_by_midi[midi] for midi in range(MIDI_MIN, MIDI_MAX + 1)]

    warning_count = sum(1 for preset in presets if preset.warning)
    header = render_header(presets)
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_text(header, encoding="utf-8")
    print(f"Wrote {output}")
    if warning_count:
        print(f"WARNING: {warning_count} presets used fallback sections")


if __name__ == "__main__":
    main()
