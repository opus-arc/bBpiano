#!/usr/bin/env python3
import argparse
import csv
import math
from pathlib import Path


def read_csv(path):
    with open(path, newline="") as f:
        return [
            {
                "time_ms": float(row["time_sec"]) * 1000.0,
                "F": float(row["F"]),
                "sample": float(row["sample"]),
            }
            for row in csv.DictReader(f)
        ]


def clip_rows(rows, x_max_ms):
    if x_max_ms is None:
        return rows
    return [row for row in rows if row["time_ms"] <= x_max_ms]


def padded_range(values, include_zero=False):
    lo = min(values)
    hi = max(values)
    if include_zero:
        lo = min(lo, 0.0)
        hi = max(hi, 0.0)
    if not math.isfinite(lo) or not math.isfinite(hi):
        return -1.0, 1.0
    if abs(hi - lo) < 1e-12:
        pad = max(abs(hi) * 0.12, 1e-9)
        return lo - pad, hi + pad
    pad = (hi - lo) * 0.10
    return lo - pad, hi + pad


def polyline(rows, key, x_range, y_range, x0, y0, width, height):
    xmin, xmax = x_range
    ymin, ymax = y_range
    coords = []
    for row in rows:
        x = x0 + (row["time_ms"] - xmin) / (xmax - xmin) * width
        y = y0 + height - (row[key] - ymin) / (ymax - ymin) * height
        coords.append(f"{x:.2f},{y:.2f}")
    return " ".join(coords)


def draw_panel(chunks, normal, perform, key, label, y_range, x_range, x0, y0, width, height):
    chunks.append(f'<text class="panel-title" x="{x0}" y="{y0 - 34}">{label}</text>')
    chunks.append(f'<rect x="{x0}" y="{y0}" width="{width}" height="{height}" fill="#fbfbfb" stroke="#d9d9d9"/>')

    for i in range(6):
        x = x0 + width * i / 5
        value = x_range[0] + (x_range[1] - x_range[0]) * i / 5
        chunks.append(f'<line x1="{x:.2f}" y1="{y0}" x2="{x:.2f}" y2="{y0 + height}" stroke="#ececec"/>')
        chunks.append(f'<text class="tick" text-anchor="middle" x="{x:.2f}" y="{y0 + height + 22}">{value:.1f}</text>')

    for i in range(5):
        y = y0 + height * i / 4
        value = y_range[1] - (y_range[1] - y_range[0]) * i / 4
        chunks.append(f'<line x1="{x0}" y1="{y:.2f}" x2="{x0 + width}" y2="{y:.2f}" stroke="#ececec"/>')
        chunks.append(f'<text class="tick" text-anchor="end" x="{x0 - 10}" y="{y + 4:.2f}">{value:.3g}</text>')

    chunks.append(
        f'<polyline points="{polyline(normal, key, x_range, y_range, x0, y0, width, height)}" '
        'fill="none" stroke="#c43a24" stroke-width="2.8"/>'
    )
    chunks.append(
        f'<polyline points="{polyline(perform, key, x_range, y_range, x0, y0, width, height)}" '
        'fill="none" stroke="#2764c7" stroke-width="2.8"/>'
    )


def write_svg(normal, perform, output, x_max_ms, force_ymax):
    normal = clip_rows(normal, x_max_ms)
    perform = clip_rows(perform, x_max_ms)
    if len(normal) < 2 or len(perform) < 2:
        raise ValueError("not enough rows in selected time range")

    width = 1280
    left = 112
    right = 42
    header_y = 52
    subtitle_y = 84
    legend_y = 124
    top = 214
    panel_height = 300
    panel_gap = 118
    plot_width = width - left - right
    height = top + panel_height * 2 + panel_gap + 78

    x_end = x_max_ms if x_max_ms is not None else max(normal[-1]["time_ms"], perform[-1]["time_ms"])
    x_range = (0.0, x_end)
    force_values = [row["F"] for row in normal] + [row["F"] for row in perform]
    sample_values = [row["sample"] for row in normal] + [row["sample"] for row in perform]
    force_range = (0.0, force_ymax) if force_ymax is not None else padded_range(force_values, include_zero=True)
    sample_range = padded_range(sample_values, include_zero=True)

    chunks = [
        f'<svg xmlns="http://www.w3.org/2000/svg" width="{width}" height="{height}" viewBox="0 0 {width} {height}">',
        '<rect width="100%" height="100%" fill="white"/>',
        '<style>',
        'text{font-family:-apple-system,BlinkMacSystemFont,Helvetica,Arial,sans-serif}',
        '.title{font-size:30px;font-weight:750;fill:#111}',
        '.subtitle{font-size:14px;fill:#555}',
        '.legend{font-size:16px;font-weight:650}',
        '.panel-title{font-size:19px;font-weight:700;fill:#111}',
        '.tick{font-size:12px;fill:#555}',
        '</style>',
        f'<text class="title" x="{left}" y="{header_y}">Normal vs Hammer-F Perform</text>',
        f'<text class="subtitle" x="{left}" y="{subtitle_y}">Force and output sample traces from the same MIDI, velocity, duration, and probe position.</text>',
        f'<line x1="{left}" y1="{legend_y}" x2="{left + 46}" y2="{legend_y}" stroke="#c43a24" stroke-width="5"/>',
        f'<text class="legend" x="{left + 58}" y="{legend_y + 6}" fill="#c43a24">Normal</text>',
        f'<line x1="{left + 160}" y1="{legend_y}" x2="{left + 206}" y2="{legend_y}" stroke="#2764c7" stroke-width="5"/>',
        f'<text class="legend" x="{left + 218}" y="{legend_y + 6}" fill="#2764c7">Hammer-F Perform</text>',
    ]

    draw_panel(
        chunks,
        normal,
        perform,
        "F",
        "Contact force F [N]",
        force_range,
        x_range,
        left,
        top,
        plot_width,
        panel_height,
    )
    draw_panel(
        chunks,
        normal,
        perform,
        "sample",
        "Output sample proxy",
        sample_range,
        x_range,
        left,
        top + panel_height + panel_gap,
        plot_width,
        panel_height,
    )

    chunks.append(f'<text class="tick" text-anchor="middle" x="{left + plot_width / 2}" y="{height - 24}">time [ms]</text>')
    chunks.append("</svg>")
    output.write_text("\n".join(chunks), encoding="utf-8")


def main():
    parser = argparse.ArgumentParser(description="Overlay Normal and Hammer-F Perform traces.")
    parser.add_argument("normal_csv")
    parser.add_argument("perform_csv")
    parser.add_argument("output_svg")
    parser.add_argument("--x-max-ms", type=float, default=None)
    parser.add_argument("--force-y-max", type=float, default=None)
    args = parser.parse_args()

    normal = read_csv(Path(args.normal_csv))
    perform = read_csv(Path(args.perform_csv))
    output = Path(args.output_svg)
    output.parent.mkdir(parents=True, exist_ok=True)
    write_svg(normal, perform, output, args.x_max_ms, args.force_y_max)

    print(f"normal max F: {max(row['F'] for row in normal):.6g} N")
    print(f"perform max F: {max(row['F'] for row in perform):.6g} N")
    print(f"wrote {output}")


if __name__ == "__main__":
    main()
