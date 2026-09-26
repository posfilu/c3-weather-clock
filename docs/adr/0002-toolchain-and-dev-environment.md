# ADR-0002：工具链版本与开发环境

- 状态：已采纳
- 日期：2026-09-26

## 决策

| 项 | 选择 |
|---|---|
| ESP-IDF | **v6.0.3**，通过乐鑫官方安装器 EIM（`winget install Espressif.EIM-CLI`）安装 |
| 固件编译 / 烧录 / 串口监视 | **Windows 原生**（`idf.py`），板子直连 COM 口 |
| PC 单元测试、LVGL 模拟器 | **WSL Ubuntu 24.04**（gcc + cmake + SDL2），与 GitHub Actions 的 ubuntu 环境一致 |
| 编辑器 | VS Code（已安装），可选装 ESP-IDF 插件 |
| GitHub | `gh` CLI，走 PR 流程 |

## 理由

- **v6.0.3 而不是 v6.1**：v6.1 在 2026-08-27 刚发布，只有一个月；6.0 系列已经出了 3 个
  bugfix 版本，组件生态（esp_lvgl_port 等）对它的适配更成熟。等 6.1 出了 .1/.2 再考虑升级。
- **不用 v5.5**：新项目没有历史包袱，直接用 6.x，避免以后做一次大版本迁移。
- **固件在 Windows 原生编译**：烧录和串口监视直接走 COM 口。如果在 WSL 里编译，还要用
  usbipd 把 USB 设备转进 WSL，多一层麻烦。
- **PC 端测试放在 WSL**：Windows 上没有现成的 C 编译器和 SDL2，ESP-IDF 的 `linux` target
  也不支持 Windows。WSL 的环境和 CI 一致，"本地过、CI 挂"的概率更低。

## 后果

- 开发者需要知道两套命令：固件用 PowerShell 里的 `idf.py`，测试和模拟器用 `wsl` 执行脚本。
  `tools/` 下的脚本封装了这些差异，见 [开发环境说明](../dev-setup.md)。
