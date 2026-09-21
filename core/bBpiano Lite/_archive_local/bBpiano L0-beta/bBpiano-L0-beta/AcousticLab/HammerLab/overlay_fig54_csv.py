#!/usr/bin/env python3
import argparse
import base64
import csv
import html
from pathlib import Path


def read_force_rows(csv_path):
    rows = []
    with open(csv_path, newline="") as f:
        for row in csv.DictReader(f):
            rows.append((float(row["time_sec"]) * 1000.0, float(row["F"])))
    return rows


def read_png_size(png_path):
    with open(png_path, "rb") as f:
        header = f.read(24)
    if header[:8] != b"\x89PNG\r\n\x1a\n":
        raise ValueError(f"{png_path} is not a PNG file")
    width = int.from_bytes(header[16:20], "big")
    height = int.from_bytes(header[20:24], "big")
    return width, height


def csv_polyline(rows, x_min, x_max, y_min, y_max, plot_left, plot_top, plot_right, plot_bottom):
    plot_w = plot_right - plot_left
    plot_h = plot_bottom - plot_top
    coords = []
    for t_ms, force in rows:
        if t_ms < x_min or t_ms > x_max:
            continue
        x = plot_left + (t_ms - x_min) / (x_max - x_min) * plot_w
        y = plot_bottom - (force - y_min) / (y_max - y_min) * plot_h
        coords.append(f"{x:.2f},{y:.2f}")
    return " ".join(coords)


def main():
    parser = argparse.ArgumentParser(
        description="Overlay a HammerLab force CSV on Bank Fig. 5.4 using the figure's pixel coordinates."
    )
    parser.add_argument("input_csv")
    parser.add_argument("output_svg")
    parser.add_argument(
        "--reference",
        default="AcousticLab/HammerLab/.References/figure 5.4.png",
        help="Bank Fig. 5.4 PNG path",
    )
    parser.add_argument("--x-min-ms", type=float, default=0.0)
    parser.add_argument("--x-max-ms", type=float, default=2.0)
    parser.add_argument("--y-min", type=float, default=0.0)
    parser.add_argument("--y-max", type=float, default=25.0)
    # Defaults calibrated for the current .References/figure 5.4.png image.
    parser.add_argument("--plot-left", type=float, default=358.0)
    parser.add_argument("--plot-top", type=float, default=65.0)
    parser.add_argument("--plot-right", type=float, default=1348.0)
    parser.add_argument("--plot-bottom", type=float, default=847.0)
    parser.add_argument("--color", default="#d62728")
    parser.add_argument("--stroke-width", type=float, default=4.0)
    args = parser.parse_args()

    csv_path = Path(args.input_csv)
    output_path = Path(args.output_svg)
    reference_path = Path(args.reference)

    rows = read_force_rows(csv_path)
    if len(rows) < 2:
        raise ValueError(f"Not enough rows in {csv_path}")

    width, height = read_png_size(reference_path)
    png_data = base64.b64encode(reference_path.read_bytes()).decode("ascii")
    points = csv_polyline(
        rows,
        args.x_min_ms,
        args.x_max_ms,
        args.y_min,
        args.y_max,
        args.plot_left,
        args.plot_top,
        args.plot_right,
        args.plot_bottom,
    )
    if not points:
        raise ValueError("No CSV points fall inside the selected x-axis window")

    output_path.parent.mkdir(parents=True, exist_ok=True)
    label = html.escape(csv_path.name)
    svg = f"""<svg xmlns="http://www.w3.org/2000/svg" width="{width}" height="{height}" viewBox="0 0 {width} {height}">
<image href="data:image/png;base64,{png_data}" x="0" y="0" width="{width}" height="{height}"/>
<polyline points="{points}" fill="none" stroke="{html.escape(args.color)}" stroke-width="{args.stroke_width}" stroke-linejoin="round" stroke-linecap="round"/>
<rect x="{args.plot_left + 24:.1f}" y="{args.plot_top + 24:.1f}" width="360" height="42" fill="white" opacity="0.82"/>
<line x1="{args.plot_left + 42:.1f}" y1="{args.plot_top + 50:.1f}" x2="{args.plot_left + 112:.1f}" y2="{args.plot_top + 50:.1f}" stroke="{html.escape(args.color)}" stroke-width="{args.stroke_width}"/>
<text x="{args.plot_left + 126:.1f}" y="{args.plot_top + 56:.1f}" font-family="Helvetica, Arial, sans-serif" font-size="24" fill="{html.escape(args.color)}">{label}</text>
</svg>
"""
    output_path.write_text(svg, encoding="utf-8")
    print(f"Wrote {output_path}")


if __name__ == "__main__":
    raise SystemExit(main())
