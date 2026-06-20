# Tessera

A serial port debugging tool ("upper-computer", in embedded terms) built with C++ and Qt 6.

> **Status:** v1 complete. The core workflow — discover a port, configure it, connect, and send/receive in ASCII or HEX — is fully working. See [Roadmap](#roadmap) for what's intentionally left for later.

## About

Tessera is a GUI serial tool for talking to microcontrollers and other serial devices: open a port with the parameters you need, then send and receive data while watching a timestamped log of what's happening.

It started as a personal learning project — a CS student moving toward embedded application development — chosen because a serial tool is small enough to actually finish but deep enough to teach C++ in a real application, the Qt framework, serial protocols, and the debugging workflows around them.

The name comes from *tessera* — the small tiles that make up a mosaic. Serial communication is much the same: meaning emerges one byte at a time.

## Features

- **Port discovery & selection** — a popup lists available ports with full device detail: description, manufacturer, serial number, system location, and vendor/product IDs.
- **Configurable connection** — baud rate (1200–115200), data bits (5–8), stop bits (1 / 1.5 / 2), parity (none / even / odd / space / mark), and flow control (none / hardware / software). Connection parameters lock while a port is open.
- **ASCII & HEX I/O** — send and receive in either mode, independently for the send and receive sides.
- **Line endings** — append none / CR / LF / CRLF to each send.
- **Timed repeat send** — fire the same frame on a fixed interval with a dedicated Start/Stop control.
- **Receive view** — per-line arrival timestamps (toggle), autoscroll toggle, and live TX/RX byte counters.
- **Event log** — color-coded, timestamped log of connection and send events.
- **Theming** — a QSS template driven by JSON color palettes. Four built-in themes (Dark / Light × Gold / Blue), a writable user-themes folder for custom palettes, and hot-reload in debug builds.
- **Settings overlay** — a centered, in-window panel (currently the theme picker).
- **Status bar** — connection state and a ticking wall clock.

## Tech stack

| Area | Choice |
|---|---|
| Language | C++ (C++17) |
| Framework | Qt 6 (Widgets) |
| Serial I/O | QSerialPort |
| Build system | CMake + Ninja |
| Toolchain | MinGW (Qt 6.11.1 tested) |
| Reference | [Qt Terminal Example](https://doc.qt.io/qt-6/qtserialport-terminal-example.html) |
| Target platform | Windows (supported) — see [Platform support](#platform-support) |

## Project structure

The codebase is split into layers so the serial logic stays independent of the UI:

```
tessera/
├── core/        Serial communication logic — static library, no Widgets dependency
│   ├── serialport.*   open/close/send + readyRead → data_received signal
│   ├── portinfo.*     enumerate ports into a plain PortInfo struct
│   └── core.*         version()
├── gui/         User interface — executable, depends on core + Widgets
│   ├── mainwindow.*       main window: connection, send/receive, repeat, logging
│   ├── portpopup.*        port list + per-device detail panel
│   ├── settingsdialog.*   floating settings overlay
│   ├── thememanager.*     QSS template + palette engine, hot-reload
│   ├── styles/theme.qss   the @token@ stylesheet skeleton
│   └── themes/*.json      color palettes (built-in themes)
├── tools/       coretest — console harness that links core only (no GUI)
├── deploy.ps1   build + bundle Qt/MinGW DLLs via windeployqt
└── run.ps1      build, bundle, and launch
```

The `gui` layer depends on `core`; `core` never depends on Widgets. That boundary is enforced in `core/CMakeLists.txt`, and `tools/coretest` exists to exercise `core` standalone (port enumeration and a send/receive smoke test).

## Build

**Prerequisites**

- Qt 6.5 or newer with the SerialPort and Widgets modules (developed against 6.11.1)
- CMake 3.19+
- Ninja
- A C++ compiler (MinGW on Windows)

**Quick start (Windows / PowerShell)**

```powershell
./run.ps1            # build, bundle Qt DLLs, and launch the GUI
./run.ps1 --test     # build and run the coretest console harness instead
```

`run.ps1` calls `deploy.ps1`, which configures CMake on first run, builds, and runs `windeployqt` so the executables run standalone (no need to put Qt's `bin` on `PATH`). If your Qt install lives elsewhere, override the paths via environment variables before running:

```powershell
$env:QT_ROOT   = "D:/Qt/6.12/mingw_64"
$env:MINGW_BIN = "D:/Qt/Tools/mingw1310_64/bin"
$env:NINJA     = "D:/Qt/Tools/Ninja/ninja.exe"
./deploy.ps1
```

**Manual CMake**

```bash
cmake -S . -B build -G Ninja -DCMAKE_PREFIX_PATH=<path-to-qt>
cmake --build build
```

## Platform support

**Windows is the supported and tested platform.** The codebase deliberately avoids
platform-specific APIs — serial I/O goes through Qt's cross-platform `QSerialPort`,
and the build is plain CMake — so it should compile on Linux and macOS as well.
Those builds are currently **untested and unsupported**, though.

The `.ps1` helper scripts (`run.ps1`, `deploy.ps1`) are Windows-only conveniences.
On other platforms, build with CMake directly (see [Build](#build)).

## Custom themes

Each theme is a small JSON palette (see `gui/themes/*.json`); the stylesheet itself lives in `gui/styles/theme.qss` and refers to colors by `@token@`. To add a theme, drop a new `<name>.json` into the user themes folder (under the per-user AppData directory, `Tessera/themes`) — no code or template changes needed. On a name clash, the user file wins. Debug builds hot-reload both the template and palettes as you edit them.

## Roadmap

Intentionally left for later versions:

- [ ] Save session logs to a file
- [ ] Auto-reconnect on device reattachment
- [ ] Quick-send buffer / macro shortcuts for repeated test sequences
- [ ] Protocol-aware views (e.g. Modbus framing)
- [ ] Tested Linux support (the code is platform-clean, but Linux builds are currently untested)

## License

Released under the [MIT License](LICENSE).
