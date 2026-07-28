#!/usr/bin/env python3
"""
Hammer-string force curve analyzer

Usage:
    python hammer_analyzer.py path/to/F.txt

Input format:
    Time: 1.02345, F: 12.34
or
    Time: 1.02345 F: 12.34

Outputs:
    - peak force
    - peak time
    - normalized peak position
    - contact duration (threshold based)
    - rise/fall symmetry indicators
    - force impulse
    - saved CSV and PNG
"""

import sys
import re
import csv
from pathlib import Path

import numpy as np

try:
    import matplotlib.pyplot as plt
except ImportError:
    plt = None


def trapezoid(y, x, axis=-1):
    """NumPy 1.x/2.x compatible trapezoidal integration."""
    implementation = getattr(np, "trapezoid", np.trapz)
    return implementation(y, x, axis=axis)


def load_force_file(path):
    t = []
    f = []

    pattern = re.compile(
        r"Time:\s*([0-9eE+.\-]+).*?F:\s*([0-9eE+.\-]+)"
    )

    with open(path, "r", encoding="utf-8") as fp:
        for line in fp:
            m = pattern.search(line)
            if m:
                t.append(float(m.group(1)))
                f.append(float(m.group(2)))

    if len(t) < 5:
        raise RuntimeError("No valid force samples found.")

    return np.array(t), np.array(f)


def select_first_contact(t, f):
    """
    Force logs contain only F > 0 samples.  A gap larger than the normal
    sample interval therefore marks separation followed by a re-contact.
    Analyze the first continuous contact instead of merging all contacts.
    """
    delta_t = np.diff(t)
    positive_delta_t = delta_t[delta_t > 0]

    if not len(positive_delta_t):
        return t, f, 1

    sample_interval = np.median(positive_delta_t)
    breaks = np.where(delta_t > sample_interval * 2.5)[0]

    if not len(breaks):
        return t, f, 1

    first_end = breaks[0] + 1
    return t[:first_end], f[:first_end], len(breaks) + 1


def analyze(t, f):

    peak_idx = np.argmax(f)
    peak_force = f[peak_idx]
    peak_time = t[peak_idx]

    duration = t[-1] - t[0]
    peak_ratio = (
        (peak_time - t[0]) / duration
        if duration > 0
        else 0
    )

    # Contact duration:
    # 5% of maximum force is a common engineering threshold
    threshold = peak_force * 0.05

    active = np.where(f > threshold)[0]

    if len(active):
        contact_duration = t[active[-1]] - t[active[0]]
    else:
        contact_duration = 0

    half_max = np.where(f > peak_force * 0.5)[0]
    if len(half_max):
        half_max_duration = (
            t[half_max[-1]] - t[half_max[0]]
        )
    else:
        half_max_duration = 0

    # rise time to peak
    rise_time = peak_time - t[0]
    fall_time = t[-1] - peak_time

    # energy-like impulse
    impulse = trapezoid(f, t)
    loading_impulse = trapezoid(
        f[:peak_idx + 1],
        t[:peak_idx + 1]
    )
    unloading_impulse = trapezoid(
        f[peak_idx:],
        t[peak_idx:]
    )

    force_centroid_time = (
        trapezoid(t * f, t) / impulse
        if impulse > 0
        else t[0]
    )
    force_centroid_ratio = (
        (force_centroid_time - t[0]) / duration
        if duration > 0
        else 0
    )

    # slope before peak
    pre = np.diff(f[:peak_idx+1]) / np.diff(t[:peak_idx+1])
    post = np.diff(f[peak_idx:]) / np.diff(t[peak_idx:])

    result = {
        "Fmax_N": peak_force,
        "t_peak_s": peak_time,
        "peak_position_ratio": peak_ratio,
        "contact_span_s": duration,
        "contact_duration_s": contact_duration,
        "half_max_duration_s": half_max_duration,
        "rise_time_s": rise_time,
        "fall_time_s": fall_time,
        "rise_fall_ratio": (
            rise_time / fall_time
            if fall_time > 0
            else float("inf")
        ),
        "impulse_Ns": impulse,
        "loading_impulse_fraction": (
            loading_impulse / impulse
            if impulse > 0
            else 0
        ),
        "unloading_impulse_fraction": (
            unloading_impulse / impulse
            if impulse > 0
            else 0
        ),
        "force_centroid_ratio": force_centroid_ratio,
        "sample_count": len(t),
        "max_rise_slope": np.max(pre) if len(pre) else 0,
        "max_fall_slope": np.min(post) if len(post) else 0
    }

    return result


def save_csv(t, f, path):
    out = Path(path).with_suffix(".csv")

    with open(out, "w", newline="") as fp:
        writer = csv.writer(fp)
        writer.writerow(["time", "force"])
        writer.writerows(zip(t, f))

    return out


def plot(t, f, path):

    if plt is None:
        return

    plt.figure(figsize=(8,4))
    plt.plot(t, f)
    plt.xlabel("Time (s)")
    plt.ylabel("Force (N)")
    plt.title("Hammer-string contact force")
    plt.grid(True)

    out = Path(path).with_suffix(".png")
    plt.savefig(out, dpi=150)
    plt.close()

    return out


def main():

    if len(sys.argv) < 2:
        print("Usage: python hammer_analyzer.py force_file.txt")
        return

    filename = sys.argv[1]

    all_t, all_f = load_force_file(filename)
    t, f, contact_event_count = select_first_contact(all_t, all_f)

    result = analyze(t, f)
    result["contact_event_count"] = contact_event_count

    print("\n=== Hammer Contact Analysis ===")

    for k,v in result.items():
        print(f"{k:25s}: {v:.8g}")

    save_csv(t,f,filename)
    plot(t,f,filename)


if __name__ == "__main__":
    main()
