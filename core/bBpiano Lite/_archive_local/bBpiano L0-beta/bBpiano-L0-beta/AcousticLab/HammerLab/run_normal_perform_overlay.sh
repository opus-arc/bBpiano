#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$0")/../.."

midi="${1:-60}"
vin="${2:-4}"
seconds="${3:-0.002}"
probe="${4:-0.7}"
force_ymax="${5:-25}"
xmax_ms="$(python3 -c 'import sys; print(float(sys.argv[1]) * 1000.0)' "$seconds")"

normal_name="normal_force_midi${midi}_vin${vin}_s${seconds}_p${probe}"
perform_name="perform_force_midi${midi}_vin${vin}_s${seconds}_p${probe}"
normal_csv="AcousticLab/HammerLab/.Generated/${normal_name}.csv"
perform_csv="AcousticLab/HammerLab/.Generated/${perform_name}.csv"
overlay_svg="AcousticLab/HammerLab/.Generated/normal_vs_perform_midi${midi}_vin${vin}_s${seconds}_p${probe}.svg"

bash AcousticLab/HammerLab/build.sh

/private/tmp/bBpiano_HammerLab_Build/HammerLab \
  --mode normal \
  --midi "$midi" \
  --vin "$vin" \
  --seconds "$seconds" \
  --probe "$probe" \
  --output "$normal_csv"

/private/tmp/bBpiano_HammerLab_Build/HammerLab \
  --mode perform \
  --midi "$midi" \
  --vin "$vin" \
  --seconds "$seconds" \
  --probe "$probe" \
  --output "$perform_csv"

python3 AcousticLab/HammerLab/overlay_hammer_traces.py \
  "$normal_csv" \
  "$perform_csv" \
  "$overlay_svg" \
  --x-max-ms "$xmax_ms" \
  --force-y-max "$force_ymax"

echo "Normal CSV:  $normal_csv"
echo "Perform CSV: $perform_csv"
echo "Overlay SVG: $overlay_svg"
