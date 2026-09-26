# ADR-0004：分区表

- 状态：已采纳（2026-09-27 实测 Flash 为 4MB，无需修订）
- 日期：2026-09-26

## 决策

按 4MB Flash 设计，从 M0 起就预留双 OTA 分区和 core dump 分区，见仓库根目录 `partitions.csv`：

| 名称 | 类型 | 偏移 | 大小 | 用途 |
|---|---|---|---|---|
| nvs | data/nvs | 0x9000 | 24K | WiFi、城市、API Key 等配置 |
| otadata | data/ota | 0xF000 | 8K | 记录当前从哪个 OTA 分区启动 |
| phy_init | data/phy | 0x11000 | 4K | 射频校准数据 |
| ota_0 | app/ota_0 | 0x20000 | 1920K | 固件槽 A |
| ota_1 | app/ota_1 | 0x200000 | 1920K | 固件槽 B |
| coredump | data/coredump | 0x3E0000 | 64K | 崩溃现场，M5 用 |

末尾 64K 暂时空着。

## 理由

- **一开始就上双 OTA**：M7 才做 OTA，但分区表一旦发出去再改就要全片擦除重烧。
  现在定好，M1～M6 都跑在 ota_0 上，行为和以后一致。
- **不要 factory 分区**：4MB 放不下 factory 加两个足够大的 OTA 槽。回滚靠 ota_0 / ota_1 互备。
- **每个槽 1920K**：WiFi + HTTPS（mbedTLS）+ LVGL + 中文字库子集，估计在 1.2～1.5MB，留出余量。
- **app 分区按 64K 对齐**：ESP32-C3 的 MMU 页是 64K。

## 修订条件

如果 M0 实测 Flash 是 16MB，就把两个 OTA 槽和字库空间扩大，并更新本 ADR。
