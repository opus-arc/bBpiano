#!/usr/bin/env python3
"""Generate per-string RT425 dispersion presets for bBpiano Lite."""

from __future__ import annotations

import argparse
import concurrent.futures
import dataclasses
import importlib.util
import json
import math
import os
import subprocess
import sys
from pathlib import Path


SCRIPT_DIR = Path(__file__).resolve().parent
PROJECT_ROOT = SCRIPT_DIR.parents[4]
BASE_GENERATOR = (
    SCRIPT_DIR / "generate_rt425_dispersion_header.py"
)
DEFAULT_FIT_SCRIPT = SCRIPT_DIR / "fit_allpass.py"
DEFAULT_OUTPUT = (
    PROJECT_ROOT
    / "Cli/bBpiano Lite/core/piano/ModelParameters/constants/RT425DispersionPresets.hpp"
)
DEFAULT_CACHE_DIR = (
    Path(__file__).resolve().parent.parent
    / "outputs/rt425_string_dispersion_fit_cache"
)

SAMPLE_RATE = 44100.0
SECTION_COUNT = 8
BYPASS_SECTION = (1.0, 0.0, 0.0, 0.0, 0.0)

PRIMARY_CONFIGS = [
    (21, 72, 16, 20, 0.995, 0.01),
    (73, 79, 12, 14, 0.990, 0.02),
    (80, 84, 6, 8, 0.980, 0.04),
    (85, 96, 4, 6, 0.970, 0.05),
    (97, 108, 2, 4, 0.950, 0.08),
]
CACHE_VERSION = "per-string-fitted-b-high-frequency-order-v2"


STRING_DATA = """
21 27.3782
22 29.0101
23 30.7393
24 32.5716
25 34.5131
26 36.5703
27 38.7502
28 41.06
29 43.5075
30 46.1008
31 48.8248
31 48.8488
32 51.7351
32 51.7605
33 54.8189
33 54.8458
34 58.084
34 58.1126
35 61.5437
35 61.5739
36 65.2094
36 65.2414
37 69.0934
37 69.1273
38 73.2088
38 73.2447
38 73.2807
39 77.5692
39 77.6074
39 77.6455
40 82.1895
40 82.2299
40 82.2702
41 87.0849
41 87.1277
41 87.1705
42 92.2719
42 92.3172
42 92.3625
43 97.7678
43 97.8158
43 97.8639
44 103.591
44 103.642
44 103.693
45 109.761
45 109.815
45 109.869
46 116.295
46 116.352
46 116.41
47 123.218
47 123.279
47 123.339
48 130.553
48 130.617
48 130.682
49 138.325
49 138.393
49 138.461
50 146.559
50 146.631
50 146.703
51 155.284
51 155.36
51 155.436
52 164.528
52 164.609
52 164.689
53 174.322
53 174.408
53 174.493
54 184.699
54 184.79
54 184.881
55 195.694
55 195.79
55 195.886
56 207.344
56 207.445
56 207.547
57 219.687
57 219.794
57 219.902
58 232.768
58 232.882
58 232.997
59 246.628
59 246.749
59 246.871
60 261.314
60 261.442
60 261.571
61 276.874
61 277.01
61 277.146
62 293.361
62 293.505
62 293.649
63 310.829
63 310.982
63 311.134
64 329.337
64 329.499
64 329.661
65 348.948
65 349.119
65 349.291
66 369.726
66 369.908
66 370.09
67 391.742
67 391.934
67 392.127
68 415.068
68 415.272
68 415.476
69 439.784
69 440
69 440.216
70 465.994
70 466.223
70 466.452
71 493.767
71 494.01
71 494.252
72 523.195
72 523.452
72 523.709
73 554.376
73 554.649
73 554.921
74 587.416
74 587.705
74 587.994
75 622.426
75 622.732
75 623.037
76 659.521
76 659.845
76 660.169
77 698.828
77 699.171
77 699.515
78 740.477
78 740.841
78 741.205
79 784.608
79 784.994
79 785.379
80 831.37
80 831.778
80 832.187
81 880.918
81 881.351
81 881.784
82 933.564
82 934.023
82 934.481
83 989.356
83 989.842
83 990.328
84 1048.48
84 1049
84 1049.51
85 1111.14
85 1111.69
85 1112.23
86 1177.55
86 1178.12
86 1178.7
87 1247.92
87 1248.53
87 1249.15
88 1322.5
88 1323.15
88 1323.8
89 1401.53
89 1402.22
89 1402.91
90 1485.29
90 1486.02
90 1486.75
91 1574.06
91 1574.83
91 1575.6
92 1668.12
92 1668.94
92 1669.76
93 1767.81
93 1768.68
93 1769.55
94 1874.6
94 1875.52
94 1876.45
95 1987.84
95 1988.82
95 1989.8
96 2107.92
96 2108.96
96 2109.99
97 2235.25
97 2236.35
97 2237.45
98 2370.28
98 2371.44
98 2372.61
99 2513.46
99 2514.7
99 2515.93
100 2665.29
100 2666.6
100 2667.91
101 2826.29
101 2827.68
101 2829.07
102 2997.02
102 2998.49
102 2999.97
103 3178.06
103 3179.62
103 3181.18
104 3370.04
104 3371.69
104 3373.35
105 3573.61
105 3575.37
105 3577.12
106 3789.48
106 3791.34
106 3793.21
107 4018.39
107 4020.37
107 4022.34
108 4261.13
108 4263.23
108 4265.32
"""


