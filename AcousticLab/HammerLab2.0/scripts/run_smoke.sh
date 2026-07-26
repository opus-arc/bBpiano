#!/bin/sh
set -eu

root=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
binary="$root/build/hammerlab2"
run_dir="$root/runs/smoke"
mkdir -p "$run_dir"

"$binary" simulate \
    --case-id smoke_stulov_rigid \
    --model stulov \
    --load rigid \
    --trace "$run_dir/trace.csv" \
    --summary "$run_dir/summary.json"

printf '%s\n' "Smoke artifacts: $run_dir"
