#!/bin/zsh

set -e

ROOT_DIR=${0:A:h:h:h}
OUTPUT=/tmp/bbpiano_damper_lifecycle_test

cd "$ROOT_DIR"

clang++ \
    -std=c++20 \
    -O3 \
    -I"Cli/bBpiano Lite/core/piano/Component" \
    "Test/DamperLifecycleTests/DamperLifecycleTest.cpp" \
    "Cli/bBpiano Lite/core/piano/PianoModel.cpp" \
    "Cli/bBpiano Lite/core/piano/Component/Src/KeyModel.cpp" \
    "Cli/bBpiano Lite/core/piano/Component/Src/HammerModel.cpp" \
    "Cli/bBpiano Lite/core/piano/Component/Src/StringModel.cpp" \
    "Cli/bBpiano Lite/core/piano/Component/Src/DamperModel.cpp" \
    "Cli/bBpiano Lite/core/piano/Component/Src/PedalModel.cpp" \
    "Cli/bBpiano Lite/core/piano/Utils/MyCSVReader.cpp" \
    -o "$OUTPUT"

"$OUTPUT"
