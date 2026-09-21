#!/usr/bin/env python3
"""
Hammer-only modal-energy fingerprint benchmark for bBpiano.

The authoritative path consumes a HammerModel force log and projects the
contact force onto the undamped, dispersion-free fixed-string modes.  It does
not call StringModel and therefore does not include the waveguide delay,
boundary loss, dispersion, pickup response, soundboard, microphone, or room.

A target may be either:

1. another force log (physical hammer-only comparison), or
2. a PCM WAV file (an explicitly labelled acoustic proxy comparison).

Run:
    python hammer_modal_benchmark.py \
        --force-log force.txt \
        --target-force-log target_force.txt \
        --output-dir benchmark

or:
    python hammer_modal_benchmark.py \
        --force-log force.txt \
        --reference-wav target.wav \
        --audio-kind microphone \
        --output-dir benchmark

Use --self-test to synthesize an ideal string-velocity observation from the
force fingerprint and verify that the audio analysis recovers its energy
distribution.
"""

from __future__ import annotations

import argparse
import csv
import json
import math
import wave
from dataclasses import dataclass, replace
from pathlib import Path

import numpy as np

from hammer_analyzer import load_force_file, select_first_contact

try:
    import soundfile as sf
except ImportError:
    sf = None

try:
    import matplotlib.pyplot as plt
except ImportError:
    plt = None


def trapezoid(y, x, axis=-1):
    """NumPy 1.x/2.x compatible trapezoidal integration."""
    implementation = getattr(np, "trapezoid", np.trapz)
    return implementation(y, x, axis=axis)


@dataclass
class StringSpec:
    sample_rate: float = 44100.0
    f0: float = 261.626
    length_m: float = 0.65
    linear_density_kg_m: float = 0.007
    strike_position: float = 1.0 / 9.4
    pickup_position: float = 6.0 / 7.0
    mode_count: int = 32
    hammer_mass_kg: float = 0.015
    impact_velocity_m_s: float = 2.0

    @property
    def modal_mass_kg(self) -> float:
        # Integral_0^L mu * sin(n*pi*x/L)^2 dx = mu*L/2.
        return self.linear_density_kg_m * self.length_m / 2.0


def _as_float_list(values):
    return [float(value) for value in np.asarray(values)]


def _normalize_energy(energy, valid=None):
    energy = np.asarray(energy, dtype=float)
    if valid is None:
        valid = np.isfinite(energy) & (energy >= 0.0)
    else:
        valid = np.asarray(valid, dtype=bool) & np.isfinite(energy)

    result = np.zeros_like(energy)
    total = float(np.sum(np.maximum(energy[valid], 0.0)))
    if total <= 0.0:
        raise RuntimeError("Fingerprint contains no positive modal energy.")
    result[valid] = np.maximum(energy[valid], 0.0) / total
    return result


def force_modal_fingerprint(path, spec, modal_frequencies_hz=None):
    all_t, all_f = load_force_file(path)
    t, force, event_count = select_first_contact(all_t, all_f)

    delta_t = np.diff(t)
    positive_delta = delta_t[delta_t > 0.0]
    if not len(positive_delta):
        raise RuntimeError("Force log has no positive sample interval.")

    dt = float(np.median(positive_delta))
    force_sample_rate = 1.0 / dt
    if modal_frequencies_hz is None:
        modal_frequencies_hz = (
            np.arange(1, spec.mode_count + 1, dtype=float) * spec.f0
        )
    else:
        modal_frequencies_hz = np.asarray(
            modal_frequencies_hz,
            dtype=float,
        )
        if len(modal_frequencies_hz) != spec.mode_count:
            raise RuntimeError(
                "Measured frequency count does not match mode count."
            )

    highest_frequency = float(np.max(modal_frequencies_hz))
    if highest_frequency >= 0.49 * force_sample_rate:
        raise RuntimeError(
            "Requested modal range reaches the force-log Nyquist limit: "
            f"{highest_frequency:.1f} Hz versus "
            f"{0.5 * force_sample_rate:.1f} Hz."
        )

    # Logs contain F > 0 samples only. Add zero-force samples on both sides so
    # the Fourier integral includes the contact endpoints.
    t = np.concatenate(([t[0] - dt], t, [t[-1] + dt]))
    force = np.concatenate(([0.0], force, [0.0]))
    t = t - t[0]

    mode = np.arange(1, spec.mode_count + 1, dtype=float)
    omega = 2.0 * np.pi * modal_frequencies_hz
    strike_shape = np.sin(np.pi * mode * spec.strike_position)
    phase = np.outer(t, omega)

    cosine_impulse = trapezoid(
        force[:, None] * np.cos(phase),
        t,
        axis=0,
    )
    sine_impulse = trapezoid(
        force[:, None] * np.sin(phase),
        t,
        axis=0,
    )
    complex_impulse_abs = np.hypot(cosine_impulse, sine_impulse)

    modal_mass = spec.modal_mass_kg
    modal_energy = (
        np.square(strike_shape)
        * np.square(complex_impulse_abs)
        / (2.0 * modal_mass)
    )

    # Recover q_n and qdot_n at contact end and independently verify the
    # energy identity shown in the user's equation.
    contact_end = float(t[-1])
    end_phase = omega * contact_end
    q_end = strike_shape / (modal_mass * omega) * (
        np.sin(end_phase) * cosine_impulse
        - np.cos(end_phase) * sine_impulse
    )
    qdot_end = strike_shape / modal_mass * (
        np.cos(end_phase) * cosine_impulse
        + np.sin(end_phase) * sine_impulse
    )
    state_energy = 0.5 * modal_mass * (
        np.square(qdot_end) + np.square(omega * q_end)
    )
    identity_relative_error = float(
        np.max(
            np.abs(state_energy - modal_energy)
            / np.maximum(modal_energy, 1e-30)
        )
    )
    incident_hammer_energy = (
        0.5
        * spec.hammer_mass_kg
        * spec.impact_velocity_m_s ** 2
    )
    energy_over_incident = (
        float(np.sum(modal_energy)) / incident_hammer_energy
        if incident_hammer_energy > 0.0
        else math.inf
    )

    fraction = _normalize_energy(modal_energy)
    return {
        "domain": "force_modal_energy",
        "source": str(Path(path)),
        "mode": mode.astype(int),
        "frequency_hz": omega / (2.0 * np.pi),
        "strike_shape": strike_shape,
        "complex_force_impulse_Ns": complex_impulse_abs,
        "modal_energy_J": modal_energy,
        "energy_fraction": fraction,
        "energy_db": 10.0 * np.log10(
            np.maximum(fraction, 1e-15) / np.max(fraction)
        ),
        "q_end_m": q_end,
        "qdot_end_m_s": qdot_end,
        "metadata": {
            "contact_event_count": int(event_count),
            "contact_span_s": float(t[-2] - t[1]),
            "force_sample_rate_hz": force_sample_rate,
            "modal_mass_kg": modal_mass,
            "total_modal_energy_J": float(np.sum(modal_energy)),
            "incident_hammer_energy_J": incident_hammer_energy,
            "projection_energy_over_incident_hammer_energy": (
                energy_over_incident
            ),
            "closed_system_energy_budget_applicable": False,
            "energy_identity_max_relative_error": identity_relative_error,
            "modal_frequencies_hz": _as_float_list(
                modal_frequencies_hz
            ),
        },
    }


