from pathlib import Path
from mido import Message, MidiFile, MidiTrack, MetaMessage, bpm2tempo

OUTPUT_DIR = Path(
    "/Users/opusarc/XCodeProjects/bBpiano/AcousticLab/LossFilterLab/Samples/Pianoteq 9/SingleNoteSamples"
)

TAKES = [
    ("loss_filter_take01_v50.mid", 50),
    ("loss_filter_take02_v65.mid", 65),
    ("loss_filter_take03_v80.mid", 80),
    ("loss_filter_take04_v95.mid", 95),
    ("loss_filter_take05_v110.mid", 110),
]

NOTE_START = 21   # A0
NOTE_END = 108    # C8
NOTE_COUNT = NOTE_END - NOTE_START + 1

TEMPO_BPM = 120
TICKS_PER_BEAT = 480
CHANNEL = 0

PRE_SILENCE_SEC = 2.0
POST_SILENCE_SEC = 2.0
SAFETY_DURATION_MULTIPLIER = 1.5


# Keep each key pressed long enough to observe the string's natural decay
# while that note's damper is lifted. This hold section is the main region
# used for partial decay / loss-filter fitting.
def hold_duration_for_note(midi_n: int) -> float:
    if midi_n < 48:
        base = 10.0
    elif midi_n < 60:
        base = 8.0
    elif midi_n < 72:
        base = 6.0
    elif midi_n < 84:
        base = 4.0
    else:
        base = 3.0

    return base * SAFETY_DURATION_MULTIPLIER


def tail_duration_for_note(midi_n: int) -> float:
    # After note_off, the damper returns. The tail is only a short buffer
    # for recording the damper return/release behavior, not the main region
    # used for string loss fitting.
    if midi_n < 48:
        return 5.0
    if midi_n < 60:
        return 4.0
    if midi_n < 72:
        return 3.0
    if midi_n < 84:
        return 2.0
    return 1.5


def seconds_to_ticks(seconds: float) -> int:
    # 120 BPM = 0.5 seconds per beat
    beats = seconds / (60.0 / TEMPO_BPM)
    return round(beats * TICKS_PER_BEAT)


def midi_note_to_name(midi_n: int) -> str:
    """
    MIDI note name conversion.

    A0 = MIDI 21
    C4 = MIDI 60
    C8 = MIDI 108
    """
    names = ["C", "C#", "D", "D#", "E", "F",
             "F#", "G", "G#", "A", "A#", "B"]
    name = names[midi_n % 12]
    octave = (midi_n // 12) - 1
    return f"{name}{octave}"


def format_duration(seconds: float) -> str:
    total = round(seconds)
    minutes = total // 60
    secs = total % 60
    return f"{minutes}:{secs:02d}"


def expected_duration_seconds() -> float:
    total = PRE_SILENCE_SEC + POST_SILENCE_SEC

    for midi_n in range(NOTE_START, NOTE_END + 1):
        total += hold_duration_for_note(midi_n)
        total += tail_duration_for_note(midi_n)

    return total


def create_midi_file(output_path: Path, velocity: int) -> None:
    mid = MidiFile(ticks_per_beat=TICKS_PER_BEAT)
    track = MidiTrack()
    mid.tracks.append(track)

    track.append(MetaMessage("set_tempo", tempo=bpm2tempo(TEMPO_BPM), time=0))

    # Opening silence
    pending_delta = seconds_to_ticks(PRE_SILENCE_SEC)

    for midi_n in range(NOTE_START, NOTE_END + 1):
        note_name = midi_note_to_name(midi_n)

        track.append(Message(
            "note_on",
            note=midi_n,
            velocity=velocity,
            channel=CHANNEL,
            time=pending_delta,
        ))

        hold_sec = hold_duration_for_note(midi_n)

        track.append(Message(
            "note_off",
            note=midi_n,
            velocity=0,
            channel=CHANNEL,
            time=seconds_to_ticks(hold_sec),
        ))

        tail_sec = tail_duration_for_note(midi_n)
        pending_delta = seconds_to_ticks(tail_sec)

        print(
            f"  note {midi_n:3d} {note_name:>3s} | "
            f"hold={hold_sec:.1f}s | tail={tail_sec:.1f}s"
        )

    # Final silence after last note tail
    pending_delta += seconds_to_ticks(POST_SILENCE_SEC)
    track.append(MetaMessage("end_of_track", time=pending_delta))

    mid.save(output_path)


def main() -> None:
    OUTPUT_DIR.mkdir(parents=True, exist_ok=True)

    per_file_duration = expected_duration_seconds()
    total_duration = per_file_duration * len(TAKES)

    print(f"Output directory: {OUTPUT_DIR}")
    print()

    for filename, velocity in TAKES:
        output_path = OUTPUT_DIR / filename

        print(f"Creating: {output_path}")
        print(f"Velocity: {velocity}")

        create_midi_file(output_path, velocity)

        print(f"Output path: {output_path}")
        print(f"Velocity: {velocity}")
        print(f"Note count: {NOTE_COUNT}")
        print(f"Estimated duration: {format_duration(per_file_duration)}")
        print()

    print(f"Total estimated recording time: {format_duration(total_duration)}")


if __name__ == "__main__":
    main()
