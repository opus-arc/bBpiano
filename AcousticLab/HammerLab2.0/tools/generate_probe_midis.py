#!/usr/bin/env python3
"""Generate deterministic format-0 MIDI probes for the Pianoteq 8 matrix."""

import argparse
import csv
import pathlib
import struct


TICKS_PER_QUARTER = 960
TEMPO_US_PER_QUARTER = 1_000_000


def variable_length(value: int) -> bytes:
    buffer = value & 0x7F
    encoded = bytearray()
    while value >> 7:
        value >>= 7
        buffer <<= 8
        buffer |= (value & 0x7F) | 0x80
    while True:
        encoded.append(buffer & 0xFF)
        if buffer & 0x80:
            buffer >>= 8
        else:
            break
    return bytes(encoded)


def midi_file(note: int, velocity: int, pre_roll_seconds: float, hold_seconds: float, tail_seconds: float) -> bytes:
    pre_roll_ticks = round(pre_roll_seconds * TICKS_PER_QUARTER)
    hold_ticks = round(hold_seconds * TICKS_PER_QUARTER)
    tail_ticks = round(tail_seconds * TICKS_PER_QUARTER)
    track = bytearray()
    track += b"\x00\xff\x51\x03" + TEMPO_US_PER_QUARTER.to_bytes(3, "big")
    track += variable_length(pre_roll_ticks) + b"\x90" + bytes((note, velocity))
    track += variable_length(hold_ticks) + b"\x80" + bytes((note, 0))
    track += variable_length(tail_ticks) + b"\xff\x2f\x00"
    header = b"MThd" + struct.pack(">IHHH", 6, 0, 1, TICKS_PER_QUARTER)
    return header + b"MTrk" + struct.pack(">I", len(track)) + track


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("matrix", type=pathlib.Path)
    parser.add_argument("output", type=pathlib.Path)
    args = parser.parse_args()
    args.output.mkdir(parents=True, exist_ok=True)
    with args.matrix.open(newline="", encoding="utf-8") as stream:
        rows = list(csv.DictReader(stream))
    for row in rows:
        payload = midi_file(
            int(row["midi_note"]), int(row["midi_velocity"]),
            float(row["pre_roll_seconds"]), float(row["hold_seconds"]),
            float(row["tail_seconds"]),
        )
        (args.output / f"{row['case_id']}.mid").write_bytes(payload)
    print(f"wrote {len(rows)} Pianoteq 8 probes to {args.output}")


if __name__ == "__main__":
    main()