def _decode_pcm_wav(path):
    try:
        with wave.open(str(path), "rb") as wav:
            channel_count = wav.getnchannels()
            sample_rate = wav.getframerate()
            sample_width = wav.getsampwidth()
            frame_count = wav.getnframes()
            compression = wav.getcomptype()
            raw = wav.readframes(frame_count)
    except wave.Error as error:
        if sf is not None:
            try:
                samples, sample_rate = sf.read(
                    str(path),
                    dtype="float64",
                    always_2d=True,
                )
            except (RuntimeError, TypeError) as soundfile_error:
                raise RuntimeError(
                    "Reference must be an uncompressed PCM or IEEE-float "
                    "WAV file."
                ) from soundfile_error
            return sample_rate, np.mean(samples, axis=1)
        raise RuntimeError(
            "Reference must be an uncompressed PCM WAV file. Install "
            "python-soundfile to read IEEE-float WAV files."
        ) from error

    if compression != "NONE":
        raise RuntimeError("Compressed WAV files are not supported.")

    if sample_width == 1:
        samples = (
            np.frombuffer(raw, dtype=np.uint8).astype(float) - 128.0
        ) / 128.0
    elif sample_width == 2:
        samples = np.frombuffer(raw, dtype="<i2").astype(float) / 32768.0
    elif sample_width == 3:
        bytes_24 = np.frombuffer(raw, dtype=np.uint8).reshape(-1, 3)
        values = (
            bytes_24[:, 0].astype(np.int32)
            | (bytes_24[:, 1].astype(np.int32) << 8)
            | (bytes_24[:, 2].astype(np.int32) << 16)
        )
        values = np.where(values & 0x800000, values - 0x1000000, values)
        samples = values.astype(float) / 8388608.0
    elif sample_width == 4:
        samples = (
            np.frombuffer(raw, dtype="<i4").astype(float) / 2147483648.0
        )
    else:
        raise RuntimeError(
            f"Unsupported PCM sample width: {sample_width * 8} bits."
        )

    samples = samples.reshape(-1, channel_count).mean(axis=1)
    return sample_rate, samples


def _detect_onset(samples, sample_rate):
    frame = max(8, int(round(0.002 * sample_rate)))
    power = np.convolve(
        np.square(samples),
        np.ones(frame, dtype=float) / frame,
        mode="same",
    )
    rms = np.sqrt(np.maximum(power, 0.0))
    peak = float(np.max(rms))
    if peak <= 0.0:
        raise RuntimeError("Reference WAV is silent.")

    baseline_length = min(
        len(rms),
        max(frame, int(round(0.05 * sample_rate))),
    )
    baseline = float(np.median(rms[:baseline_length]))
    threshold = max(peak * 0.03, baseline * 8.0)
    candidates = np.flatnonzero(rms >= threshold)
    return int(candidates[0]) if len(candidates) else int(np.argmax(rms))


