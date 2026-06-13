#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$0")/../.."

csv="${1:-AcousticLab/HammerLab/.Generated/lite_hammerp_force_midi60_vin4_s0.003_p0.7.csv}"
out="${2:-}"

if [[ -z "$out" ]]; then
  stem="$(basename "$csv" .csv)"
  out="AcousticLab/HammerLab/.Generated/${stem}_over_fig54.svg"
fi

python3 AcousticLab/HammerLab/overlay_fig54_csv.py \
  "$csv" \
  "$out" \
  --reference "AcousticLab/HammerLab/.References/figure 5.4.png"

echo "Overlay SVG: $out"
