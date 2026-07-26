#!/usr/bin/env python3
"""Compare candidate audio with an owned Pianoteq 8 reference capture."""

import argparse
import json
import pathlib
import wave

import numpy as np


def decode_pcm(raw: bytes, width: int) -> np.ndarray:
    if width == 1:
        return (np.frombuffer(raw, dtype=np.uint8).astype(np.float64) - 128.0) / 128.0
    if width == 2:
        return np.frombuffer(raw, dtype="<i2").astype(np.float64) / 32768.0
    if width == 3:
        triples = np.frombuffer(raw, dtype=np.uint8).reshape(-1, 3).astype(np.int32)
        values = triples[:, 0] | (triples[:, 1] << 8) | (triples[:, 2] << 16)
        values = np.where(values & 0x800000, values - 0x1000000, values)
        return values.astype(np.float64) / 8388608.0
    if width == 4:
        return np.frombuffer(raw, dtype="<i4").astype(np.float64) / 2147483648.0
    raise ValueError(f"unsupported PCM sample width: {width}")


def load_wav(path: pathlib.Path) -> tuple[int, np.ndarray]:
    with wave.open(str(path), "rb") as stream:
        if stream.getcomptype() != "NONE":
            raise ValueError(f"{path}: compressed WAV is not supported")
        rate, channels, width = stream.getframerate(), stream.getnchannels(), stream.getsampwidth()
        signal = decode_pcm(stream.readframes(stream.getnframes()), width)
    return rate, signal.reshape(-1, channels).mean(axis=1)


def onset(signal: np.ndarray, rate: int) -> int:
    window = max(1, round(rate * 0.0005))
    envelope = np.convolve(np.abs(signal), np.ones(window) / window, mode="same")
    noise = np.percentile(envelope[: max(window, min(len(envelope), rate // 5))], 90)
    threshold = max(noise * 8.0, envelope.max() * 1.0e-4)
    indices = np.flatnonzero(envelope >= threshold)
    return int(indices[0]) if len(indices) else 0


def spectrum_metrics(reference: np.ndarray, candidate: np.ndarray, rate: int, start_ms: float, end_ms: float) -> dict:
    start, end = round(start_ms * rate / 1000.0), round(end_ms * rate / 1000.0)
    length = min(end - start, len(reference) - start, len(candidate) - start)
    if length < 32:
        raise ValueError("audio is too short for requested analysis windows")
    window = np.hanning(length)
    ref_spectrum = np.abs(np.fft.rfft(reference[start:start + length] * window)) + 1.0e-12
    can_spectrum = np.abs(np.fft.rfft(candidate[start:start + length] * window)) + 1.0e-12
    frequencies = np.fft.rfftfreq(length, 1.0 / rate)
    ref_db = 20.0 * np.log10(ref_spectrum / ref_spectrum.max())
    can_db = 20.0 * np.log10(can_spectrum / can_spectrum.max())
    audible = frequencies <= min(20000.0, rate / 2.0)
    ref_power, can_power = ref_spectrum ** 2, can_spectrum ** 2
    ref_centroid = float(np.sum(frequencies * ref_power) / np.sum(ref_power))
    can_centroid = float(np.sum(frequencies * can_power) / np.sum(can_power))
    high = frequencies >= 5000.0
    return {
        "start_ms": start_ms,
        "end_ms": end_ms,
        "log_spectral_rmse_db": float(np.sqrt(np.mean((can_db[audible] - ref_db[audible]) ** 2))),
        "reference_centroid_hz": ref_centroid,
        "candidate_centroid_hz": can_centroid,
        "centroid_error_hz": can_centroid - ref_centroid,
        "reference_high_frequency_ratio": float(np.sum(ref_power[high]) / np.sum(ref_power)),
        "candidate_high_frequency_ratio": float(np.sum(can_power[high]) / np.sum(can_power)),
    }


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("reference", type=pathlib.Path, help="owned Pianoteq 8 WAV")
    parser.add_argument("candidate", type=pathlib.Path)
    parser.add_argument("--output", type=pathlib.Path)
    args = parser.parse_args()
    ref_rate, reference = load_wav(args.reference)
    can_rate, candidate = load_wav(args.candidate)
    if ref_rate != can_rate:
        raise ValueError("sample rates differ; resample before evaluation")
    ref_onset, can_onset = onset(reference, ref_rate), onset(candidate, can_rate)
    reference, candidate = reference[ref_onset:], candidate[can_onset:]
    common = min(len(reference), len(candidate))
    reference, candidate = reference[:common], candidate[:common]
    peak_ref, peak_can = float(np.max(np.abs(reference))), float(np.max(np.abs(candidate)))
    normalized_ref = reference / max(peak_ref, 1.0e-12)
    normalized_can = candidate / max(peak_can, 1.0e-12)
    report = {
        "schema_version": 1,
        "reference_engine": "Pianoteq 8",
        "reference": str(args.reference.resolve()),
        "candidate": str(args.candidate.resolve()),
        "sample_rate_hz": ref_rate,
        "reference_onset_ms": 1000.0 * ref_onset / ref_rate,
        "candidate_onset_ms": 1000.0 * can_onset / can_rate,
        "onset_offset_ms": 1000.0 * (can_onset - ref_onset) / ref_rate,
        "reference_peak": peak_ref,
        "candidate_peak": peak_can,
        "peak_level_error_db": float(20.0 * np.log10(max(peak_can, 1.0e-12) / max(peak_ref, 1.0e-12))),
        "normalized_waveform_rmse_0_20ms": float(np.sqrt(np.mean((normalized_can[: round(ref_rate * .02)] - normalized_ref[: round(ref_rate * .02)]) ** 2))),
        "windows": [
            spectrum_metrics(normalized_ref, normalized_can, ref_rate, 0.0, 5.0),
            spectrum_metrics(normalized_ref, normalized_can, ref_rate, 5.0, 20.0),
            spectrum_metrics(normalized_ref, normalized_can, ref_rate, 20.0, 100.0),
        ],
    }
    payload = json.dumps(report, indent=2) + "\n"
    if args.output:
        args.output.parent.mkdir(parents=True, exist_ok=True)
        args.output.write_text(payload)
    print(payload, end="")


if __name__ == "__main__":
    main()
