#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$0")/../.."
mkdir -p AcousticLab/HammerLab/Build AcousticLab/HammerLab/Generated

clang++ -std=c++20 -O0 -g \
  AcousticLab/HammerLab/main.cpp \
  AcousticLab/HammerLab/MyCSVReaderStub.cpp \
  Core/CppCore/Piano/Component/Src/HammerModel.cpp \
  Core/CppCore/Piano/Component/Src/StringModel.cpp \
  Core/CppCore/ModelParameters/ModelParameters.cpp \
  Core/CppCore/ModelParameters/PrecomputedValue.cpp \
  -I. \
  -ICore/CppCore/Piano/Component \
  -ICore/CppCore/Piano/Component/Src \
  -o AcousticLab/HammerLab/Build/HammerLab

echo "Built AcousticLab/HammerLab/Build/HammerLab"
