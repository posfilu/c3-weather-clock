#!/usr/bin/env python
"""L2 冒烟测试：复位板子，读取串口日志，检查启动输出里是否出现预期内容。

在 ESP-IDF 环境里运行（用它自带的 pyserial）：
    python tools/smoke.py -p COM8
    python tools/smoke.py -p COM8 --duration 60   # 同时检查运行 60 秒内没有复位

M0 只检查开机日志；M3 起会加上"命令行能响应""能连上 WiFi"。
"""
import argparse
import re
import sys
import time

import serial

# 启动时必须出现的日志（正则），按出现顺序检查
EXPECTED = [
    r"hello, c3-weather-clock",
    r"chip: esp32c3",
    r"flash: configured \d+ KB, physical \d+ KB",
    r"alive, uptime",
]

# 出现任何一条就判定失败
FORBIDDEN = [
    r"Guru Meditation",
    r"abort\(\) was called",
    r"Stack smashing",
    r"Task watchdog got triggered",
    r"rst:0x[0-9a-f]+ \((?!POWERON|USB_UART_CHIP_RESET|RTC_SW_SYS_RST)",  # 非预期复位
]


def open_port(port: str, timeout_s: float = 10.0) -> serial.Serial:
    """USB-Serial-JTAG 复位后会重新枚举，COM 口短暂消失，这里重试打开。"""
    deadline = time.monotonic() + timeout_s
    while True:
        try:
            return serial.Serial(port, 115200, timeout=0.2)
        except serial.SerialException:
            if time.monotonic() > deadline:
                raise
            time.sleep(0.2)


def hard_reset(ser: serial.Serial) -> None:
    """与 esptool 对 USB-Serial-JTAG 的 hard reset 时序一致：拉一下 RTS。"""
    ser.dtr = False
    ser.rts = True
    time.sleep(0.1)
    ser.rts = False


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("-p", "--port", required=True)
    ap.add_argument("--duration", type=float, default=15.0, help="抓取时长（秒），默认 15")
    ap.add_argument("--no-reset", action="store_true", help="不复位，直接读")
    args = ap.parse_args()

    ser = open_port(args.port)
    if not args.no_reset:
        # 复位后不要关串口：USB-Serial-JTAG 在主机没在读的时候会直接丢弃输出，
        # 关掉再重开的这段空窗期正好会丢掉开机日志
        hard_reset(ser)

    lines = []
    buf = b""
    deadline = time.monotonic() + args.duration
    try:
        while time.monotonic() < deadline:
            try:
                chunk = ser.read(1024)
            except serial.SerialException:
                ser.close()
                ser = open_port(args.port)
                continue
            buf += chunk
            while b"\n" in buf:
                raw, buf = buf.split(b"\n", 1)
                line = re.sub(r"\x1b\[[0-9;]*m", "", raw.decode("utf-8", "replace")).rstrip("\r")
                lines.append(line)
                print(line)
    finally:
        ser.close()

    log = "\n".join(lines)
    failed = False
    pos = 0
    for pattern in EXPECTED:
        m = re.compile(pattern).search(log, pos)
        if not m:
            print(f"[smoke] FAIL: 没有找到 /{pattern}/", file=sys.stderr)
            failed = True
        else:
            pos = m.end()
    for pattern in FORBIDDEN:
        m = re.search(pattern, log)
        if m:
            print(f"[smoke] FAIL: 出现了 /{pattern}/：{m.group(0)}", file=sys.stderr)
            failed = True

    print("[smoke] FAIL" if failed else "[smoke] PASS", file=sys.stderr)
    return 1 if failed else 0


if __name__ == "__main__":
    sys.exit(main())
