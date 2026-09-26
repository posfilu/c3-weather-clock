# ADR-0001：用 ESP-IDF + FreeRTOS，不用 RT-Thread

- 状态：已采纳
- 日期：2026-09-26

## 背景

最初设想是 RT-Thread + LVGL。项目需要联网（天气、NTP、OTA），硬件是 ESP32-C3 简约版。

2026-09 调研 RT-Thread 主线 `bsp/ESP/ESP32_C3`（v5.3.0）的结论：

- WiFi 驱动只做了一部分：STA 扫描 / 连接和基础 softAP，`ap_stop`、`deauth` 是空函数；2023 年后没有 WiFi 相关修复；CI 只检查能否编译。
  读代码时还怀疑 RX 回调没有释放 WiFi 接收缓冲，长时间运行可能卡死（未上板验证）。
- 依赖的 ESP-IDF 是 v5.1 时期的快照（`rtt-dev` 分支，2023-11 后无更新），通过 FreeRTOS-Wrapper 让 IDF 代码跑在 RT-Thread 上。
- 控制台只支持 UART0，**不支持 USB-Serial-JTAG**，简约版必须外接 USB 转串口。
- 原生 Windows 编译出的固件启动即崩溃，修复在未合并的 PR #11816 里。
- `drv_spi.c` 写死了 SPI 引脚，与 Air101-LCD 的接线冲突。

## 决策

用 **ESP-IDF（自带 FreeRTOS）+ LVGL**。

项目目标是"熟悉 RTOS 项目开发"，不是"熟悉 RT-Thread 生态"。FreeRTOS 是业界最主流的 RTOS，
任务、队列、事件组、优先级、栈调优、看门狗这些能力完全可以迁移到其他 RTOS。

## 后果

- 精力集中在项目开发本身，而不是修 BSP。
- 放弃了 RT-Thread 特有的内容（设备驱动框架、msh、软件包、scons）。以后如果想学，
  可以换一块 RT-Thread 支持好的板子，再把 `*_core` 业务逻辑移植过去。
