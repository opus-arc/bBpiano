from pathlib import Path
from mido import Message, MidiFile, MidiTrack, MetaMessage, bpm2tempo

OUTPUT_DIR = Path(
    "/Users/opusarc/XCodeProjects/bBpiano/AcousticLab/StringFilterLab/Midi"
)

TAKES = [
    ("string_analysis_take01_v50.mid", 50),
    ("string_analysis_take02_v65.mid", 65),
    ("string_analysis_take03_v80.mid", 80),
    ("string_analysis_take04_v95.mid", 95),
    ("string_analysis_take05_v110.mid", 110),
]

NOTE_START = 21   # A0
NOTE_END = 108    # C8
NOTE_COUNT = NOTE_END - NOTE_START + 1

TEMPO_BPM = 120
TICKS_PER_BEAT = 480
CHANNEL = 0

SUSTAIN_CC = 64
SUSTAIN_ON_VALUE = 127
SUSTAIN_OFF_VALUE = 0
ALL_SOUND_OFF_CC = 120
RESET_ALL_CONTROLLERS_CC = 121
ALL_NOTES_OFF_CC = 123

PRE_SILENCE_SEC = 3.0
POST_SILENCE_SEC = 3.0

# Lift all dampers before note-on so the recording captures free string decay,
# but leave a short lead-in so the pedal transition itself is not inside the
# useful analysis window.
PEDAL_LEAD_IN_SEC = 0.25

# After the key is released, keep sustain pedal down for a while. This gives a
# clean free-decay region without the damper touching the string.
POST_NOTE_SUSTAIN_SEC_LOW = 8.0
POST_NOTE_SUSTAIN_SEC_MID_LOW = 6.0
POST_NOTE_SUSTAIN_SEC_MID = 4.0
POST_NOTE_SUSTAIN_SEC_MID_HIGH = 2.5
POST_NOTE_SUSTAIN_SEC_HIGH = 1.5

# After pedal-up, allow the damper to settle before forcing MIDI cleanup.
DAMPER_SETTLE_SEC = 1.0

# Extra silence between notes after MIDI cleanup.
INTER_NOTE_SILENCE_SEC = 0.50

# Main key-down region. The recommended B-fitting window should be taken from
# early sustain, after the hammer transient, while the key is still held.
KEY_HOLD_SEC_LOW = 12.0
KEY_HOLD_SEC_MID_LOW = 9.0
KEY_HOLD_SEC_MID = 6.0
KEY_HOLD_SEC_MID_HIGH = 4.0
KEY_HOLD_SEC_HIGH = 3.0

# Useful for later analysis code: skip the hammer collision / attack transient.
RECOMMENDED_ANALYSIS_SKIP_SEC = 0.12


def key_hold_duration_for_note(midi_n: int) -> float:
    if midi_n < 48:
        return KEY_HOLD_SEC_LOW
    if midi_n < 60:
        return KEY_HOLD_SEC_MID_LOW
    if midi_n < 72:
        return KEY_HOLD_SEC_MID
    if midi_n < 84:
        return KEY_HOLD_SEC_MID_HIGH
    return KEY_HOLD_SEC_HIGH


def post_note_sustain_duration_for_note(midi_n: int) -> float:
    if midi_n < 48:
        return POST_NOTE_SUSTAIN_SEC_LOW
    if midi_n < 60:
        return POST_NOTE_SUSTAIN_SEC_MID_LOW
    if midi_n < 72:
        return POST_NOTE_SUSTAIN_SEC_MID
    if midi_n < 84:
        return POST_NOTE_SUSTAIN_SEC_MID_HIGH
    return POST_NOTE_SUSTAIN_SEC_HIGH


def seconds_to_ticks(seconds: float) -> int:
    beats = seconds / (60.0 / TEMPO_BPM)
    return round(beats * TICKS_PER_BEAT)


