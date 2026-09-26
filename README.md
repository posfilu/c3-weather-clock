# c3-weather-clock

基于 **ESP-IDF + FreeRTOS + LVGL** 的桌面天气时钟，运行在合宙 ESP32C3-CORE（简约版）+ Air101-LCD（0.96 寸 160×80）上。

这是一个以学习 RTOS 项目开发为目的的项目，涵盖多任务架构、IPC、配网、NVS、OTA、健壮性、CI 和单元测试。

> **English:** A desktop weather clock for the LuatOS ESP32C3-CORE board with the Air101-LCD
> (0.96" 160×80 ST7735S), built on ESP-IDF, FreeRTOS and LVGL. It is a learning project for RTOS
> application development: multitasking, IPC, Wi-Fi provisioning, NVS, OTA, robustness, CI and unit tests.
> Docs are in Chinese.

## 功能（规划中）

- 4 个页面：时钟 / 当前天气 / 三天预报 / 系统状态，摇杆翻页
- NTP 对时，和风天气数据，中文显示
- SoftAP 配置网页（WiFi、城市、API Key）
- 从 GitHub Releases 进行 OTA 升级，失败自动回滚

进度见 [docs/roadmap.md](docs/roadmap.md)。

## 快速开始

环境安装见 [docs/dev-setup.md](docs/dev-setup.md)。

```powershell
idf.py build                  # 编译固件
idf.py -p COM8 flash monitor  # 烧录并查看日志（COM 口号以你电脑上的为准）
wsl bash tools/host-test.sh   # PC 单元测试
wsl bash tools/sim.sh         # 模拟器截图 → build-sim/shots/
```

## 文档

| 文档 | 内容 |
|---|---|
| [docs/requirements.md](docs/requirements.md) | 需求与共识 |
| [docs/roadmap.md](docs/roadmap.md) | 里程碑 |
| [docs/adr/](docs/adr/) | 技术决策记录 |
| [docs/acceptance/](docs/acceptance/) | 各里程碑验收清单 |
| [docs/pitfalls.md](docs/pitfalls.md) | 踩坑日志 |

## 许可证

[MIT](LICENSE)