def load_base_generator():
    spec = importlib.util.spec_from_file_location("rt425_base_generator", BASE_GENERATOR)
    if spec is None or spec.loader is None:
        raise RuntimeError(f"cannot load {BASE_GENERATOR}")
    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


BASE = load_base_generator()
STRING_SPECS = [
    (index, int(midi), float(f0))
    for index, (midi, f0) in enumerate(
        line.split() for line in STRING_DATA.strip().splitlines()
    )
]


@dataclasses.dataclass(frozen=True)
class StringPreset:
    string_index: int
    midi: int
    f1: float
    b: float
    config: object
    loop_delay_samples: float
    sections: list[tuple[float, float, float, float, float]]
    warning: str | None = None


def compute_fitted_b(f0_hz: float) -> float:
    k1 = -1.3333333333333333
    k2 = 1.3333333333333333
    c0 = -6.5760753629956712
    c1 = 1.9137732290077178
    c2 = 0.27614763151078042
    c3 = -0.28572738557770405
    c4 = -0.30709648310494486

    x = math.log2(f0_hz / 440.0)
    log_b = (
        c0
        + c1 * x
        + c2 * x * x
        + c3 * max(x - k1, 0.0) ** 2
        + c4 * max(x - k2, 0.0) ** 2
    )
    log_b = min(max(log_b, math.log(5e-6)), math.log(2e-2))
    return math.exp(log_b)


def candidate_configs(
    midi: int,
    starts: int,
    max_nfev: int,
) -> list[object]:
    start_index = next(
        index
        for index, (lo, hi, *_rest) in enumerate(PRIMARY_CONFIGS)
        if lo <= midi <= hi
    )

    configs = [
        BASE.FitConfig(
            order=order,
            max_partial=max_partial,
            starts=starts,
            max_nfev=max_nfev,
            max_radius=max_radius,
            radius_regularization=regularization,
        )
        for _lo, _hi, order, max_partial, max_radius, regularization
        in PRIMARY_CONFIGS[start_index:]
    ]

    configs.extend(
        BASE.FitConfig(
            order=order,
            max_partial=max_partial,
            starts=starts,
            max_nfev=max_nfev,
            max_radius=max_radius,
            radius_regularization=regularization,
        )
        for order, max_partial, max_radius, regularization
        in BASE.EMERGENCY_CONFIGS
    )

    deduped = []
    seen = set()
    for config in configs:
        key = (
            config.order,
            config.max_partial,
            config.max_radius,
            config.radius_regularization,
        )
        if key not in seen:
            seen.add(key)
            deduped.append(config)
    return deduped


