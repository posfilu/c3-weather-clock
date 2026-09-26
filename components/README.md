# components

每个子目录是一个 ESP-IDF 组件（自带 `CMakeLists.txt`，用 `idf_component_register` 注册）。

## `*_core` 组件：可测试性硬规则

目录名以 `_core` 结尾的组件是**纯逻辑**组件：

- 只依赖标准 C（`<stdint.h>`、`<string.h>` 等），**不许 include** 任何 ESP-IDF、FreeRTOS、LVGL 头文件。
- 公开头文件放在 `include/`，源文件放在组件根目录或 `src/`。
- 硬件和网络通过参数或函数指针注入。例如解析函数接收 `const char *json`，而不是自己发 HTTP 请求。

`test/host/` 会自动扫描所有 `components/*_core`，把它们的 `.c` 编进 PC 单元测试。
这条规则在 PR 评审时检查。

推荐结构：

```
components/weather_core/
├── CMakeLists.txt        # idf_component_register(SRCS "weather_parse.c" INCLUDE_DIRS "include")
├── include/weather_core.h
└── weather_parse.c
```

对应的测试放在 `test/host/tests/test_weather_core.c`。
