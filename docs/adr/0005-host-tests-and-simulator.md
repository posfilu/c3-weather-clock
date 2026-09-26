# ADR-0005：PC 单元测试与 LVGL 模拟器

- 状态：已采纳
- 日期：2026-09-26

## 决策

### 单元测试（L1）

- 框架：**Unity**（ThrowTheSwitch，v2.6.1），通过 CMake FetchContent 拉取。选它是因为 ESP-IDF
  板上测试用的也是 Unity，断言写法两边一致。
- 位置：`test/host/`。自动扫描 `components/*_core`，把每个 core 组件的 `.c` 编进测试。
  测试文件放在 `test/host/tests/test_<模块>.c`，每个文件编译成一个独立的可执行文件，
  由 CTest 统一运行。
- 编译选项：`-Wall -Wextra -Werror`，外加 AddressSanitizer 和 UBSan，在 PC 上尽早抓出越界和未定义行为。

### LVGL 模拟器

- 位置：`sim/`。LVGL v9.5.0 通过 FetchContent 拉取，`sim/lv_conf.h` 的颜色深度（RGB565）与真机一致。
- **两种模式**：
  - **截图模式**（默认，无界面）：渲染后输出 PNG，按 4 倍放大方便查看。CI 里也能跑，
    PR 里可以直接下载截图。
  - **交互模式**（`-DSIM_SDL=ON`）：SDL 窗口，键盘方向键 / 回车模拟摇杆。WSLg 下可以直接弹窗。
- PNG 输出用内置的最小编码器（不压缩的 deflate），不引入额外依赖。

## 理由

- UI 调整是"烧录 → 拍照 → 改"循环里最耗时的部分，截图模式能让 UI 评审完全在 PC 上完成。
- 截图模式不依赖显示器，同一个程序既能给人看，也能在 CI 里跑。

## 后果

- UI 代码（M2）必须写成**只依赖 LVGL**的组件，不能直接调用 ESP-IDF API，
  才能同时编进固件和模拟器。这和 `*_core` 的可测试性规则是同一个思路。
