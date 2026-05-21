#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$0")/../.."

midi="${1:-69}"
velocity="${2:-80}"
seconds="${3:-0.08}"
probe="${4:-0.7}"

name="midi${midi}_v${velocity}_p${probe}"
csv="AcousticLab/HammerLab/Generated/${name}.csv"
svg="AcousticLab/HammerLab/Generated/${name}.svg"

bash AcousticLab/HammerLab/build.sh

AcousticLab/HammerLab/Build/HammerLab \
  --midi "$midi" \
  --velocity "$velocity" \
  --seconds "$seconds" \
  --probe "$probe" \
  --output "$csv"

python3 AcousticLab/HammerLab/plot_hammer_trace.py "$csv" "$svg"

echo "CSV: $csv"
echo "SVG: $svg"
