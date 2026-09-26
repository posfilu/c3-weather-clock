# 开发环境

选型理由见 [ADR-0002](adr/0002-toolchain-and-dev-environment.md)。

## 1. 固件：Windows 原生 ESP-IDF v6.0.3

```powershell
winget install Espressif.EIM-CLI
eim install -c tools/eim_config.toml   # 按仓库里记录的参数安装，与项目环境完全一致
```

`tools/eim_config.toml` 是 M0 安装时 EIM 自动保存的配置：ESP-IDF v6.0.3，只装 esp32c3 相关工具。

ESP-IDF 装在 `C:\esp\v6.0.3\esp-idf`，工具装在 `C:\Espressif\tools`。安装器会在桌面放一个
**ESP-IDF PowerShell** 快捷方式；也可以在任意 PowerShell 里手动激活：

```powershell
. C:\Espressif\tools\Microsoft.v6.0.3.PowerShell_profile.ps1
```

然后进入仓库目录：

```powershell
idf.py build
idf.py -p COM8 flash monitor   # Ctrl+] 退出 monitor
python tools/smoke.py -p COM8  # L2 冒烟测试：复位并检查开机日志
```

- 简约版用芯片内置的 USB-Serial-JTAG，设备管理器里显示为"USB 串行设备"。COM 口号以你电脑上的为准。
- 如果烧录失败：按住 BOOT 键（GPIO9），插拔 USB 或按一下 RST，再重新烧录。

## 2. PC 单元测试与模拟器：WSL Ubuntu

```bash
sudo apt update
sudo apt install -y build-essential cmake libsdl2-dev
```

在 PowerShell 的仓库目录下直接运行：

```powershell
wsl bash tools/host-test.sh   # L1 单元测试
wsl bash tools/sim.sh         # 截图 → build-sim/shots/demo.png
wsl bash tools/sim.sh sdl     # SDL 交互窗口（方向键 / 回车）
```

## 3. GitHub

```powershell
winget install GitHub.cli
gh auth login   # 需要你本人在浏览器里完成授权
```