def midi_note_to_name(midi_n: int) -> str:
    names = ["C", "C#", "D", "D#", "E", "F",
             "F#", "G", "G#", "A", "A#", "B"]
    name = names[midi_n % 12]
    octave = (midi_n // 12) - 1
    return f"{name}{octave}"


def safe_note_name(midi_n: int) -> str:
    return midi_note_to_name(midi_n).replace("#", "s")


def format_duration(seconds: float) -> str:
    total = round(seconds)
    minutes = total // 60
    secs = total % 60
    return f"{minutes}:{secs:02d}"


def expected_duration_seconds() -> float:
    total = PRE_SILENCE_SEC + POST_SILENCE_SEC

    for midi_n in range(NOTE_START, NOTE_END + 1):
        total += PEDAL_LEAD_IN_SEC
        total += key_hold_duration_for_note(midi_n)
        total += post_note_sustain_duration_for_note(midi_n)
        total += DAMPER_SETTLE_SEC
        total += INTER_NOTE_SILENCE_SEC

    return total


def append_cc(track: MidiTrack, control: int, value: int, delta_seconds: float) -> None:
    track.append(Message(
        "control_change",
        control=control,
        value=value,
        channel=CHANNEL,
        time=seconds_to_ticks(delta_seconds),
    ))


def append_note_on(track: MidiTrack, midi_n: int, velocity: int, delta_seconds: float) -> None:
    track.append(Message(
        "note_on",
        note=midi_n,
        velocity=velocity,
        channel=CHANNEL,
        time=seconds_to_ticks(delta_seconds),
    ))


def append_note_off(track: MidiTrack, midi_n: int, delta_seconds: float) -> None:
    track.append(Message(
        "note_off",
        note=midi_n,
        velocity=0,
        channel=CHANNEL,
        time=seconds_to_ticks(delta_seconds),
    ))


def append_midi_cleanup(track: MidiTrack, delta_seconds: float) -> None:
    append_cc(track, ALL_NOTES_OFF_CC, 0, delta_seconds)
    append_cc(track, ALL_SOUND_OFF_CC, 0, 0.0)
    append_cc(track, RESET_ALL_CONTROLLERS_CC, 0, 0.0)


def create_midi_file(output_path: Path, velocity: int) -> None:
    mid = MidiFile(ticks_per_beat=TICKS_PER_BEAT)
    track = MidiTrack()
    mid.tracks.append(track)

    track.append(MetaMessage("set_tempo", tempo=bpm2tempo(TEMPO_BPM), time=0))

    pending_silence_sec = PRE_SILENCE_SEC

    for midi_n in range(NOTE_START, NOTE_END + 1):
        note_name = midi_note_to_name(midi_n)
        hold_sec = key_hold_duration_for_note(midi_n)
        sustain_sec = post_note_sustain_duration_for_note(midi_n)

        # Start from a known clean controller state before lifting the pedal.
        append_cc(track, SUSTAIN_CC, SUSTAIN_OFF_VALUE, pending_silence_sec)
        append_cc(track, RESET_ALL_CONTROLLERS_CC, 0, 0.0)

        # Pedal down before note-on: damper is already lifted when the string is struck.
        append_cc(track, SUSTAIN_CC, SUSTAIN_ON_VALUE, 0.0)
        append_note_on(track, midi_n, velocity, PEDAL_LEAD_IN_SEC)

        # Keep the key down for the main analysis region.
        append_note_off(track, midi_n, hold_sec)

        # Keep pedal down after note-off to capture free decay without immediate damping.
        append_cc(track, SUSTAIN_CC, SUSTAIN_OFF_VALUE, sustain_sec)

        # Let the damper return, then force MIDI cleanup and add isolation silence.
        append_midi_cleanup(track, DAMPER_SETTLE_SEC)
        pending_silence_sec = INTER_NOTE_SILENCE_SEC

        recommended_start = PEDAL_LEAD_IN_SEC + RECOMMENDED_ANALYSIS_SKIP_SEC
        recommended_end = PEDAL_LEAD_IN_SEC + min(hold_sec, 1.5)

        print(
            f"  note {midi_n:3d} {note_name:>3s} | "
            f"vel={velocity:3d} | "
            f"pedalLead={PEDAL_LEAD_IN_SEC:.2f}s | "
            f"hold={hold_sec:.1f}s | "
            f"freeSustain={sustain_sec:.1f}s | "
            f"damperSettle={DAMPER_SETTLE_SEC:.1f}s | "
            f"analysis≈{recommended_start:.2f}-{recommended_end:.2f}s after note block start"
        )

    track.append(MetaMessage("end_of_track", time=seconds_to_ticks(pending_silence_sec + POST_SILENCE_SEC)))
    mid.save(output_path)


def main() -> None:
    OUTPUT_DIR.mkdir(parents=True, exist_ok=True)

    per_file_duration = expected_duration_seconds()
    total_duration = per_file_duration * len(TAKES)

    print(f"Output directory: {OUTPUT_DIR}")
    print(f"Note range: {NOTE_START}-{NOTE_END} ({NOTE_COUNT} notes)")
    print(f"Per-file estimated duration: {format_duration(per_file_duration)}")
    print(f"Total estimated recording time: {format_duration(total_duration)}")
    print()

    for filename, velocity in TAKES:
        output_path = OUTPUT_DIR / filename

        print(f"Creating: {output_path}")
        create_midi_file(output_path, velocity)

        print(f"Output path: {output_path}")
        print(f"Velocity: {velocity}")
        print(f"Estimated duration: {format_duration(per_file_duration)}")
        print()


if __name__ == "__main__":
    main()
