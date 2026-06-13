#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$0")/../.."
build_dir="/private/tmp/bBpiano_HammerLab_Build"
mkdir -p "$build_dir" AcousticLab/HammerLab/.Generated

clang++ -std=c++20 -O0 -g \
  -x c++ AcousticLab/HammerLab/main.cpp.lab \
  "Cli/bBpiano Lite/core/piano/Component/Src/HammerModel.cpp" \
  "Cli/bBpiano Lite/core/piano/Component/Src/StringModel.cpp" \
  "Cli/bBpiano Lite/core/piano/ModelParameters/ModelParameters.cpp" \
  "Cli/bBpiano Lite/core/piano/ModelParameters/PrecomputedValue.cpp" \
  "Cli/bBpiano Lite/core/piano/Utils/MyCSVReader.cpp" \
  -I. \
  -I"Cli/bBpiano Lite/core/piano/Component" \
  -I"Cli/bBpiano Lite/core/piano/Component/Src" \
  -o "$build_dir/HammerLab"

echo "Built $build_dir/HammerLab"
