# 踩坑日志

> 由开发者维护。每个坑按下面的模板记一条，最新的写在最上面。
> Claude 在基础设施上踩的坑也记在这里，标注 `[infra]`。

## 2026-09-27 [infra] core.autocrlf 会把 shell 脚本变成 CRLF

- **现象**：第一次提交时 git 对每个文件都警告 `LF will be replaced by CRLF`。
- **根因**：本机全局开了 `core.autocrlf=true`。检出时 `.sh` 会被转成 CRLF，
  在 WSL / Linux 里执行会报 `$'\r': command not found`。
- **解决**：加 `.gitattributes`，写 `* text=auto eol=lf`，工作区统一用 LF，不依赖个人 git 配置。
- **教训**：跨 Windows / Linux 的仓库，第一天就加 `.gitattributes`。

## 2026-09-27 [infra] USB-Serial-JTAG 控制台复位后丢开机日志

- **现象**：冒烟脚本复位板子后，日志从 `alive, uptime 5 s` 开始，bootloader 日志和 `hello` 全部丢失。
- **排查过程**：心跳正常说明固件在跑，问题出在"读"的一侧。脚本在复位后会先关串口、等 0.5 秒再重新打开。
- **根因**：简约版的控制台是芯片内置的 USB-Serial-JTAG。**主机端没在读的时候，芯片会直接丢弃输出**
  （不像 CH343 这类 USB 转串口芯片那样有缓冲）。开机日志正好在串口关闭的空窗期里输出。
- **解决**：复位后保持串口打开、持续读取；如果 USB 重新枚举导致读取异常，再重新打开串口。
- **教训**：用 USB-Serial-JTAG 做控制台时，没人读就丢数据。同理，以后如果看到"日志缺了一段"，
  先怀疑主机端有没有在读，而不是固件没打印。另外，`printf` 在没有主机连接时可能会阻塞或超时，
  这点在 M5 做健壮性时要留意。

## 2026-09-27 [infra] SDL 窗口创建失败后段错误

- **现象**：模拟器 SDL 模式在没有显示环境时（`SDL_VIDEODRIVER=dummy`）直接 `Segmentation fault`。
- **排查过程**：日志先打印了 `lv_sdl_window_create: Failed to initialize window`，紧接着崩溃。
- **根因**：`lv_sdl_window_create()` 失败时返回 `NULL`，代码没检查，直接传给了 `lv_sdl_window_set_zoom()`。
- **解决**：检查返回值，打印原因后退出。
- **教训**：库函数返回指针时，先查文档看它会不会返回 NULL。嵌入式代码里 `malloc`、`xQueueCreate`、
  `lv_obj_create` 这类创建函数同样适用。

## 2026-09-27 [infra] LVGL 9.5 的 CMake 开关名和网上教程不一样

- **现象**：模拟器编译时 LVGL 仍然编译了 examples、demos 和 ThorVG，首次编译明显变慢。
- **排查过程**：CMake 输出里有 `Enabling the building of examples`，说明我设置的
  `LV_CONF_BUILD_DISABLE_EXAMPLES` 没生效。查了 `env_support/cmake/os_desktop.cmake`。
- **根因**：9.5 的开关名是 `CONFIG_LV_BUILD_EXAMPLES`、`CONFIG_LV_BUILD_DEMOS`、
  `CONFIG_LV_USE_THORVG_INTERNAL`，旧名字已经废弃，而且设了旧名字**不会报错**。
- **解决**：改用新开关名，并且设成 `CACHE ... FORCE`。
- **教训**：版本升级后，别照搬网上的配置项；以锁定版本的源码为准。关注构建日志里的
  "Enabling ..." 这类提示，确认配置真的生效了。

<!--
## YYYY-MM-DD 一句话标题

- **现象**：看到了什么（日志、截图、行为）
- **排查过程**：试了什么、排除了什么
- **根因**：
- **解决**：
- **教训**：下次怎么更快发现 / 怎么避免
-->
