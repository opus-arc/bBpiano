#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$0")/../.."

midi="${1:-60}"
vin="${2:-4}"
seconds="${3:-0.002}"
ymax="${4:-25}"
probe="${5:-0.7}"
xmax_ms="$(python3 -c 'import sys; print(float(sys.argv[1]) * 1000.0)' "$seconds")"

name="lite_hammerp_force_midi${midi}_vin${vin}_s${seconds}_p${probe}"
csv="AcousticLab/HammerLab/.Generated/${name}.csv"
svg="AcousticLab/HammerLab/.Generated/${name}.svg"

bash AcousticLab/HammerLab/build.sh

/private/tmp/bBpiano_HammerLab_Build/HammerLab \
  --midi "$midi" \
  --vin "$vin" \
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
