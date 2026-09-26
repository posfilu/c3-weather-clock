# LVGL PC 模拟器。用法见 tools/sim.sh，设计说明见 docs/adr/0005-host-tests-and-simulator.md
cmake_minimum_required(VERSION 3.16)
project(c3wc_sim C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

option(SIM_SDL "打开 SDL 交互窗口（默认是无界面截图模式）" OFF)

# LVGL 会自动使用本目录（顶层工程目录）下的 lv_conf.h。
# 示例、演示和 ThorVG 用不到，关掉以缩短编译时间。
set(CONFIG_LV_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(CONFIG_LV_BUILD_DEMOS OFF CACHE BOOL "" FORCE)
set(CONFIG_LV_USE_THORVG_INTERNAL OFF CACHE BOOL "" FORCE)

include(FetchContent)
FetchContent_Declare(lvgl
    URL https://github.com/lvgl/lvgl/archive/refs/tags/v9.5.0.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(lvgl)

if(SIM_SDL)
    find_package(SDL2 REQUIRED)
    target_compile_definitions(lvgl PUBLIC SIM_SDL=1)
    target_include_directories(lvgl PUBLIC ${SDL2_INCLUDE_DIRS})
    target_link_libraries(lvgl PUBLIC ${SDL2_LIBRARIES})
endif()

# UI 代码和固件共用：components/ 下只依赖 LVGL 的组件直接编进模拟器
set(UI_DEMO_DIR ${CMAKE_CURRENT_LIST_DIR}/../components/ui_demo)

add_executable(c3wc_sim main.c png_writer.c ${UI_DEMO_DIR}/demo_screen.c)
target_include_directories(c3wc_sim PRIVATE ${UI_DEMO_DIR}/include)
target_link_libraries(c3wc_sim PRIVATE lvgl m)
target_compile_options(c3wc_sim PRIVATE -Wall -Wextra -Werror)
