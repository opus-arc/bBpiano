#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$0")/../.."
build_dir="/private/tmp/bBpiano_HammerLab_Build"
mkdir -p "$build_dir" AcousticLab/HammerLab/.Generated

clang++ -std=c++20 -O0 -g \
  -x c++ AcousticLab/HammerLab/main.cpp.lab \
  -x c++ AcousticLab/HammerLab/MyCSVReaderStub.cpp.lab \
  Core/CppCore/Piano/Component/Src/HammerModel.cpp \
  Core/CppCore/Piano/Component/Src/StringModel.cpp \
  Core/CppCore/ModelParameters/ModelParameters.cpp \
  Core/CppCore/ModelParameters/PrecomputedValue.cpp \
  -I. \
  -ICore/CppCore/Piano/Component \
  -ICore/CppCore/Piano/Component/Src \
  -o "$build_dir/HammerLab"

echo "Built $build_dir/HammerLab"
