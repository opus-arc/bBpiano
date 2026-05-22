#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$0")/../.."

midi="${1:-60}"
velocity="${2:-2}"
seconds="${3:-0.002}"
probe="${4:-0.7}"
ymax="${5:-25}"
xmax_ms="$(python3 -c 'import sys; print(float(sys.argv[1]) * 1000.0)' "$seconds")"

name="bank_force_midi${midi}_v${velocity}_s${seconds}_p${probe}"
csv="AcousticLab/HammerLab/.Generated/${name}.csv"
svg="AcousticLab/HammerLab/.Generated/${name}.svg"

bash AcousticLab/HammerLab/build.sh

/private/tmp/bBpiano_HammerLab_Build/HammerLab \
  --midi "$midi" \
  --velocity "$velocity" \
  --seconds "$seconds" \
  --probe "$probe" \
  --output "$csv"

python3 AcousticLab/HammerLab/plot_hammer_trace.py \
  "$csv" \
  "$svg" \
  --mode force \
  --x-max-ms "$xmax_ms" \
  --y-min 0 \
  --y-max "$ymax"

echo "CSV: $csv"
echo "SVG: $svg"
