#!/bin/zsh

set -e

mkdir -p build

find . -path './build' -prune \
    -o -path './_archive_local' -prune \
    -o -path './notes' -prune \
    -o -name '*.cpp' \
    -print0 \
| xargs -0 \
    cpp-amalgamate -d . \
    -o './build/bbpl.cpp'

clang++ \
    -std=c++20 \
    -Os \
    './build/bbpl.cpp' \
    -framework AudioToolbox \
    -framework CoreMIDI \
    -framework CoreFoundation \
    -o './build/bbpl'

./build/bbpl -t
