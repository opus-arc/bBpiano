"""Check the published CLI's original -t output with the CI occupancy limit."""

import argparse
import math
from pathlib import Path
import re


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("log", type=Path)
    parser.add_argument("--exit-code", type=int, required=True)
    parser.add_argument("--max-occupancy", type=float, default=200.0)
    parser.add_argument("--summary", type=Path)
    args = parser.parse_args()
    if not math.isfinite(args.max_occupancy) or args.max_occupancy <= 0:
        raise SystemExit("Invalid CI occupancy limit")

    log = re.sub(r"\x1b\[[0-?]*[ -/]*[@-~]", "", args.log.read_text(encoding="utf-8"))
    number = r"([0-9]+(?:\.[0-9]+)?)"
    pattern = re.compile(
        rf"Frame:\s*{number}\s*us\s*/\s*{number}\s*us\s*"
        rf"Occupancy:\s*{number}%\s*Speed:\s*{number}x realtime\s*"
        r"(High-range|Mid-range|Bass-range|88-keys) realtime:\s*(PASS|FAIL)"
    )
    results = {}
    for frame, budget, occupancy, speed, name, native_status in pattern.findall(log):
        if name in results:
            raise SystemExit(f"Duplicate benchmark range: {name}")
        values = tuple(map(float, (frame, budget, occupancy, speed)))
        if not all(math.isfinite(v) and v > 0 for v in values):
            raise SystemExit(f"Invalid benchmark values: {name}")
        results[name] = (*values, native_status)

    expected = ("High-range", "Mid-range", "Bass-range", "88-keys")
    missing = set(expected) - results.keys()
    if missing:
        raise SystemExit("Incomplete bbpl -t output: " + ", ".join(sorted(missing)))
    passed = args.exit_code == 0 and results["88-keys"][2] <= args.max_occupancy
    lines = ["### bbpl -t", "", "| Range | Occupancy | Speed | Native realtime result |",
             "|---|---:|---:|:---:|"]
    for name in expected:
        _, _, occupancy, speed, native_status = results[name]
        lines.append(f"| {name} | {occupancy:.2f}% | {speed:.2f}× | {native_status} |")
    lines += ["", f"**88-key CI gate: {'PASS' if passed else 'FAIL'}** "
              f"(occupancy ≤ {args.max_occupancy:g}%; process exit {args.exit_code}).", "",
              f"The CI limit is {args.max_occupancy:g}% because GitHub Actions VMs have lower performance "
              "than the development Mac; this is not a 100% real-time guarantee.", ""]
    summary = "\n".join(lines)
    print(summary)
    if args.summary:
        with args.summary.open("a", encoding="utf-8") as output:
            output.write(summary)
    raise SystemExit(0 if passed else 1)


if __name__ == "__main__":
    main()
