#!/usr/bin/env python3
"""Deterministic, headless Pianoteq 8 renderer for HammerLab2.0."""

from __future__ import annotations

import argparse
import datetime as dt
import hashlib
import json
import pathlib
import subprocess
import sys
import time
import wave
from dataclasses import dataclass
from typing import Any, Mapping, Sequence


class PianoteqRenderError(RuntimeError):
    """Raised when Pianoteq cannot produce a valid requested render."""


def file_sha256(path: pathlib.Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as stream:
        for chunk in iter(lambda: stream.read(1 << 20), b""):
            digest.update(chunk)
    return digest.hexdigest()


def json_sha256(payload: Any) -> str:
    encoded = json.dumps(
        payload, sort_keys=True, separators=(",", ":"), ensure_ascii=False
    ).encode("utf-8")
    return hashlib.sha256(encoded).hexdigest()


def write_json_atomic(path: pathlib.Path, payload: Any) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    temporary = path.with_name(f".{path.name}.tmp")
    temporary.write_text(
        json.dumps(payload, indent=2, ensure_ascii=False) + "\n",
        encoding="utf-8",
    )
    temporary.replace(path)


def native_to_wine_z_path(path: pathlib.Path) -> str:
    """Map an absolute macOS path through Wine's standard Z: root mapping."""
    resolved = path.resolve()
    return "Z:" + str(resolved).replace("/", "\\")


def _parameter_assignment(parameter_id: int | str, value: Any) -> str:
    identifier = str(parameter_id).strip()
    if not identifier.isdigit() or int(identifier) <= 0:
        raise ValueError(f"invalid Pianoteq parameter id: {parameter_id!r}")
    if isinstance(value, bool):
        rendered = "On" if value else "Off"
    elif isinstance(value, float):
        rendered = format(value, ".12g")
    else:
        rendered = str(value)
    if not rendered:
        raise ValueError(f"empty value for Pianoteq parameter {identifier}")
    return f"{identifier}={rendered}"


@dataclass(frozen=True)
class PianoteqEngine:
    wine_path: pathlib.Path
    bottle: str
    executable_windows: str
    expected_version: str
    executable_native_path: pathlib.Path | None = None
    timeout_seconds: float = 120.0

    @classmethod
    def from_mapping(cls, payload: Mapping[str, Any]) -> "PianoteqEngine":
        required = ("wine_path", "bottle", "executable_windows", "expected_version")
        missing = [key for key in required if not payload.get(key)]
        if missing:
            raise ValueError(f"engine configuration missing: {', '.join(missing)}")
        engine = cls(
            wine_path=pathlib.Path(str(payload["wine_path"])).expanduser().resolve(),
            bottle=str(payload["bottle"]),
            executable_windows=str(payload["executable_windows"]),
            expected_version=str(payload["expected_version"]),
            executable_native_path=(
                pathlib.Path(str(payload["executable_native_path"]))
                .expanduser()
                .resolve()
                if payload.get("executable_native_path")
                else None
            ),
            timeout_seconds=float(payload.get("timeout_seconds", 120.0)),
        )
        if not engine.wine_path.is_file():
            raise ValueError(f"CrossOver wine launcher not found: {engine.wine_path}")
        if engine.timeout_seconds <= 0.0:
            raise ValueError("engine timeout_seconds must be positive")
        if (
            engine.executable_native_path is not None
            and not engine.executable_native_path.is_file()
        ):
            raise ValueError(
                f"Pianoteq executable not found: {engine.executable_native_path}"
            )
        return engine

    def base_command(self) -> list[str]:
        return [
            str(self.wine_path),
            "--bottle",
            self.bottle,
            "--no-gui",
            "--cx-app",
            self.executable_windows,
            "--",
        ]

    def query_version(self) -> str:
        process = subprocess.run(
            [*self.base_command(), "--version"],
            text=True,
            capture_output=True,
            check=False,
            timeout=self.timeout_seconds,
        )
        combined = (process.stdout + process.stderr).strip()
        if process.returncode != 0:
            raise PianoteqRenderError(
                f"Pianoteq version query failed ({process.returncode}): {combined}"
            )
        if self.expected_version not in combined:
            raise PianoteqRenderError(
                f"Pianoteq version mismatch: expected {self.expected_version!r}, "
                f"got {combined!r}"
            )
        return combined


def inspect_pcm_wav(
    path: pathlib.Path,
    *,
    expected_sample_rate_hz: int | None = None,
    expected_bit_depth: int | None = None,
    expected_channels: int | None = None,
    minimum_duration_seconds: float = 0.05,
    reject_clipping: bool = True,
) -> dict[str, Any]:
    """Inspect PCM content and return a JSON-serializable validation report."""
    errors: list[str] = []
    if not path.is_file():
        return {
            "valid": False,
            "errors": [f"missing WAV: {path}"],
            "path": str(path),
        }

    pcm_digest = hashlib.sha256()
    peak_integer = 0
    clipped_samples = 0
    nonzero_samples = 0

    try:
        with wave.open(str(path), "rb") as stream:
            compression = stream.getcomptype()
            sample_rate = stream.getframerate()
            channels = stream.getnchannels()
            sample_width = stream.getsampwidth()
            frame_count = stream.getnframes()
            bit_depth = sample_width * 8
            duration = frame_count / sample_rate if sample_rate else 0.0

            if compression != "NONE":
                errors.append(f"compressed WAV is unsupported: {compression}")
            if sample_width not in (1, 2, 3, 4):
                errors.append(f"unsupported PCM sample width: {sample_width}")
            if expected_sample_rate_hz is not None and sample_rate != expected_sample_rate_hz:
                errors.append(
                    f"sample rate mismatch: expected {expected_sample_rate_hz}, got {sample_rate}"
                )
            if expected_bit_depth is not None and bit_depth != expected_bit_depth:
                errors.append(
                    f"bit depth mismatch: expected {expected_bit_depth}, got {bit_depth}"
                )
            if expected_channels is not None and channels != expected_channels:
                errors.append(
                    f"channel mismatch: expected {expected_channels}, got {channels}"
                )
            if frame_count <= 0:
                errors.append("WAV contains no audio frames")
            if duration < minimum_duration_seconds:
                errors.append(
                    f"WAV is too short: {duration:.6f}s < {minimum_duration_seconds:.6f}s"
                )

            if sample_width in (1, 2, 3, 4):
                maximum = (1 << (bit_depth - 1)) - 1
                minimum = -(1 << (bit_depth - 1))
                while True:
                    raw = stream.readframes(1 << 15)
                    if not raw:
                        break
                    pcm_digest.update(raw)
                    for offset in range(0, len(raw), sample_width):
                        encoded = raw[offset : offset + sample_width]
                        if len(encoded) != sample_width:
                            errors.append("truncated PCM sample")
                            break
                        if sample_width == 1:
                            value = encoded[0] - 128
                        else:
                            value = int.from_bytes(encoded, "little", signed=True)
                        absolute = abs(value)
                        peak_integer = max(peak_integer, absolute)
                        if value != 0:
                            nonzero_samples += 1
                        if value == maximum or value == minimum:
                            clipped_samples += 1
    except (OSError, EOFError, wave.Error) as error:
        return {
            "valid": False,
            "errors": [f"invalid PCM WAV: {error}"],
            "path": str(path),
        }

    peak_normalized = (
        peak_integer / float(1 << (bit_depth - 1)) if bit_depth > 0 else 0.0
    )
    if nonzero_samples == 0:
        errors.append("WAV is digitally silent")
    if reject_clipping and clipped_samples:
        errors.append(f"WAV contains {clipped_samples} clipped PCM samples")

    return {
        "valid": not errors,
        "errors": errors,
        "path": str(path.resolve()),
        "file_sha256": file_sha256(path),
        "pcm_sha256": pcm_digest.hexdigest(),
        "sample_rate_hz": sample_rate,
        "bit_depth": bit_depth,
        "channels": channels,
        "frame_count": frame_count,
        "duration_seconds": duration,
        "peak_normalized": peak_normalized,
        "nonzero_samples": nonzero_samples,
        "clipped_samples": clipped_samples,
        "compression": compression,
    }


def build_render_command(
    engine: PianoteqEngine,
    *,
    preset: str,
    midi_path: pathlib.Path,
    output_path: pathlib.Path,
    parameters: Mapping[int | str, Any],
    sample_rate_hz: int,
    bit_depth: int,
    channels: int,
    dither: bool,
    normalize: bool,
    maximum_duration_seconds: float | None = None,
) -> list[str]:
    if not preset:
        raise ValueError("Pianoteq preset must not be empty")
    if not midi_path.is_file():
        raise ValueError(f"MIDI input not found: {midi_path}")
    if sample_rate_hz <= 0:
        raise ValueError("sample rate must be positive")
    if bit_depth not in (16, 24, 32):
        raise ValueError("bit depth must be 16, 24, or 32")
    if channels not in (1, 2):
        raise ValueError("only mono or stereo rendering is supported")

    command = [
        *engine.base_command(),
        "--headless",
        "--quiet",
        "--no-prefs",
        "--preset",
        preset,
    ]
    for parameter_id in sorted(parameters, key=lambda item: int(item)):
        command.extend(
            ["--set-param", _parameter_assignment(parameter_id, parameters[parameter_id])]
        )
    command.extend(
        [
            "--rate",
            str(sample_rate_hz),
            "--bit-depth",
            str(bit_depth),
            "--dither",
            "ON" if dither else "OFF",
        ]
    )
    if channels == 1:
        command.append("--mono")
    if normalize:
        command.append("--normalize")
    if maximum_duration_seconds is not None:
        if maximum_duration_seconds <= 0.0:
            raise ValueError("maximum render duration must be positive")
        command.extend(["--max-duration", format(maximum_duration_seconds, ".12g")])
    command.extend(
        [
            "--midi",
            native_to_wine_z_path(midi_path),
            "--wav",
            native_to_wine_z_path(output_path),
        ]
    )
    return command


def render_wav(
    engine: PianoteqEngine,
    *,
    preset: str,
    midi_path: pathlib.Path,
    output_path: pathlib.Path,
    parameters: Mapping[int | str, Any],
    sample_rate_hz: int = 44_100,
    bit_depth: int = 24,
    channels: int = 1,
    dither: bool = False,
    normalize: bool = False,
    maximum_duration_seconds: float | None = None,
    minimum_duration_seconds: float = 0.05,
    reject_clipping: bool = True,
    verified_version: str | None = None,
) -> dict[str, Any]:
    midi_path = midi_path.resolve()
    output_path = output_path.resolve()
    output_path.parent.mkdir(parents=True, exist_ok=True)
    if output_path.exists():
        output_path.unlink()

    version = verified_version or engine.query_version()
    command = build_render_command(
        engine,
        preset=preset,
        midi_path=midi_path,
        output_path=output_path,
        parameters=parameters,
        sample_rate_hz=sample_rate_hz,
        bit_depth=bit_depth,
        channels=channels,
        dither=dither,
        normalize=normalize,
        maximum_duration_seconds=maximum_duration_seconds,
    )
    started_at = dt.datetime.now(dt.timezone.utc)
    wall_start = time.monotonic()
    process = subprocess.run(
        command,
        text=True,
        capture_output=True,
        check=False,
        timeout=engine.timeout_seconds,
    )
    wall_seconds = time.monotonic() - wall_start
    validation = inspect_pcm_wav(
        output_path,
        expected_sample_rate_hz=sample_rate_hz,
        expected_bit_depth=bit_depth,
        expected_channels=channels,
        minimum_duration_seconds=minimum_duration_seconds,
        reject_clipping=reject_clipping,
    )
    record = {
        "schema_version": 1,
        "started_at_utc": started_at.isoformat(),
        "wall_seconds": wall_seconds,
        "pianoteq_version": version,
        "wine_path": str(engine.wine_path),
        "bottle": engine.bottle,
        "executable_windows": engine.executable_windows,
        "executable_native_path": (
            str(engine.executable_native_path)
            if engine.executable_native_path is not None
            else None
        ),
        "executable_sha256": (
            file_sha256(engine.executable_native_path)
            if engine.executable_native_path is not None
            else None
        ),
        "preset": preset,
        "parameters": {
            str(key): parameters[key] for key in sorted(parameters, key=lambda item: int(item))
        },
        "midi_path": str(midi_path),
        "midi_sha256": file_sha256(midi_path),
        "output_path": str(output_path),
        "sample_rate_hz": sample_rate_hz,
        "bit_depth": bit_depth,
        "channels": channels,
        "dither": dither,
        "normalize": normalize,
        "command": command,
        "returncode": process.returncode,
        "stdout": process.stdout,
        "stderr": process.stderr,
        "validation": validation,
    }
    if process.returncode != 0:
        raise PianoteqRenderError(
            f"Pianoteq render failed ({process.returncode}): "
            f"{(process.stdout + process.stderr).strip()}"
        )
    if not validation["valid"]:
        raise PianoteqRenderError(
            "Pianoteq produced an invalid WAV: " + "; ".join(validation["errors"])
        )
    return record


def _parse_assignment(value: str) -> tuple[str, str]:
    if "=" not in value:
        raise argparse.ArgumentTypeError("parameter must use ID=VALUE")
    identifier, parameter_value = value.split("=", 1)
    try:
        _parameter_assignment(identifier, parameter_value)
    except ValueError as error:
        raise argparse.ArgumentTypeError(str(error)) from error
    return identifier, parameter_value


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--engine-config", required=True, type=pathlib.Path)
    parser.add_argument("--preset", required=True)
    parser.add_argument("--midi", required=True, type=pathlib.Path)
    parser.add_argument("--output", required=True, type=pathlib.Path)
    parser.add_argument("--record", type=pathlib.Path)
    parser.add_argument("--parameter", action="append", default=[], type=_parse_assignment)
    parser.add_argument("--sample-rate", type=int, default=44_100)
    parser.add_argument("--bit-depth", type=int, default=24)
    parser.add_argument("--channels", type=int, choices=(1, 2), default=1)
    parser.add_argument("--dither", action="store_true")
    parser.add_argument("--normalize", action="store_true")
    args = parser.parse_args()

    try:
        engine_payload = json.loads(args.engine_config.read_text(encoding="utf-8"))
        engine = PianoteqEngine.from_mapping(engine_payload)
        parameters = dict(args.parameter)
        record = render_wav(
            engine,
            preset=args.preset,
            midi_path=args.midi,
            output_path=args.output,
            parameters=parameters,
            sample_rate_hz=args.sample_rate,
            bit_depth=args.bit_depth,
            channels=args.channels,
            dither=args.dither,
            normalize=args.normalize,
        )
        record_path = args.record or args.output.with_suffix(".render.json")
        write_json_atomic(record_path, record)
        print(
            f"PASS: {args.output} "
            f"sha256={record['validation']['file_sha256']} "
            f"peak={record['validation']['peak_normalized']:.6f}"
        )
        return 0
    except (OSError, ValueError, json.JSONDecodeError, PianoteqRenderError) as error:
        print(f"pianoteq8_renderer: {error}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
