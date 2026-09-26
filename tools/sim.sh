#!/usr/bin/env bash
# LVGL 模拟器。在 Linux / WSL 里执行；Windows 下用：wsl bash tools/sim.sh [shot|sdl]
#   shot（默认）：无界面渲染，截图输出到 build-sim/shots/
#   sdl        ：打开 SDL 窗口，方向键 / 回车模拟摇杆（需要 libsdl2-dev；WSLg 下可直接弹窗）
set -euo pipefail
cd "$(dirname "$0")/.."

mode="${1:-shot}"
case "$mode" in
shot)
    cmake -S sim -B build-sim -DSIM_SDL=OFF
    cmake --build build-sim -j
    mkdir -p build-sim/shots
    ./build-sim/c3wc_sim --out build-sim/shots/demo.png --scale 4
    ;;
sdl)
    cmake -S sim -B build-sim-sdl -DSIM_SDL=ON
    cmake --build build-sim-sdl -j
    ./build-sim-sdl/c3wc_sim
    ;;
*)
    echo "usage: $0 [shot|sdl]" >&2
    exit 2
    ;;
esac