def fit_string(
    cache_dir: Path,
    fit_python: Path,
    fit_script: Path,
    string_index: int,
    midi: int,
    f1: float,
    starts: int,
    max_nfev: int,
) -> StringPreset:
    b = compute_fitted_b(f1)
    cache_path = cache_dir / f"{string_index:03d}.json"
    if cache_path.exists():
        cached = json.loads(cache_path.read_text(encoding="utf-8"))
        if (
            cached.get("version") == CACHE_VERSION
            and cached.get("midi") == midi
            and cached.get("f1") == f1
            and cached.get("starts") == starts
            and cached.get("max_nfev") == max_nfev
        ):
            config = BASE.FitConfig(**cached["config"])
            return StringPreset(
                string_index=string_index,
                midi=midi,
                f1=f1,
                b=b,
                config=config,
                loop_delay_samples=cached["loop_delay_samples"],
                sections=[tuple(section) for section in cached["sections"]],
                warning=cached.get("warning"),
            )

    errors: list[str] = []

    for config in candidate_configs(midi, starts, max_nfev):
        try:
            cmd = [
                str(fit_python),
                str(fit_script),
                "--sample-rate", format_double(SAMPLE_RATE),
                "--f1", format_double(f1),
                "--B", format_double(b),
                "--order", str(config.order),
                "--max-partial", str(config.max_partial),
                "--max-radius", format_double(config.max_radius),
                "--radius-regularization",
                format_double(config.radius_regularization),
                "--starts", str(config.starts),
                "--max-nfev", str(config.max_nfev),
                "--cpp-sos-only",
            ]
            result = subprocess.run(cmd, check=False, text=True, capture_output=True)
            if result.returncode != 0:
                raise RuntimeError(
                    f"fit_allpass failed: "
                    f"{result.stderr.strip() or result.stdout.strip()}"
                )
            loop_delay_samples = BASE.parse_loop_delay_samples(result.stdout)
            sections = BASE.parse_sections(result.stdout)
            expected_sections = config.order // 2
            if len(sections) != expected_sections:
                raise RuntimeError(
                    f"parsed {len(sections)} sections; expected "
                    f"{expected_sections}"
                )
            if loop_delay_samples <= 0.0:
                raise RuntimeError(
                    f"non-positive loopDelaySamples={loop_delay_samples:.12f}"
                )
            print(
                f"string {string_index:03d} MIDI {midi:3d} "
                f"f1={f1:.6f} B={b:.12g} order={config.order} "
                f"loopDelay={loop_delay_samples:.9f}",
                flush=True,
            )
            preset = StringPreset(
                string_index=string_index,
                midi=midi,
                f1=f1,
                b=b,
                config=config,
                loop_delay_samples=loop_delay_samples,
                sections=BASE.padded_sections(sections),
            )
            cache_dir.mkdir(parents=True, exist_ok=True)
            cache_path.write_text(
                json.dumps(
                    {
                        "version": CACHE_VERSION,
                        "midi": midi,
                        "f1": f1,
                        "starts": starts,
                        "max_nfev": max_nfev,
                        "config": dataclasses.asdict(config),
                        "loop_delay_samples": loop_delay_samples,
                        "sections": preset.sections,
                        "warning": None,
                    }
                ),
                encoding="utf-8",
            )
            return preset
        except Exception as exc:
            errors.append(str(exc))

    warning = "all fit configurations failed; bypass emitted"
    print(
        f"WARNING string {string_index} MIDI {midi} f1={f1}: {warning}",
        flush=True,
    )
    for error in errors:
        print(f"  {error}", flush=True)

    fallback = BASE.FitConfig(
        order=0,
        max_partial=1,
        starts=starts,
        max_nfev=max_nfev,
        max_radius=0.0,
        radius_regularization=0.0,
    )
    preset = StringPreset(
        string_index=string_index,
        midi=midi,
        f1=f1,
        b=b,
        config=fallback,
        loop_delay_samples=SAMPLE_RATE / f1,
        sections=[BYPASS_SECTION] * SECTION_COUNT,
        warning=warning,
    )
    cache_dir.mkdir(parents=True, exist_ok=True)
    cache_path.write_text(
        json.dumps(
            {
                "version": CACHE_VERSION,
                "midi": midi,
                "f1": f1,
                "starts": starts,
                "max_nfev": max_nfev,
                "config": dataclasses.asdict(fallback),
                "loop_delay_samples": preset.loop_delay_samples,
                "sections": preset.sections,
                "warning": warning,
            }
        ),
        encoding="utf-8",
    )
    return preset


def format_double(value: float) -> str:
    return f"{value:.17g}"


