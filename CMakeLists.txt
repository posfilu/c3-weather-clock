# ESP-IDF 工程入口。固件版本号由 git describe 自动生成（打 tag 后形如 v0.1.0）。
cmake_minimum_required(VERSION 3.22)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(c3-weather-clock)
