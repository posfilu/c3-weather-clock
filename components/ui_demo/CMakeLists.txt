# M1 上板对照用的演示界面。固件和模拟器（sim/CMakeLists.txt）编译的是同一份源码。
idf_component_register(
    SRCS "demo_screen.c"
    INCLUDE_DIRS "include"
    REQUIRES lvgl
)
