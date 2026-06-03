# Tessera

A serial port debugging tool built with C++ and Qt.

> **Status:** Planning phase — no code yet. This README will evolve as the project takes shape.

## About

Tessera is a personal learning project: a GUI-based serial port tool (an "upper-computer" tool, in the embedded world's terminology) for communicating with microcontrollers and other serial devices.

The name comes from *tessera* — the small tiles that make up a mosaic. Serial communication is much the same: meaning emerges one byte at a time.

## Why this project

I'm a CS student moving toward embedded application development. Building a serial tool from scratch hits several things I want to learn at once:

- **C++ in a real application context**, not just exercises
- **Qt framework**, which is widely used in embedded GUI tooling
- **Serial communication protocols** and the practical debugging workflows around them
- **Cross-platform development** — Windows first, Linux later

A serial tool is small enough to actually finish, but deep enough to teach a lot.

## Tech stack

| Area | Choice | Reason |
|---|---|---|
| Language | C++ | Industry standard for embedded application tooling |
| Framework | Qt | Mature, cross-platform, well-documented |
| Serial I/O | QSerialPort | Official Qt module, no third-party dependencies |
| Primary reference | [Qt Terminal Example](https://doc.qt.io/qt-6/qtserialport-terminal-example.html) | Official, idiomatic Qt code |
| Target platform | Windows (initial), Linux (future) | |

## Planned features

Rough scope for the first usable version. Subject to change as I learn what actually matters.

- [ ] Open / close serial ports with configurable parameters (baud rate, data bits, stop bits, parity, flow control)
- [ ] Send and receive data in ASCII and HEX modes
- [ ] Display incoming data with timestamps
- [ ] Save session logs to file
- [ ] Multi-line send buffer with quick-send shortcuts
- [ ] Auto-reconnect on device reattachment

Stretch goals:

- [ ] Cross-platform support (Linux)
- [ ] Scripting / macro support for repeated test sequences
- [ ] Protocol-aware views (e.g., Modbus framing)

## Hardware for testing

No hardware in hand yet. Planned testing path:

1. **USB-to-TTL module** — cheapest way to get real serial traffic
2. **Arduino or STM32** — once basic functionality works, move to a real MCU for richer test scenarios

## Project structure

*(To be filled in once the codebase exists.)*

## Build

*(To be filled in once there's something to build.)*

## Roadmap

- **Phase 0 — Planning** *(current)*: scope, references, learning prep
- **Phase 1 — Walking skeleton**: open a port, send/receive one byte, prove the toolchain works end-to-end
- **Phase 2 — Core features**: the checklist above
- **Phase 3 — Polish**: usability, logging, configuration persistence
- **Phase 4 — Linux port**

## Learning log

I may keep notes on what I learn along the way — design decisions, things that surprised me, Qt quirks, embedded gotchas. If those notes get long, they'll move to a separate `NOTES.md`.

## License

*(To be decided.)*