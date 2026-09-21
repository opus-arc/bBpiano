#!/usr/bin/env python3
"""Build and execute the benchmark-validity audit notebook without Jupyter."""

from __future__ import annotations

import contextlib
import io
import json
from pathlib import Path


ROOT = Path(__file__).resolve().parent


def markdown(source: str) -> dict:
    return {
        "cell_type": "markdown",
        "metadata": {},
        "source": source.splitlines(keepends=True),
    }


def code(source: str) -> dict:
    return {
        "cell_type": "code",
        "execution_count": None,
        "metadata": {},
        "outputs": [],
        "source": source.splitlines(keepends=True),
    }


cells = [
    markdown(
        """## tl;dr

The previous `29.1%` score improvement is not robust.  It appears only when
partials below the residual-noise level are admitted (`minimum_snr_db=-15`
or `-20`).  With the reliable modes (`SNR >= 0 dB`), the restored Stulov
baseline scores better.

The larger mismatch with Friture is caused by the current `StringModel`, not
by least-squares leakage.  Direct model output contains essentially only odd
harmonics: total even/odd peak power is about `-105 dB`.  The benchmark
projects force into a different, ideal fixed-fixed string that contains every
mode, so its bars cannot predict the live bBworks spectrum.
"""
    ),
    markdown(
        """## Context & Methods

This is a diagnostic and data-quality audit, not a new hammer experiment.
Production `HammerModel`, `StringModel`, waveguide, delay, loss, and benchmark
code are read-only inputs.

### Key Assumptions

- C4 fundamental: 261.626 Hz.
- Direct-output probe reads the existing `StringModel::getSamples()` at
  44.1 kHz after the restored MIDI-80 strike.
- Target estimator reconciliation uses the same Pianoteq C4 MIDI-80 WAV.
- Positive `phase1_minus_restored_db` means phase 1 is worse.
"""
    ),
    markdown("## Data\n\nLoad the four reviewed audit tables."),
    code(
        """from pathlib import Path
import csv
import math

ROOT = Path.cwd()

def load_csv(name):
    with (ROOT / name).open(encoding="utf-8") as stream:
        return list(csv.DictReader(stream))

direct_modes = load_csv("direct_output_modes.csv")
target_estimators = load_csv("target_estimator_reconciliation.csv")
snr_sensitivity = load_csv("snr_threshold_sensitivity.csv")
window_sensitivity = load_csv("window_sensitivity.csv")

print(
    len(direct_modes),
    len(target_estimators),
    len(snr_sensitivity),
    len(window_sensitivity),
)
"""
    ),
    markdown("## Results\n\n### Direct StringModel output contains only odd modes"),
    code(
        """odd_power = sum(
    10.0 ** (float(row["relative_peak_power_db"]) / 10.0)
    for row in direct_modes
    if row["parity"] == "odd"
)
even_power = sum(
    10.0 ** (float(row["relative_peak_power_db"]) / 10.0)
    for row in direct_modes
    if row["parity"] == "even"
)
even_over_odd_db = 10.0 * math.log10(even_power / odd_power)
print(f"even/odd peak power = {even_over_odd_db:.3f} dB")
print("strong modes:", [
    int(row["mode"])
    for row in direct_modes
    if float(row["relative_peak_power_db"]) > -60.0
])
"""
    ),
    markdown("### Least-squares and FFT extraction agree on the target WAV"),
    code(
        """maximum_estimator_difference = max(
    abs(float(row["ls_minus_fft100_db"]))
    for row in target_estimators
)
print(
    "maximum |LS100 - FFT100| = "
    f"{maximum_estimator_difference:.3f} dB"
)
print(
    "negative-SNR modes:",
    [
        int(row["mode"])
        for row in target_estimators
        if float(row["target_snr_db"]) < 0.0
    ],
)
"""
    ),
    markdown("### The model ranking reverses when noisy modes are excluded"),
    code(
        """for row in snr_sensitivity:
    print(
        f"SNR >= {float(row['minimum_snr_db']):5.1f} dB: "
        f"restored={float(row['restored_stulov_rmse_db']):.4f}, "
        f"phase1={float(row['chabassier_phase1_rmse_db']):.4f}, "
        f"delta={float(row['phase1_minus_restored_db']):+.4f}"
    )
"""
    ),
    markdown("### The apparent improvement disappears in a 500 ms window"),
    code(
        """for row in window_sensitivity:
    print(
        f"{int(row['window_ms']):3d} ms: "
        f"delta={float(row['phase1_minus_restored_db']):+.4f} dB"
    )
"""
    ),
    markdown(
        """## Takeaways

1. Do not use the previous `-20 dB SNR / 100 ms` score for model selection.
2. The target partial estimator is numerically well-conditioned and agrees
   with an FFT peak estimator; adjacent-mode mixing is not the observed cause.
3. The current waveguide boundary implementation recirculates each travelling
   rail independently with one sign inversion per one-way delay.  Its loop
   resonance condition admits `(2k+1)f0` only.
4. Until the string topology is either corrected or explicitly included in a
   new observation-domain benchmark, force-only ideal-string scores must be
   labelled counterfactual and cannot be compared to Friture.
"""
    ),
]


namespace: dict = {}
execution_count = 0
original_cwd = Path.cwd()
try:
    import os

    os.chdir(ROOT)
    for cell in cells:
        if cell["cell_type"] != "code":
            continue
        execution_count += 1
        source = "".join(cell["source"])
        captured = io.StringIO()
        with contextlib.redirect_stdout(captured):
            exec(compile(source, "<audit-cell>", "exec"), namespace)
        text = captured.getvalue()
        cell["execution_count"] = execution_count
        if text:
            cell["outputs"] = [{
                "name": "stdout",
                "output_type": "stream",
                "text": text.splitlines(keepends=True),
            }]
finally:
    os.chdir(original_cwd)


notebook = {
    "cells": cells,
    "metadata": {
        "kernelspec": {
            "display_name": "Python 3",
            "language": "python",
            "name": "python3",
        },
        "language_info": {
            "name": "python",
            "version": "3",
        },
    },
    "nbformat": 4,
    "nbformat_minor": 5,
}

output = ROOT / "benchmark_validity_audit.ipynb"
output.write_text(
    json.dumps(notebook, indent=1, ensure_ascii=False),
    encoding="utf-8",
)
print(output)
