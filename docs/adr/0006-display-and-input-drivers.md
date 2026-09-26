# ADR-0006：屏幕、LVGL 移植层与按键驱动的选型

- 状态：已采纳
- 日期：2026-09-27

## 背景

M1 要点亮 Air101-LCD（ST7735S，160×80，SPI），接入 LVGL，驱动五向摇杆。
本 ADR 只决定**用哪些库**；怎么组织代码、按键事件交给谁处理，属于 M2 的架构设计，由开发者决定。

### 引脚（资料汇总，M1 实测确认）

| 功能 | GPIO | 备注 |
|---|---|---|
| SPI SCK | 2 | |
| SPI MOSI | 3 | 屏幕只写不读，没有 MISO |
| LCD CS | 7 | |
| LCD DC | 6 | |
| LCD RST | 10 | |
| 背光 | 11 | VDD_SPI，**不要配置**，背光常亮（见 requirements） |
| 摇杆中键 | 4 | 低电平有效 |
| 摇杆方向键 | 5、8、9、13 | 低电平有效；具体方向 M1 实测。8/9 是 strapping 脚，13 与 LED D5 共用 |

## 决策

| 用途 | 选择 | 版本 |
|---|---|---|
| 屏幕驱动 | ESP-IDF 自带的 `esp_lcd`：SPI panel IO + **`esp_lcd_new_panel_st7789`** | 随 IDF v6.0.3 |
| LVGL | `lvgl/lvgl` | **锁定 9.5.0**（与模拟器一致，见 ADR-0003） |
| LVGL 移植层 | `espressif/esp_lvgl_port` | ~2.9.0 |
| 按键 | `espressif/button`（iot_button） | ^4.2 |

### 为什么用 ST7789 的驱动来驱动 ST7735S

- ST7735S 和 ST7789 都遵循 MIPI DCS 基本命令集，初始化和刷屏用到的命令完全一样：
  `SWRESET`、`SLPOUT`、`MADCTL`、`COLMOD`、`CASET`、`RASET`、`RAMWR`、`INVON`、`DISPON`。
  驱动提供的 `set_gap`（显示偏移）、`invert_color`、`mirror`、`swap_xy` 正好是这块屏要调的参数。
- 它是 IDF 自带的，跟着 IDF 一起维护，没有第三方依赖风险。
- **已知差异**：ST7735S 有自己的帧率、电源、Gamma 寄存器（`FRMCTR1`、`PWCTR1`、`GMCTRP1` 等），
  ST7789 驱动不会设置它们。如果实测颜色发灰、偏色或闪烁，就在 `esp_lcd_panel_init()` 之后
  用 `esp_lcd_panel_io_tx_param()` 补发这些命令，参考 LuatOS 的 st7735s 初始化序列。

### 放弃的方案

| 方案 | 放弃理由 |
|---|---|
| `waveshare/esp_lcd_st7735` 2.0.0 | 第三方；没有声明 IDF 版本约束，与 v6 的兼容性未知 |
| `teriyakigod/esp_lcd_st7735` 0.0.1 | 下载量高，但 2024 年后没有更新 |
| 从零写 SPI 驱动 | `esp_lcd` 已经处理好了 DMA、队列化传输和 DC 线切换，重写没有学习以外的收益 |
| 不用 `esp_lvgl_port`，自己写 LVGL 任务和锁 | 可以作为练习，但 M1 的重点是"点亮 + 调参"。`esp_lvgl_port` 的锁机制正好是 M2 讨论线程安全的现成素材 |

### 为什么用 `espressif/button`

- 它提供消抖，以及单击、长按（可配置时长）、长按保持等事件，正好满足"中键长按 3 秒进入配网"。
- 按键事件是**喂给 LVGL 的输入设备（indev）**，还是**发到应用自己的队列**，由 M2 架构决定。

## 一致性约束

- 固件里 LVGL 通过 menuconfig（Kconfig）配置，模拟器用 `sim/lv_conf.h`。两边的**颜色深度（16 位）
  和启用的字体必须一致**，否则模拟器截图不能代表真机效果。
  这些 LVGL 选项统一写在 `sdkconfig.defaults` 里，改其中一边时另一边要同步。
- SPI 传输的是大端 RGB565，而 LVGL 在内存里是小端，需要交换字节。
  `esp_lvgl_port` 的显示配置里有对应开关，模拟器那边不需要交换。

## 实现提示（给开发者，非强制）

- SPI 时钟：ST7735S 规格书的写周期下限是 66ns，对应约 **15 MHz**。先按 15 MHz 跑通，再尝试提速，
  出现花屏就退回来。
- 160×80×2 字节只有 25.6KB，用两块部分缓冲（比如每块 20 行）就足够流畅。
