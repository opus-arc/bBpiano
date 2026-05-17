
#!/usr/bin/env python3


from pathlib import Path
import argparse
import re
import sys

import numpy as np
import soundfile as sf


NOTE_START = 21
NOTE_END = 108
NOTE_COUNT = 88

PRE_SILENCE_SEC = 2.0
POST_SILENCE_SEC = 2.0
HOLD_SEC = 1.0

INPUT_EXTENSIONS = [".wav", ".aiff", ".aif"]


def note_number_to_name(midi_n: int) -> str:
    names = ["C", "Cs", "D", "Ds", "E", "F", "Fs", "G", "Gs", "A", "As", "B"]
    note = names[midi_n % 12]
    octave = midi_n // 12 - 1
    return f"{note}{octave}"


def tail_duration_for_note(midi_n: int) -> float:
    if midi_n < 48:
        return 29.0
    if midi_n < 60:
        return 20.0
    if midi_n < 72:
        return 14.0
    if midi_n < 84:
        return 10.0
    return 7.0


def build_time_table():
    items = []
    t = PRE_SILENCE_SEC

    for midi_n in range(NOTE_START, NOTE_END + 1):
        start = t
        duration = HOLD_SEC + tail_duration_for_note(midi_n)
        end = start + duration

        items.append({
            "midi_n": midi_n,
            "name": note_number_to_name(midi_n),
            "start": start,
            "end": end,
        })

        t = end

    expected_total = t + POST_SILENCE_SEC
    return items, expected_total


def format_duration(seconds: float) -> str:
    total = round(seconds)
    return f"{total // 60}:{total % 60:02d}"


def parse_take_and_velocity(path: Path):
    match = re.search(r"take(\d+)_v(\d+)", path.stem)
    if not match:
        return None

    return int(match.group(1)), int(match.group(2))


def find_input_files(input_dir: Path):
    files = []
    for ext in INPUT_EXTENSIONS:
        files.extend(sorted(input_dir.glob(f"*{ext}")))
    return files


def split_one_file(
    input_path: Path,
    output_dir: Path,
    overwrite: bool,
    pre_margin: float,
    post_margin: float,
) -> int:
    parsed = parse_take_and_velocity(input_path)

    if parsed is None:
        print(f"WARNING: skip file, cannot parse take/velocity: {input_path}")
        return 0

    take_index, velocity = parsed

    audio, sample_rate = sf.read(input_path, always_2d=True, dtype="float32")
    total_frames = audio.shape[0]
    channels = audio.shape[1]
    audio_duration = total_frames / sample_rate

    time_table, expected_total = build_time_table()

    print()
    print(f"Input path: {input_path}")
    print(f"Sample rate: {sample_rate}")
    print(f"Channels: {channels}")
    print(f"Total duration: {format_duration(audio_duration)} ({audio_duration:.3f}s)")
    print(f"Expected timetable duration: {format_duration(expected_total)} ({expected_total:.3f}s)")

    if audio_duration + 1e-6 < expected_total:
        raise RuntimeError(
            f"Recording is shorter than timetable. "
            f"input={audio_duration:.3f}s expected={expected_total:.3f}s path={input_path}"
        )

    print("Length sufficient: yes")

    written_count = 0
    skipped_count = 0

    for item in time_table:
        midi_n = item["midi_n"]
        note_name = item["name"]

        start_sec = max(0.0, item["start"] - pre_margin)
        end_sec = min(audio_duration, item["end"] + post_margin)

        start_frame = int(round(start_sec * sample_rate))
        end_frame = int(round(end_sec * sample_rate))

        if end_frame <= start_frame:
            raise RuntimeError(f"Invalid slice range for note {midi_n} {note_name}")

        out_name = f"{note_name}_take{take_index:02d}_v{velocity}.wav"
        out_path = output_dir / out_name

        if out_path.exists() and not overwrite:
            skipped_count += 1
            continue

        segment = audio[start_frame:end_frame, :]

        sf.write(
            out_path,
            segment,
            sample_rate,
            subtype="FLOAT",
            format="WAV",
        )

        written_count += 1

    print(f"Written files: {written_count}")
    print(f"Skipped existing files: {skipped_count}")
    print(f"Expected note count: {NOTE_COUNT}")

    return written_count


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Split Pianoteq 88-key long recordings by fixed MIDI timetable."
    )

    parser.add_argument(
        "--input",
        type=Path,
        default=Path("AcousticLab/LossFilterLab/SingleNoteSamples/RawLong"),
        help="Directory containing long recordings.",
    )

    parser.add_argument(
        "--output",
        type=Path,
        default=Path("AcousticLab/LossFilterLab/SingleNoteSamples/Split"),
        help="Directory for split note WAV files.",
    )

    parser.add_argument(
        "--overwrite",
        action="store_true",
        help="Overwrite existing output files.",
    )

    parser.add_argument(
        "--pre-margin",
        type=float,
        default=0.05,
        help="Seconds to include before each note_on.",
    )

    parser.add_argument(
        "--post-margin",
        type=float,
        default=0.05,
        help="Seconds to include after each note slice.",
    )

    args = parser.parse_args()

    if not args.input.exists():
        raise FileNotFoundError(f"Input directory does not exist: {args.input}")

    args.output.mkdir(parents=True, exist_ok=True)

    input_files = find_input_files(args.input)

    if not input_files:
        print(f"No input files found in: {args.input}")
        return 1

    total_written = 0

    for input_path in input_files:
        total_written += split_one_file(
            input_path=input_path,
            output_dir=args.output,
            overwrite=args.overwrite,
            pre_margin=args.pre_margin,
            post_margin=args.post_margin,
        )

    print()
    print(f"Done. Total written files: {total_written}")
    print(f"Output directory: {args.output}")

    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except Exception as exc:
        print()
        print(f"ERROR: {exc}", file=sys.stderr)
        raise SystemExit(1)
