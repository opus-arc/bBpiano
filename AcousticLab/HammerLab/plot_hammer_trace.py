#!/usr/bin/env python3
import csv
import argparse
import math
import sys
from pathlib import Path


def read_csv(path):
    rows = []
    with open(path, newline="") as f:
        for row in csv.DictReader(f):
            rows.append({
                "time_ms": float(row["time_sec"]) * 1000.0,
                "F": float(row["F"]),
                "dy": float(row["dy"]),
                "v_in": float(row["v_in"]),
                "dv": float(row["dv"]),
                "sample": float(row["sample"]),
            })
    return rows


def nice_range(values):
    lo = min(values)
    hi = max(values)
    if not math.isfinite(lo) or not math.isfinite(hi):
        return -1.0, 1.0
    if abs(hi - lo) < 1e-12:
        pad = max(abs(hi) * 0.1, 1e-9)
        return lo - pad, hi + pad
    pad = (hi - lo) * 0.08
    return lo - pad, hi + pad


def polyline(points, x_key, y_key, x_range, y_range, x0, y0, w, h):
    xmin, xmax = x_range
    ymin, ymax = y_range
    coords = []
    for p in points:
        x = x0 + (p[x_key] - xmin) / (xmax - xmin) * w
        y = y0 + h - (p[y_key] - ymin) / (ymax - ymin) * h
        coords.append(f"{x:.2f},{y:.2f}")
    return " ".join(coords)


def clipped_rows(rows, x_max_ms):
    if x_max_ms is None:
        return rows
    return [row for row in rows if row["time_ms"] <= x_max_ms]


def make_svg(rows, output_path, mode="overview", x_max_ms=None, y_min=None, y_max=None):
    width = 1200
    panel_h = 210
    left = 104
    right = 36
    title_y = 44
    top = 122
    label_gap = 18
    gap = 68
    plot_w = width - left - right
    if mode == "force":
        panel_h = 560
        top = 92
        series = [("F", "contact force F [N]", "#c23b22")]
        title = "HammerLab force trace"
    else:
        series = [
            ("F", "contact force F [N]", "#c23b22"),
            ("dy", "felt compression dy [m]", "#3763b8"),
            ("v_in", "hammer velocity v_in [m/s proxy]", "#21835f"),
            ("sample", "output sample proxy", "#6a3d9a"),
        ]
        title = "HammerLab trace: F / dy / v_in / sample"

    rows = clipped_rows(rows, x_max_ms)
    if len(rows) < 2:
        raise ValueError("Not enough rows in selected time range")

    height = top + len(series) * panel_h + (len(series) - 1) * gap + 72
    x_range = (rows[0]["time_ms"], x_max_ms if x_max_ms is not None else rows[-1]["time_ms"])

    chunks = [
        f'<svg xmlns="http://www.w3.org/2000/svg" width="{width}" height="{height}" viewBox="0 0 {width} {height}">',
        '<rect width="100%" height="100%" fill="white"/>',
        '<style>text{font-family:-apple-system,BlinkMacSystemFont,Helvetica,Arial,sans-serif}.label{font-size:18px;font-weight:600}.tick{font-size:12px;fill:#555}.title{font-size:28px;font-weight:700}</style>',
        f'<text class="title" x="{left}" y="{title_y}">{title}</text>',
    ]

    for idx, (key, label, color) in enumerate(series):
        y = top + idx * (panel_h + gap)
        vals = [r[key] for r in rows]
        if mode == "force" and y_min is not None and y_max is not None:
            y_range = (y_min, y_max)
        else:
            y_range = nice_range(vals)

        chunks.append(f'<text class="label" x="{left}" y="{y - label_gap}">{label}</text>')
        chunks.append(f'<rect x="{left}" y="{y}" width="{plot_w}" height="{panel_h}" fill="#fbfbfb" stroke="#ddd"/>')

        x_tick_count = 5 if mode == "force" else 6
        for t in range(x_tick_count):
            denom = x_tick_count - 1
            gx = left + plot_w * t / denom
            tx = x_range[0] + (x_range[1] - x_range[0]) * t / denom
            chunks.append(f'<line x1="{gx:.2f}" y1="{y}" x2="{gx:.2f}" y2="{y + panel_h}" stroke="#eee"/>')
            chunks.append(f'<text class="tick" x="{gx - 16:.2f}" y="{y + panel_h + 18}">{tx:.1f}</text>')

        y_tick_count = 6 if mode == "force" else 5
        for t in range(y_tick_count):
            denom = y_tick_count - 1
            gy = y + panel_h * t / denom
            ty = y_range[1] - (y_range[1] - y_range[0]) * t / denom
            chunks.append(f'<line x1="{left}" y1="{gy:.2f}" x2="{left + plot_w}" y2="{gy:.2f}" stroke="#eee"/>')
            chunks.append(f'<text class="tick" text-anchor="end" x="{left - 8}" y="{gy + 4:.2f}">{ty:.3g}</text>')

        points = polyline(rows, "time_ms", key, x_range, y_range, left, y, plot_w, panel_h)
        chunks.append(f'<polyline points="{points}" fill="none" stroke="{color}" stroke-width="2"/>')

    chunks.append(f'<text class="tick" x="{left + plot_w / 2 - 48}" y="{height - 22}">time [ms]</text>')
    chunks.append("</svg>")
    output_path.write_text("\n".join(chunks), encoding="utf-8")


def main():
    parser = argparse.ArgumentParser(description="Plot HammerLab CSV traces as SVG.")
    parser.add_argument("input_csv")
    parser.add_argument("output_svg")
    parser.add_argument("--mode", choices=["overview", "force"], default="overview")
    parser.add_argument("--x-max-ms", type=float, default=None)
    parser.add_argument("--y-min", type=float, default=None)
    parser.add_argument("--y-max", type=float, default=None)
    args = parser.parse_args()

    csv_path = Path(args.input_csv)
    svg_path = Path(args.output_svg)
    rows = read_csv(csv_path)
    if not rows:
        print(f"No rows in {csv_path}", file=sys.stderr)
        return 1
    svg_path.parent.mkdir(parents=True, exist_ok=True)
    make_svg(rows, svg_path, args.mode, args.x_max_ms, args.y_min, args.y_max)
    print(f"Wrote {svg_path}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
