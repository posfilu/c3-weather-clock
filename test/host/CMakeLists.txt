# L1：PC 单元测试。在 Linux / WSL 上运行：bash tools/host-test.sh
# 设计说明见 docs/adr/0005-host-tests-and-simulator.md
cmake_minimum_required(VERSION 3.16)
project(c3wc_host_tests C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(unity
    URL https://github.com/ThrowTheSwitch/Unity/archive/refs/tags/v2.6.1.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(unity)

# 整个工程都开 ASan / UBSan，尽早在 PC 上抓出越界和未定义行为
set(SANITIZERS -fsanitize=address,undefined -fno-omit-frame-pointer -fno-sanitize-recover=all)
add_compile_options(-g ${SANITIZERS})
add_link_options(${SANITIZERS})
set(STRICT_WARNINGS -Wall -Wextra -Werror)

set(REPO_ROOT ${CMAKE_CURRENT_LIST_DIR}/../..)

# 收集所有 components/*_core 的源文件和 include 目录
file(GLOB CORE_DIRS LIST_DIRECTORIES true ${REPO_ROOT}/components/*_core)
set(CORE_SOURCES "")
set(CORE_INCLUDES "")
foreach(dir IN LISTS CORE_DIRS)
    if(IS_DIRECTORY ${dir})
        file(GLOB_RECURSE srcs ${dir}/*.c)
        list(APPEND CORE_SOURCES ${srcs})
        if(IS_DIRECTORY ${dir}/include)
            list(APPEND CORE_INCLUDES ${dir}/include)
        endif()
    endif()
endforeach()

if(CORE_SOURCES)
    add_library(core STATIC ${CORE_SOURCES})
    target_include_directories(core PUBLIC ${CORE_INCLUDES})
    target_compile_options(core PRIVATE ${STRICT_WARNINGS})
else()
    add_library(core INTERFACE)
endif()

enable_testing()

# 每个 tests/test_*.c 编成一个独立的可执行文件
file(GLOB TEST_SOURCES ${CMAKE_CURRENT_LIST_DIR}/tests/test_*.c)
foreach(src IN LISTS TEST_SOURCES)
    get_filename_component(name ${src} NAME_WE)
    add_executable(${name} ${src})
    target_link_libraries(${name} PRIVATE unity core)
    target_compile_options(${name} PRIVATE ${STRICT_WARNINGS})
    add_test(NAME ${name} COMMAND ${name})
endforeach()
