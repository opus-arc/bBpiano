#!/bin/zsh

set -e

ROOT_DIR=${0:A:h:h}
BUILD_DIR="$ROOT_DIR/hammer_benchmark/build"

mkdir -p "$BUILD_DIR"
cd "$ROOT_DIR"

COMMON_SOURCES=(
    "Cli/bBpiano Lite/core/piano/PianoModel.cpp"
    "Cli/bBpiano Lite/core/piano/Component/Src/KeyModel.cpp"
    "Cli/bBpiano Lite/core/piano/Component/Src/HammerModel.cpp"
    "Cli/bBpiano Lite/core/piano/Component/Src/StringModel.cpp"
    "Cli/bBpiano Lite/core/piano/Component/Src/DamperModel.cpp"
    "Cli/bBpiano Lite/core/piano/Component/Src/PedalModel.cpp"
    "Cli/bBpiano Lite/core/piano/Utils/MyCSVReader.cpp"
)

clang++ \
    -std=c++20 \
    -O3 \
    -DNDEBUG \
    -I"Cli/bBpiano Lite/core/piano/Component" \
    "hammer_benchmark/render_hammer_benchmark.cpp" \
    "${COMMON_SOURCES[@]}" \
    -o "$BUILD_DIR/render_hammer_benchmark"

clang++ \
    -std=c++20 \
    -O3 \
    -DNDEBUG \
    -I"Cli/bBpiano Lite/core/piano/Component" \
    -DBBPIANO_ENABLE_HAMMER_STRING_COUPLING_TEST \
    -DBBPIANO_HAMMER_STRING_COUPLING_TEST_MAIN \
    "Test/HammerStringEvaluationTests/HammerStringTest.cpp" \
    "${COMMON_SOURCES[@]}" \
    -o "$BUILD_DIR/hammer_string_coupling_test"

clang++ \
    -std=c++20 \
    -O3 \
    -DNDEBUG \
    -I"Cli/bBpiano Lite/core/piano/Component" \
    "hammer_benchmark/performance_main.cpp" \
    "Cli/bBpiano Lite/hardware/DSP-Benchmark.cpp" \
    "${COMMON_SOURCES[@]}" \
    -o "$BUILD_DIR/hammer_performance_test"

echo "Built Hammer benchmark tools in $BUILD_DIR"
