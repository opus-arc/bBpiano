#!/usr/bin/env python3

import sys
import json
import numpy as np
import soundfile as sf
import loristrck


def main():
    if len(sys.argv) != 2:
        print("Usage: python loris.py <audio.wav>", file=sys.stderr)
        sys.exit(1)

    wav_path = sys.argv[1]

    samples, sr = sf.read(wav_path)

    if samples.ndim > 1:
        samples = samples.mean(axis=1)

    samples = samples.astype(np.float64)

    partials = loristrck.analyze(
        samples,
        sr,
        resolution=60.0,
        ampfloor=-80.0
    )

    output = {
        "sample_rate": int(sr),
        "partial_count": len(partials),
        "exported_partial_count": 0,
        "column_format": [
            "time_seconds",
            "frequency_hz",
            "amplitude_linear",
            "phase_radians",
            "bandwidth"
        ],
        "partials": []
    }

    partial_summaries = []

    for idx, partial in enumerate(partials):
        time = partial[:, 0]
        freq = partial[:, 1]
        amp = partial[:, 2]
        phase = partial[:, 3]
        bandwidth = partial[:, 4]

        max_amp_index = int(np.argmax(amp))
        duration = float(time[-1] - time[0])
        max_amp = float(amp[max_amp_index])

        partial_summaries.append({
            "id": idx,
            "summary": {
                "mean_frequency_hz": float(np.mean(freq)),
                "frequency_at_max_amplitude_hz": float(freq[max_amp_index]),
                "start_frequency_hz": float(freq[0]),
                "end_frequency_hz": float(freq[-1]),
                "max_amplitude_linear": max_amp,
                "mean_amplitude_linear": float(np.mean(amp)),
                "start_time_seconds": float(time[0]),
                "end_time_seconds": float(time[-1]),
                "duration_seconds": duration,
                "breakpoint_count": int(len(partial)),
                "mean_bandwidth": float(np.mean(bandwidth))
            },
            "breakpoints": partial.tolist()
        })

    partial_summaries.sort(
        key=lambda p: p["summary"]["max_amplitude_linear"],
        reverse=True
    )

    output["partials"] = partial_summaries[:100]
    output["exported_partial_count"] = len(output["partials"])

    json.dump(output, sys.stdout, ensure_ascii=False, separators=(",", ":"))
    sys.stdout.write("\n")


if __name__ == "__main__":
    main()
