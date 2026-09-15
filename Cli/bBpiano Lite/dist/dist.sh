#!/bin/zsh

cd "/Users/opusarc/Projects/XCodeProjects/bBpiano/Cli/bBpiano Lite/dist" || exit 1

cpp-amalgamate \
    -d "/Users/opusarc/Projects/XCodeProjects/bBpiano/Cli/bBpiano Lite" \
    -o bbpl_single.cpp \
    amalgamation.cpp

