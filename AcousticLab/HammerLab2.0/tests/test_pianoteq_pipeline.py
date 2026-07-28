#!/usr/bin/env python3

import json
import pathlib
import struct
import sys
import tempfile
import unittest
import wave


PROJECT = pathlib.Path(__file__).resolve().parents[1]
TOOLS = PROJECT / "tools"
sys.path.insert(0, str(TOOLS))

from pianoteq8_renderer import (  # noqa: E402
    PianoteqEngine,
    build_render_command,
    inspect_pcm_wav,
    native_to_wine_z_path,
)
from run_study import build_heartbeat_report, expand_heartbeat_cases  # noqa: E402


class PianoteqPipelineTests(unittest.TestCase):
    def setUp(self) -> None:
        self.study = json.loads(
            (PROJECT / "configs/studies/pt843_c4_hardness_heartbeat.json").read_text(
                encoding="utf-8"
            )
        )

    def test_heartbeat_expands_to_27_unique_cases(self) -> None:
        cases = expand_heartbeat_cases(self.study)
        self.assertEqual(len(cases), 27)
        self.assertEqual(len({case["case_id"] for case in cases}), 27)
        self.assertEqual(
            sorted({case["parameter_value"] for case in cases}), [0.8, 1.0, 1.2]
        )

    def test_native_to_wine_path(self) -> None:
        converted = native_to_wine_z_path(pathlib.Path("/private/tmp/example.mid"))
        self.assertEqual(converted, r"Z:\private\tmp\example.mid")

    def test_render_command_is_headless_and_sets_parameters(self) -> None:
        with tempfile.TemporaryDirectory() as temporary:
            root = pathlib.Path(temporary)
            midi = root / "probe.mid"
            midi.write_bytes(b"MThd")
            output = root / "output.wav"
            engine = PianoteqEngine(
                wine_path=pathlib.Path("/bin/false"),
                bottle="test",
                executable_windows=r"C:\Pianoteq 8.exe",
                expected_version="8.4.3",
            )
            command = build_render_command(
                engine,
                preset="Test",
                midi_path=midi,
                output_path=output,
                parameters={"30": 0.8, "31": 1.0, "210": "Off"},
                sample_rate_hz=44_100,
                bit_depth=24,
                channels=1,
                dither=False,
                normalize=False,
            )
            self.assertIn("--headless", command)
            self.assertIn("30=0.8", command)
            self.assertIn("31=1", command)
            self.assertIn("210=Off", command)
            self.assertIn("--mono", command)
            self.assertNotIn("--normalize", command)

    def test_wav_inspection_accepts_valid_24_bit_pcm(self) -> None:
        with tempfile.TemporaryDirectory() as temporary:
            path = pathlib.Path(temporary) / "valid.wav"
            samples = [0, 1000, -1000, 250_000, -250_000] * 10_000
            raw = bytearray()
            for value in samples:
                raw.extend(int(value).to_bytes(3, "little", signed=True))
            with wave.open(str(path), "wb") as stream:
                stream.setnchannels(1)
                stream.setsampwidth(3)
                stream.setframerate(44_100)
                stream.writeframes(bytes(raw))
            report = inspect_pcm_wav(
                path,
                expected_sample_rate_hz=44_100,
                expected_bit_depth=24,
                expected_channels=1,
                minimum_duration_seconds=1.0,
            )
            self.assertTrue(report["valid"], report["errors"])
            self.assertGreater(report["peak_normalized"], 0.0)
            self.assertEqual(report["clipped_samples"], 0)

    def test_wav_inspection_rejects_silence_and_wrong_rate(self) -> None:
        with tempfile.TemporaryDirectory() as temporary:
            path = pathlib.Path(temporary) / "silent.wav"
            with wave.open(str(path), "wb") as stream:
                stream.setnchannels(1)
                stream.setsampwidth(2)
                stream.setframerate(48_000)
                stream.writeframes(struct.pack("<h", 0) * 48_000)
            report = inspect_pcm_wav(
                path,
                expected_sample_rate_hz=44_100,
                expected_bit_depth=16,
                expected_channels=1,
            )
            self.assertFalse(report["valid"])
            self.assertTrue(any("sample rate mismatch" in item for item in report["errors"]))
            self.assertTrue(any("silent" in item for item in report["errors"]))

    def test_heartbeat_report_checks_repeatability_and_sensitivity(self) -> None:
        with tempfile.TemporaryDirectory() as temporary:
            root = pathlib.Path(temporary)
            records = []
            for delta_index, perturbation in enumerate((-0.2, 0.0, 0.2)):
                for repeat in range(1, 4):
                    path = root / f"{delta_index}_{repeat}.wav"
                    amplitude = 8_000 + delta_index * 2_000
                    # One-LSB repeat variation must be accepted even though hashes differ.
                    samples = [
                        amplitude + (1 if repeat == 2 and index == 0 else 0)
                        if index % 2 == 0
                        else -amplitude
                        for index in range(2_000)
                    ]
                    with wave.open(str(path), "wb") as stream:
                        stream.setnchannels(1)
                        stream.setsampwidth(2)
                        stream.setframerate(44_100)
                        stream.writeframes(
                            b"".join(
                                struct.pack("<h", sample) for sample in samples
                            )
                        )
                    records.append(
                        {
                            "case": {
                                "midi_note": 60,
                                "midi_velocity": 70,
                                "control_name": "mezzo",
                                "perturbation": perturbation,
                                "repeat": repeat,
                            },
                            "output_path": str(path),
                            "validation": {
                                "pcm_sha256": f"{delta_index}-{repeat}"
                            },
                        }
                    )
            report = build_heartbeat_report(records)
            json.dumps(report, allow_nan=False)
            self.assertTrue(report["pass"])
            self.assertTrue(report["all_repeats_within_tolerance"])
            self.assertTrue(report["all_hardness_effects_above_noise_floor"])
            self.assertTrue(
                all(
                    not item["pcm_identical"]
                    for item in report["repeatability"]
                )
            )


if __name__ == "__main__":
    unittest.main()
