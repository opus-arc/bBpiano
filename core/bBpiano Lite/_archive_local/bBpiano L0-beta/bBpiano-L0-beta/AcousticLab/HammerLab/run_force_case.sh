#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$0")/../.."

mode="${1:-normal}"
midi="${2:-60}"
vin="${3:-4}"
seconds="${4:-0.002}"
ymax="${5:-25}"
probe="${6:-0.7}"
xmax_ms="$(python3 -c 'import sys; print(float(sys.argv[1]) * 1000.0)' "$seconds")"

if [[ "$mode" != "normal" && "$mode" != "hammerf" && "$mode" != "perform" ]]; then
  echo "mode must be normal, hammerf, or perform" >&2
  exit 1
fi

name="${mode}_force_midi${midi}_vin${vin}_s${seconds}_p${probe}"
csv="AcousticLab/HammerLab/.Generated/${name}.csv"
svg="AcousticLab/HammerLab/.Generated/${name}.svg"

bash AcousticLab/HammerLab/build.sh

/private/tmp/bBpiano_HammerLab_Build/HammerLab \
  --mode "$mode" \
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