def render_header(presets: list[StringPreset]) -> str:
    lines = [
        "#pragma once",
        "",
        "#include <array>",
        "#include <cstddef>",
        "",
        "namespace Parameters::Tuning::RT425DispersionPresets {",
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
        "inline constexpr int kRT425DispersionSectionCount = 8;",
        f"inline constexpr std::size_t kRT425DispersionPresetCount = {len(presets)};",
        "",
        f"inline constexpr std::array<DispersionPreset, {len(presets)}> "
        "kRT425DispersionPresets = {{",
    ]

    for preset_index, preset in enumerate(presets):
        comma = "," if preset_index + 1 != len(presets) else ""
        lines.append(
            f"    // String {preset.string_index}, MIDI {preset.midi}, "
            f"f1 = {preset.f1:.12f}, B = {preset.b:.12g}, "
            f"maxPartial = {preset.config.max_partial}, "
            f"loopDelaySamples = {preset.loop_delay_samples:.12f}, "
            f"order = {preset.config.order}, sectionCount = "
            f"{preset.config.order // 2}"
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
            section_comma = "," if section_index + 1 != len(preset.sections) else ""
            b0, b1, b2, a1, a2 = section
            lines.append(
                "            "
                f"AllpassBiquad{{{format_double(b0)}, {format_double(b1)}, "
                f"{format_double(b2)}, {format_double(a1)}, "
                f"{format_double(a2)}}}{section_comma}"
            )
        lines.extend(["        }}", f"    }}{comma}"])

    lines.extend(
        [
            "}};",
            "",
            "inline constexpr const DispersionPreset& "
            "getRT425DispersionPreset(double f0_hz) {",
            "    std::size_t bestIndex = 0;",
            "    double bestDistance = "
            "kRT425DispersionPresets[0].referenceF1 - f0_hz;",
            "    if (bestDistance < 0.0) bestDistance = -bestDistance;",
            "",
            "    for (std::size_t i = 1; i < "
            "kRT425DispersionPresets.size(); ++i) {",
            "        double distance = "
            "kRT425DispersionPresets[i].referenceF1 - f0_hz;",
            "        if (distance < 0.0) distance = -distance;",
            "        if (distance < bestDistance) {",
            "            bestDistance = distance;",
            "            bestIndex = i;",
            "        }",
            "    }",
            "    return kRT425DispersionPresets[bestIndex];",
            "}",
            "",
            "} // namespace Parameters::Tuning::RT425DispersionPresets",
            "",
        ]
    )
    return "\n".join(lines)


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser()
    parser.add_argument("--fit-script", type=Path, default=DEFAULT_FIT_SCRIPT)
    parser.add_argument("--fit-python", type=Path, default=Path(sys.executable))
    parser.add_argument("--cache-dir", type=Path, default=DEFAULT_CACHE_DIR)
    parser.add_argument("--output", type=Path, default=DEFAULT_OUTPUT)
    parser.add_argument("--starts", type=int, default=24)
    parser.add_argument("--max-nfev", type=int, default=3000)
    parser.add_argument(
        "--jobs",
        type=int,
        default=min(8, max(1, (os.cpu_count() or 4) // 2)),
    )
    return parser.parse_args()


def main() -> None:
    args = parse_args()
    fit_script = args.fit_script.resolve()
    fit_python = args.fit_python.resolve()
    cache_dir = args.cache_dir.resolve()
    output = args.output.resolve()

    if not fit_script.exists():
        raise FileNotFoundError(fit_script)
    if not fit_python.exists():
        raise FileNotFoundError(fit_python)
    if args.jobs < 1:
        raise ValueError("--jobs must be >= 1")
    if len(STRING_SPECS) != 237:
        raise RuntimeError(f"expected 237 strings, got {len(STRING_SPECS)}")

    presets_by_index: dict[int, StringPreset] = {}
    print(f"Generating {len(STRING_SPECS)} per-string presets", flush=True)

    with concurrent.futures.ThreadPoolExecutor(max_workers=args.jobs) as executor:
        futures = {
            executor.submit(
                fit_string,
                cache_dir,
                fit_python,
                fit_script,
                string_index,
                midi,
                f1,
                args.starts,
                args.max_nfev,
            ): string_index
            for string_index, midi, f1 in STRING_SPECS
        }
        for future in concurrent.futures.as_completed(futures):
            string_index = futures[future]
            presets_by_index[string_index] = future.result()
            print(
                f"progress {len(presets_by_index):03d}/{len(STRING_SPECS)}",
                flush=True,
            )

    presets = [presets_by_index[i] for i in range(len(STRING_SPECS))]
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_text(render_header(presets), encoding="utf-8")
    print(f"Wrote {output}", flush=True)


if __name__ == "__main__":
    main()
