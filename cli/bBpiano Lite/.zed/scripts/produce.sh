#!/bin/zsh
# produce

# 设置中途出错不继续往下执行
set -e

mkdir -p build/bbpl_SDK/lib

find . \
    -path './build' -prune \
    -o -path './_archive_local' -prune \
    -o -path './notes' -prune \
    -o -name '*.cpp' \
    ! -name "main.cpp" -print0 \
| xargs -0 \
    cpp-amalgamate -d . \
    -o './build/bbpl_SDK.cpp'

cppobf --verify './build/bbpl_SDK.cpp' \
    -o './build/bbpl_SDK_cppobf.cpp'

clang++ \
    -std=c++20 \
    -Os \
    -c './build/bbpl_SDK_cppobf.cpp' \
    -o './build/bbpl.o'

ar rcs \
    './build/bbpl_SDK/lib/libbbpl.a' \
    './build/bbpl.o'

cp './core/c_api/bbpl_c_api.h' './build/bbpl_SDK/bbpl_c_api.h'
cp './core/c_api/bbpl-Bridging-Header.h' './build/bbpl_SDK/bbpl-Bridging-Header.h'

rm -rf './build/bbpl_SDK/example'
cp -R './example' './build/bbpl_SDK/example'

cat > './build/bbpl_SDK/main.cpp' <<'EOF'
#include "bbpl_c_api.h"

#include <chrono>
#include <thread>

int main() {

    // midi_service_start(
    //     "./build/bbpl_SDK/example/Nocturne No. 6 in D-Flat Major, Op. 63.midi"
    // );
    // std::this_thread::sleep_for(std::chrono::seconds(20));
    // midi_service_stop();
    test_service_start();

    return 0;
}
EOF

clang++ -std=c++20 './build/bbpl_SDK/main.cpp' \
    -I./build/bbpl_SDK \
    -L./build/bbpl_SDK/lib \
    -lbbpl \
    -framework AudioToolbox \
    -framework CoreMIDI \
    -framework CoreFoundation \
    -o './build/bbpl_SDK/bbpl'

rm './build/bbpl_SDK/main.cpp'

./build/bbpl_SDK/bbpl
