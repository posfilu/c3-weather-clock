#!/usr/bin/env bash
# L1：编译并运行 PC 单元测试。在 Linux / WSL 里执行；Windows 下用：wsl bash tools/host-test.sh
set -euo pipefail
cd "$(dirname "$0")/.."

cmake -S test/host -B build-host -DCMAKE_BUILD_TYPE=Debug
cmake --build build-host -j
ctest --test-dir build-host --output-on-failure
