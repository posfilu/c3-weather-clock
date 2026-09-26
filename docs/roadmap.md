# 里程碑

每个里程碑开始前，先在 `docs/acceptance/Mx.md` 写好验收清单并得到开发者认可。

| # | 内容 | 负责 | 状态 |
|---|---|---|---|
| M0 | 安装 ESP-IDF、工程骨架、CI、PC 模拟器；串口输出 hello；实测 Flash 容量 | Claude | 进行中 |
| M1 | 点亮屏幕（实测偏移）、接入 LVGL、摇杆驱动（实测方向，处理 GPIO13 与 LED 共用） | 开发者 | |
| M2 | 系统架构设计（开发者出草案，Claude 评审）；在模拟器里用假数据做出 4 个页面，然后上板 | 开发者 | |
| M3 | WiFi、SoftAP 配置网页、NVS | 开发者 | |
| M4 | NTP、和风天气（gzip 解压、JSON 解析）、城市搜索、中文字库子集 | 开发者；Claude 写逻辑测试和字库生成脚本 | |
| M5 | 故障注入与健壮性：断网重连、重试、看门狗、core dump | 开发者 | |
| M6 | 串口命令行（esp_console） | 开发者 | |
| M7 | OTA（GitHub Releases、自动回滚）；CI 自动发版 | 开发者；Claude 做 CI 部分 | |