def estimate_modal_frequencies(
    path,
    spec,
    onset_ms=None,
    post_contact_ms=5.0,
    search_window_ms=500.0,
):
    """Estimate one fixed target frequency for each ideal mode index."""
    sample_rate, samples = _decode_pcm_wav(path)
    onset = (
        int(round(onset_ms * 1e-3 * sample_rate))
        if onset_ms is not None
        else _detect_onset(samples, sample_rate)
    )
    start = onset + int(round(post_contact_ms * 1e-3 * sample_rate))
    requested_length = int(round(search_window_ms * 1e-3 * sample_rate))
    end = min(len(samples), start + requested_length)
    segment = np.asarray(samples[start:end], dtype=float)
    if len(segment) < max(256, requested_length // 2):
        raise RuntimeError(
            "Reference WAV is too short for measured-frequency analysis."
        )

    segment = segment - np.mean(segment)
    segment *= np.hanning(len(segment))
    fft_length = 1
    while fft_length < 8 * len(segment):
        fft_length *= 2
    spectrum = np.abs(np.fft.rfft(segment, n=fft_length))
    frequency_axis = np.fft.rfftfreq(fft_length, 1.0 / sample_rate)

    frequencies = []
    for mode in range(1, spec.mode_count + 1):
        lower = max(1.0, (mode - 0.45) * spec.f0)
        upper = min(
            0.49 * sample_rate,
            (mode + 0.45) * spec.f0,
        )
        candidates = np.flatnonzero(
            (frequency_axis >= lower)
            & (frequency_axis <= upper)
        )
        if not len(candidates):
            raise RuntimeError(
                f"No frequency-search bins for mode {mode}."
            )
        peak_index = int(
            candidates[np.argmax(spectrum[candidates])]
        )

        # Sub-bin parabolic interpolation on log magnitude.
        offset = 0.0
        if 0 < peak_index < len(spectrum) - 1:
            values = np.log(
                np.maximum(
                    spectrum[peak_index - 1:peak_index + 2],
                    1e-30,
                )
            )
            denominator = values[0] - 2.0 * values[1] + values[2]
            if abs(denominator) > 1e-15:
                offset = float(
                    0.5 * (values[0] - values[2]) / denominator
                )
                offset = float(np.clip(offset, -0.5, 0.5))
        frequencies.append(
            (peak_index + offset) * sample_rate / fft_length
        )

    return np.asarray(frequencies, dtype=float)


def _least_squares_partial_amplitudes(samples, sample_rate, frequencies):
    sample_count = len(samples)
    if sample_count < 32:
        raise RuntimeError("Reference analysis window is too short.")

    time = np.arange(sample_count, dtype=float) / sample_rate
    angular_phase = 2.0 * np.pi * np.outer(time, frequencies)
    design = np.concatenate(
        (np.cos(angular_phase), np.sin(angular_phase)),
        axis=1,
    )
    window = np.hanning(sample_count)
    root_window = np.sqrt(np.maximum(window, 0.0))
    weighted_design = design * root_window[:, None]
    centered = samples - np.average(samples, weights=window)
    weighted_samples = centered * root_window

    coefficients, _, _, _ = np.linalg.lstsq(
        weighted_design,
        weighted_samples,
        rcond=None,
    )
    mode_count = len(frequencies)
    cosine = coefficients[:mode_count]
    sine = coefficients[mode_count:]
    amplitude = np.hypot(cosine, sine)

    residual = weighted_samples - weighted_design @ coefficients
    residual_rms = float(np.sqrt(np.mean(np.square(residual))))
    snr_db = 10.0 * np.log10(
        np.maximum(np.square(amplitude), 1e-30)
        / max(2.0 * residual_rms * residual_rms, 1e-30)
    )
    return amplitude, snr_db, residual_rms


def audio_modal_fingerprint(
    path,
    spec,
    audio_kind,
    onset_ms=None,
    post_contact_ms=5.0,
    window_ms=100.0,
    modal_frequencies_hz=None,
):
    sample_rate, samples = _decode_pcm_wav(path)
    onset = (
        int(round(onset_ms * 1e-3 * sample_rate))
        if onset_ms is not None
        else _detect_onset(samples, sample_rate)
    )
    start = onset + int(round(post_contact_ms * 1e-3 * sample_rate))
    length = int(round(window_ms * 1e-3 * sample_rate))
    end = min(len(samples), start + length)
    if end - start < max(32, length // 2):
        raise RuntimeError(
            "Reference WAV does not contain enough samples after onset."
        )

    segment = samples[start:end]
    mode = np.arange(1, spec.mode_count + 1, dtype=float)
    frequencies = (
        mode * spec.f0
        if modal_frequencies_hz is None
        else np.asarray(modal_frequencies_hz, dtype=float)
    )
    if len(frequencies) != spec.mode_count:
        raise RuntimeError(
            "Measured frequency count does not match mode count."
        )
    if frequencies[-1] >= 0.49 * sample_rate:
        raise RuntimeError("Requested mode range reaches WAV Nyquist.")

    amplitude, snr_db, residual_rms = _least_squares_partial_amplitudes(
        segment,
        sample_rate,
        frequencies,
    )
    raw_power = np.square(amplitude)
    pickup_shape = np.sin(np.pi * mode * spec.pickup_position)
    valid = np.ones(spec.mode_count, dtype=bool)

    if audio_kind == "string-velocity":
        valid = np.abs(pickup_shape) >= 0.05
        energy_proxy = raw_power / np.maximum(
            np.square(pickup_shape),
            0.05 ** 2,
        )
        domain = "recovered_modal_energy_from_string_velocity"
    elif audio_kind == "string-displacement":
        valid = np.abs(pickup_shape) >= 0.05
        omega = 2.0 * np.pi * frequencies
        energy_proxy = (
            np.square(omega)
            * raw_power
            / np.maximum(np.square(pickup_shape), 0.05 ** 2)
        )
        domain = "recovered_modal_energy_from_string_displacement"
    elif audio_kind == "microphone":
        energy_proxy = raw_power
        domain = "microphone_partial_power_proxy"
    else:
        raise ValueError(audio_kind)

    fraction = _normalize_energy(energy_proxy, valid)
    return {
        "domain": domain,
        "source": str(Path(path)),
        "mode": mode.astype(int),
        "frequency_hz": frequencies,
        "partial_amplitude": amplitude,
        "pickup_shape": pickup_shape,
        "snr_db": snr_db,
        "energy_proxy": energy_proxy,
        "energy_fraction": fraction,
        "energy_db": 10.0 * np.log10(
            np.maximum(fraction, 1e-15) / np.max(fraction)
        ),
        "valid": valid,
        "metadata": {
            "sample_rate_hz": sample_rate,
            "onset_s": onset / sample_rate,
            "analysis_start_s": start / sample_rate,
            "analysis_duration_s": len(segment) / sample_rate,
            "audio_kind": audio_kind,
            "residual_rms": residual_rms,
            "physical_energy_comparison": audio_kind != "microphone",
            "modal_frequencies_hz": _as_float_list(frequencies),
        },
    }


def fingerprint_distance(candidate, target):
    candidate_energy = np.asarray(candidate["energy_fraction"], dtype=float)
    target_energy = np.asarray(target["energy_fraction"], dtype=float)
    if len(candidate_energy) != len(target_energy):
        raise RuntimeError("Fingerprints use different mode counts.")

    valid = np.ones(len(candidate_energy), dtype=bool)
    if "valid" in candidate:
        valid &= np.asarray(candidate["valid"], dtype=bool)
    if "valid" in target:
        valid &= np.asarray(target["valid"], dtype=bool)
    if not np.any(valid):
        raise RuntimeError("Fingerprints have no common valid modes.")

    p = _normalize_energy(candidate_energy, valid)[valid]
    q = _normalize_energy(target_energy, valid)[valid]
    midpoint = 0.5 * (p + q)
    positive_p = p > 0.0
    positive_q = q > 0.0
    js_divergence = 0.5 * (
        np.sum(p[positive_p] * np.log2(p[positive_p] / midpoint[positive_p]))
        + np.sum(q[positive_q] * np.log2(q[positive_q] / midpoint[positive_q]))
    )
    js_distance = math.sqrt(max(float(js_divergence), 0.0))
    total_variation = 0.5 * float(np.sum(np.abs(p - q)))

    floor_db = -80.0
    p_db = np.maximum(
        10.0 * np.log10(np.maximum(p, 1e-15) / np.max(p)),
        floor_db,
    )
    q_db = np.maximum(
        10.0 * np.log10(np.maximum(q, 1e-15) / np.max(q)),
        floor_db,
    )
    log_energy_rmse_db = float(np.sqrt(np.mean(np.square(p_db - q_db))))

    valid_modes = np.arange(1, len(candidate_energy) + 1)[valid]
    candidate_centroid = float(np.sum(valid_modes * p))
    target_centroid = float(np.sum(valid_modes * q))

    return {
        "valid_mode_count": int(np.sum(valid)),
        "sqrt_jensen_shannon_distance": js_distance,
        "modal_similarity_score_0_100": 100.0 * (1.0 - js_distance),
        "total_variation_distance": total_variation,
        "log_energy_rmse_db": log_energy_rmse_db,
        "candidate_energy_centroid_mode": candidate_centroid,
        "target_energy_centroid_mode": target_centroid,
        "centroid_error_modes": candidate_centroid - target_centroid,
        "comparison_is_physical_hammer_only": (
            candidate["domain"] == "force_modal_energy"
            and target["domain"] == "force_modal_energy"
        ),
    }


def paired_velocity_benchmark(
    manifest_path,
    spec,
    audio_kind,
    baseline_velocity=None,
    post_contact_ms=5.0,
    window_ms=100.0,
    minimum_snr_db=10.0,
    frequency_mode="harmonic",
):
    manifest_path = Path(manifest_path)
    with manifest_path.open(encoding="utf-8") as fp:
        rows = list(csv.DictReader(fp))
    if len(rows) < 2:
        raise RuntimeError(
            "Paired WAV benchmark needs at least two velocities."
        )

    required = {"velocity_m_s", "force_log", "reference_wav"}
    missing = required - set(rows[0])
    if missing:
        raise RuntimeError(
            "Paired manifest is missing columns: "
            + ", ".join(sorted(missing))
        )

    parsed_rows = []
    for row in rows:
        velocity = float(row["velocity_m_s"])
        force_path = (manifest_path.parent / row["force_log"]).resolve()
        wav_path = (manifest_path.parent / row["reference_wav"]).resolve()
        onset_text = row.get("onset_ms", "").strip()
        onset_ms = float(onset_text) if onset_text else None
        parsed_rows.append({
            "velocity_m_s": velocity,
            "force_path": force_path,
            "wav_path": wav_path,
            "onset_ms": onset_ms,
        })

    parsed_rows.sort(key=lambda run: run["velocity_m_s"])
    if baseline_velocity is None:
        baseline_row = parsed_rows[0]
    else:
        candidates = [
            run for run in parsed_rows
            if math.isclose(
                run["velocity_m_s"],
                baseline_velocity,
                rel_tol=0.0,
                abs_tol=1e-9,
            )
        ]
        if not candidates:
            raise RuntimeError(
                f"Baseline velocity {baseline_velocity:g} is not in manifest."
            )
        baseline_row = candidates[0]

    modal_frequencies_hz = None
    if frequency_mode == "measured":
        modal_frequencies_hz = estimate_modal_frequencies(
            baseline_row["wav_path"],
            spec,
            onset_ms=baseline_row["onset_ms"],
            post_contact_ms=post_contact_ms,
        )
    elif frequency_mode != "harmonic":
        raise RuntimeError(
            "frequency_mode must be harmonic or measured."
        )

    runs = []
    for row in parsed_rows:
        velocity = row["velocity_m_s"]
        run_spec = replace(spec, impact_velocity_m_s=velocity)
        runs.append({
            "velocity_m_s": velocity,
            "candidate": force_modal_fingerprint(
                row["force_path"],
                run_spec,
                modal_frequencies_hz=modal_frequencies_hz,
            ),
            "target": audio_modal_fingerprint(
                row["wav_path"],
                run_spec,
                audio_kind,
                onset_ms=row["onset_ms"],
                post_contact_ms=post_contact_ms,
                window_ms=window_ms,
                modal_frequencies_hz=modal_frequencies_hz,
            ),
        })

    baseline = next(
        run for run in runs
        if math.isclose(
            run["velocity_m_s"],
            baseline_row["velocity_m_s"],
            rel_tol=0.0,
            abs_tol=1e-9,
        )
    )

    base_force = np.asarray(
        baseline["candidate"]["modal_energy_J"],
        dtype=float,
    )
    base_audio = np.asarray(
        baseline["target"]["energy_proxy"],
        dtype=float,
    )
    base_snr = np.asarray(baseline["target"]["snr_db"], dtype=float)
    result_rows = []
    per_velocity = []

    for run in runs:
        if run is baseline:
            continue

        force_energy = np.asarray(
            run["candidate"]["modal_energy_J"],
            dtype=float,
        )
        audio_power = np.asarray(
            run["target"]["energy_proxy"],
            dtype=float,
        )
        target_snr = np.asarray(run["target"]["snr_db"], dtype=float)
        valid = (
            np.isfinite(force_energy)
            & np.isfinite(base_force)
            & np.isfinite(audio_power)
            & np.isfinite(base_audio)
            & (force_energy > np.max(force_energy) * 1e-8)
            & (base_force > np.max(base_force) * 1e-8)
            & (audio_power > 0.0)
            & (base_audio > 0.0)
            & (target_snr >= minimum_snr_db)
            & (base_snr >= minimum_snr_db)
        )
        if "valid" in run["target"]:
            valid &= np.asarray(run["target"]["valid"], dtype=bool)
        if "valid" in baseline["target"]:
            valid &= np.asarray(baseline["target"]["valid"], dtype=bool)
        if np.sum(valid) < 3:
            raise RuntimeError(
                f"Only {np.sum(valid)} valid modes remain at "
                f"{run['velocity_m_s']:g} m/s. Lower --minimum-snr-db "
                "or improve the recording."
            )

        candidate_gain_db = 10.0 * np.log10(
            force_energy[valid] / base_force[valid]
        )
        target_gain_db = 10.0 * np.log10(
            audio_power[valid] / base_audio[valid]
        )
        raw_error_db = target_gain_db - candidate_gain_db

        # A per-recording gain change is constant across modes. Removing the
        # median leaves the modal-shape error and keeps the comparison valid
        # even if the WAVs were not recorded at exactly the same gain.
        gain_offset_db = float(np.median(raw_error_db))
        shape_error_db = raw_error_db - gain_offset_db
        shape_rmse_db = float(
            np.sqrt(np.mean(np.square(shape_error_db)))
        )
        raw_rmse_db = float(np.sqrt(np.mean(np.square(raw_error_db))))
        median_absolute_shape_error_db = float(
            np.median(np.abs(shape_error_db))
        )
        percentile_90_shape_error_db = float(
            np.percentile(np.abs(shape_error_db), 90)
        )

        mode_numbers = np.asarray(run["candidate"]["mode"])[valid]
        frequencies = np.asarray(
            run["candidate"]["frequency_hz"]
        )[valid]
        for index, mode_number in enumerate(mode_numbers):
            result_rows.append({
                "baseline_velocity_m_s": baseline["velocity_m_s"],
                "velocity_m_s": run["velocity_m_s"],
                "mode": int(mode_number),
                "frequency_hz": float(frequencies[index]),
                "candidate_gain_db": float(candidate_gain_db[index]),
                "target_gain_db": float(target_gain_db[index]),
                "raw_error_db": float(raw_error_db[index]),
                "shape_error_db": float(shape_error_db[index]),
                "target_snr_db": float(target_snr[valid][index]),
            })

        per_velocity.append({
            "velocity_m_s": run["velocity_m_s"],
            "valid_mode_count": int(np.sum(valid)),
            "gain_offset_db": gain_offset_db,
            "shape_rmse_db": shape_rmse_db,
            "raw_level_rmse_db": raw_rmse_db,
            "median_absolute_shape_error_db": (
                median_absolute_shape_error_db
            ),
            "percentile_90_shape_error_db": (
                percentile_90_shape_error_db
            ),
            "candidate_projection_energy_over_incident": (
                run["candidate"]["metadata"][
                    "projection_energy_over_incident_hammer_energy"
                ]
            ),
        })

    all_shape_error = np.asarray(
        [row["shape_error_db"] for row in result_rows],
        dtype=float,
    )
    return {
        "domain": "paired_velocity_transfer_cancelled",
        "manifest": str(manifest_path),
        "audio_kind": audio_kind,
        "baseline_velocity_m_s": baseline["velocity_m_s"],
        "minimum_snr_db": minimum_snr_db,
        "frequency_mode": frequency_mode,
        "modal_frequencies_hz": (
            _as_float_list(modal_frequencies_hz)
            if modal_frequencies_hz is not None
            else _as_float_list(
                np.arange(1, spec.mode_count + 1) * spec.f0
            )
        ),
        "run_count": len(runs),
        "comparison_count": len(per_velocity),
        "overall_shape_rmse_db": float(
            np.sqrt(np.mean(np.square(all_shape_error)))
        ),
        "overall_median_absolute_shape_error_db": float(
            np.median(np.abs(all_shape_error))
        ),
        "overall_90_percent_shape_error_db": float(
            np.percentile(np.abs(all_shape_error), 90)
        ),
        "per_velocity": per_velocity,
        "rows": result_rows,
        "closed_system_energy_budget_applicable": False,
    }


def write_paired_outputs(output_dir, result):
    output_dir = Path(output_dir)
    output_dir.mkdir(parents=True, exist_ok=True)
    json_path = output_dir / "paired_velocity_benchmark.json"
    json_path.write_text(
        json.dumps(result, indent=2, ensure_ascii=False),
        encoding="utf-8",
    )

    csv_path = output_dir / "paired_velocity_benchmark.csv"
    with csv_path.open("w", newline="", encoding="utf-8") as fp:
        fieldnames = [
            "baseline_velocity_m_s",
            "velocity_m_s",
            "mode",
            "frequency_hz",
            "candidate_gain_db",
            "target_gain_db",
            "raw_error_db",
            "shape_error_db",
            "target_snr_db",
        ]
        writer = csv.DictWriter(fp, fieldnames=fieldnames)
        writer.writeheader()
        writer.writerows(result["rows"])
    return {"json": str(json_path), "csv": str(csv_path)}


def synthesize_ideal_velocity(fingerprint, spec, duration_s=0.12):
    sample_count = int(round(duration_s * spec.sample_rate))
    time = np.arange(sample_count, dtype=float) / spec.sample_rate
    mode = np.asarray(fingerprint["mode"], dtype=float)
    omega = 2.0 * np.pi * spec.f0 * mode
    pickup_shape = np.sin(np.pi * mode * spec.pickup_position)
    q_end = np.asarray(fingerprint["q_end_m"], dtype=float)
    qdot_end = np.asarray(fingerprint["qdot_end_m_s"], dtype=float)

    phase = np.outer(time, omega)
    modal_velocity = (
        -np.sin(phase) * (omega * q_end)[None, :]
        + np.cos(phase) * qdot_end[None, :]
    )
    signal = modal_velocity @ pickup_shape
    peak = float(np.max(np.abs(signal)))
    if peak > 0.0:
        signal = 0.8 * signal / peak
    return signal


def self_test(force_fingerprint, spec):
    signal = synthesize_ideal_velocity(force_fingerprint, spec)
    frequencies = (
        np.arange(1, spec.mode_count + 1, dtype=float) * spec.f0
    )
    amplitude, _, _ = _least_squares_partial_amplitudes(
        signal,
        spec.sample_rate,
        frequencies,
    )
    pickup_shape = np.sin(
        np.pi
        * np.arange(1, spec.mode_count + 1, dtype=float)
        * spec.pickup_position
    )
    valid = np.abs(pickup_shape) >= 0.05
    recovered = np.square(amplitude) / np.maximum(
        np.square(pickup_shape),
        0.05 ** 2,
    )
    synthetic_fingerprint = {
        "domain": "recovered_modal_energy_from_string_velocity",
        "energy_fraction": _normalize_energy(recovered, valid),
        "valid": valid,
    }
    result = fingerprint_distance(force_fingerprint, synthetic_fingerprint)
    if result["sqrt_jensen_shannon_distance"] > 1e-6:
        raise RuntimeError(
            "Self-test failed: ideal velocity round-trip distance is "
            f"{result['sqrt_jensen_shannon_distance']:.6g}."
        )
    return result


def _fingerprint_json(fingerprint):
    result = {}
    for key, value in fingerprint.items():
        if isinstance(value, np.ndarray):
            if value.dtype == bool:
                result[key] = [bool(item) for item in value]
            else:
                result[key] = _as_float_list(value)
        else:
            result[key] = value
    return result


def write_outputs(output_dir, candidate, target=None, distance=None):
    output_dir = Path(output_dir)
    output_dir.mkdir(parents=True, exist_ok=True)

    payload = {
        "benchmark": {
            "name": "bBpiano hammer-only modal-energy fingerprint",
            "version": 1,
            "assumptions": [
                "ideal fixed-fixed string",
                "no dispersion",
                "no loss",
                "point-force hammer coupling",
                "mode frequencies n*f0",
            ],
        },
        "candidate": _fingerprint_json(candidate),
        "target": _fingerprint_json(target) if target else None,
        "distance": distance,
    }
    json_path = output_dir / "modal_fingerprint.json"
    json_path.write_text(
        json.dumps(payload, indent=2, ensure_ascii=False),
        encoding="utf-8",
    )

    csv_path = output_dir / "modal_fingerprint.csv"
    with csv_path.open("w", newline="", encoding="utf-8") as fp:
        fieldnames = [
            "mode",
            "frequency_hz",
            "candidate_fraction",
            "candidate_db",
            "target_fraction",
            "target_db",
            "delta_db",
        ]
        writer = csv.DictWriter(fp, fieldnames=fieldnames)
        writer.writeheader()
        for index, mode in enumerate(candidate["mode"]):
            candidate_fraction = float(candidate["energy_fraction"][index])
            target_fraction = (
                float(target["energy_fraction"][index])
                if target is not None
                else None
            )
            candidate_db = float(candidate["energy_db"][index])
            target_db = (
                float(target["energy_db"][index])
                if target is not None
                else None
            )
            writer.writerow({
                "mode": int(mode),
                "frequency_hz": float(candidate["frequency_hz"][index]),
                "candidate_fraction": candidate_fraction,
                "candidate_db": candidate_db,
                "target_fraction": target_fraction,
                "target_db": target_db,
                "delta_db": (
                    candidate_db - target_db
                    if target_db is not None
                    else None
                ),
            })

    plot_path = None
    if plt is not None:
        mode = np.asarray(candidate["mode"])
        figure, axes = plt.subplots(
            2 if target is not None else 1,
            1,
            figsize=(11, 7 if target is not None else 4),
            sharex=True,
        )
        if not isinstance(axes, np.ndarray):
            axes = np.array([axes])
        axes[0].plot(mode, candidate["energy_db"], "o-", label="candidate")
        if target is not None:
            axes[0].plot(mode, target["energy_db"], "o-", label="target")
        axes[0].set_ylabel("Relative modal energy (dB)")
        axes[0].set_ylim(-80.0, 3.0)
        axes[0].grid(True, alpha=0.3)
        axes[0].legend()

        if target is not None:
            delta_db = (
                np.asarray(candidate["energy_db"])
                - np.asarray(target["energy_db"])
            )
            axes[1].axhline(0.0, color="black", linewidth=1.0)
            axes[1].bar(mode, delta_db)
            axes[1].set_ylabel("Candidate - target (dB)")
            axes[1].grid(True, axis="y", alpha=0.3)

        axes[-1].set_xlabel("Ideal string mode n")
        figure.suptitle("Hammer modal-energy fingerprint")
        figure.tight_layout()
        plot_path = output_dir / "modal_fingerprint.png"
        figure.savefig(plot_path, dpi=160)
        plt.close(figure)

    return {
        "json": str(json_path),
        "csv": str(csv_path),
        "plot": str(plot_path) if plot_path else None,
    }


def build_parser():
    parser = argparse.ArgumentParser(
        description=(
            "Compare bBpiano hammer force using an undamped, "
            "dispersion-free ideal-string modal-energy fingerprint."
        )
    )
    source = parser.add_mutually_exclusive_group(required=True)
    source.add_argument("--force-log")
    source.add_argument(
        "--paired-manifest",
        help=(
            "CSV with velocity_m_s, force_log, reference_wav, and "
            "optional onset_ms columns."
        ),
    )
    target = parser.add_mutually_exclusive_group()
    target.add_argument("--target-force-log")
    target.add_argument("--reference-wav")
    parser.add_argument(
        "--audio-kind",
        choices=["microphone", "string-velocity", "string-displacement"],
        default="microphone",
    )
    parser.add_argument("--reference-onset-ms", type=float)
    parser.add_argument("--post-contact-ms", type=float, default=5.0)
    parser.add_argument("--window-ms", type=float, default=100.0)
    parser.add_argument("--baseline-velocity", type=float)
    parser.add_argument("--minimum-snr-db", type=float, default=10.0)
    parser.add_argument(
        "--frequency-mode",
        choices=["harmonic", "measured"],
        default="harmonic",
    )
    parser.add_argument("--frequency-reference-wav")
    parser.add_argument("--sample-rate", type=float, default=44100.0)
    parser.add_argument("--f0", type=float, default=261.626)
    parser.add_argument("--length-m", type=float, default=0.65)
    parser.add_argument("--linear-density", type=float, default=0.007)
    parser.add_argument("--strike-position", type=float, default=1.0 / 9.4)
    parser.add_argument("--pickup-position", type=float, default=6.0 / 7.0)
    parser.add_argument("--modes", type=int, default=32)
    parser.add_argument("--hammer-mass-kg", type=float, default=0.015)
    parser.add_argument("--impact-velocity", type=float, default=2.0)
    parser.add_argument("--output-dir", default="modal_benchmark")
    parser.add_argument("--self-test", action="store_true")
    return parser


def main():
    args = build_parser().parse_args()
    spec = StringSpec(
        sample_rate=args.sample_rate,
        f0=args.f0,
        length_m=args.length_m,
        linear_density_kg_m=args.linear_density,
        strike_position=args.strike_position,
        pickup_position=args.pickup_position,
        mode_count=args.modes,
        hammer_mass_kg=args.hammer_mass_kg,
        impact_velocity_m_s=args.impact_velocity,
    )

    if args.paired_manifest:
        if args.target_force_log or args.reference_wav or args.self_test:
            raise RuntimeError(
                "--paired-manifest cannot be combined with single-run "
                "target options or --self-test."
            )
        result = paired_velocity_benchmark(
            args.paired_manifest,
            spec,
            args.audio_kind,
            baseline_velocity=args.baseline_velocity,
            post_contact_ms=args.post_contact_ms,
            window_ms=args.window_ms,
            minimum_snr_db=args.minimum_snr_db,
            frequency_mode=args.frequency_mode,
        )
        outputs = write_paired_outputs(args.output_dir, result)
        summary = {
            key: value
            for key, value in result.items()
            if key not in {"rows"}
        }
        summary["outputs"] = outputs
        print(json.dumps(summary, indent=2, ensure_ascii=False))
        return

    modal_frequencies_hz = None
    if args.frequency_mode == "measured":
        frequency_reference = (
            args.frequency_reference_wav or args.reference_wav
        )
        if not frequency_reference:
            raise RuntimeError(
                "Measured frequency mode needs --frequency-reference-wav "
                "or --reference-wav."
            )
        modal_frequencies_hz = estimate_modal_frequencies(
            frequency_reference,
            spec,
            onset_ms=args.reference_onset_ms,
            post_contact_ms=args.post_contact_ms,
        )

    candidate = force_modal_fingerprint(
        args.force_log,
        spec,
        modal_frequencies_hz=modal_frequencies_hz,
    )

    self_test_result = None
    if args.self_test:
        if modal_frequencies_hz is not None:
            raise RuntimeError(
                "--self-test currently requires harmonic frequency mode."
            )
        self_test_result = self_test(candidate, spec)

    target = None
    if args.target_force_log:
        target = force_modal_fingerprint(
            args.target_force_log,
            spec,
            modal_frequencies_hz=modal_frequencies_hz,
        )
    elif args.reference_wav:
        target = audio_modal_fingerprint(
            args.reference_wav,
            spec,
            args.audio_kind,
            onset_ms=args.reference_onset_ms,
            post_contact_ms=args.post_contact_ms,
            window_ms=args.window_ms,
            modal_frequencies_hz=modal_frequencies_hz,
        )

    distance = (
        fingerprint_distance(candidate, target)
        if target is not None
        else None
    )
    outputs = write_outputs(
        args.output_dir,
        candidate,
        target=target,
        distance=distance,
    )

    summary = {
        "candidate_domain": candidate["domain"],
        "candidate_total_modal_energy_J": (
            candidate["metadata"]["total_modal_energy_J"]
        ),
        "candidate_incident_hammer_energy_J": (
            candidate["metadata"]["incident_hammer_energy_J"]
        ),
        "candidate_projection_energy_over_incident": (
            candidate["metadata"][
                "projection_energy_over_incident_hammer_energy"
            ]
        ),
        "closed_system_energy_budget_applicable": (
            candidate["metadata"]["closed_system_energy_budget_applicable"]
        ),
        "energy_identity_max_relative_error": (
            candidate["metadata"]["energy_identity_max_relative_error"]
        ),
        "self_test": self_test_result,
        "target_domain": target["domain"] if target else None,
        "distance": distance,
        "outputs": outputs,
    }
    print(json.dumps(summary, indent=2, ensure_ascii=False))

    if target is not None and target["domain"] == "microphone_partial_power_proxy":
        print(
            "\nWARNING: microphone comparison is a reproducible acoustic "
            "proxy, not an absolute hammer-only physical distance. "
            "Soundboard, microphone, room, and radiation transfer remain "
            "inside the target recording."
        )


if __name__ == "__main__":
    main()
