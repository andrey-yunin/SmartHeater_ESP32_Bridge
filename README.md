# 🌡️ SmartHeater ESP32 Bridge (v1.2)

[![PlatformIO](https://img.shields.io/badge/PlatformIO-Compatible-orange.svg)](https://platformio.org/)
[![Framework](https://img.shields.io/badge/Framework-Arduino/FreeRTOS-blue.svg)](https://www.freertos.org/)
[![License](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE)

**SmartHeater Bridge** is a high-performance **ESP32-based** gateway designed to control an intelligent heating system. It acts as a bridge between the low-level controller (STM32) and a modern Web UI.

---

## 🚀 Key Features
*   **Dual-Core Architecture (FreeRTOS):**
    *   `Core 0`: Network stack handling (Wi-Fi, Web Server).
    *   `Core 1`: Time-critical UART packet processing.
*   **Protocol v1.2:** Robust binary protocol with parity check (CRC8) and event-driven data model.
*   **Modern Web UI:** Responsive interface (Dark Mode / Glassmorphism) built with Vanilla JS, served from LittleFS.
*   **Watchdog & Fail-Safe:** 
    *   System Watchdog (65s) for communication monitoring.
    *   Automatic handling of sensor errors (`-999` code).
*   **High Reactivity:** Instant relay status updates (Event-driven) without waiting for scheduled polls.

---

## 🏗️ System Architecture
The project is divided into functional layers to ensure modularity:
1.  **Drivers Layer:** Low-level hardware abstraction (UART, FIFO, WiFi, Storage).
2.  **Services Layer:** Business logic (Protocol Parser, API JSON mapping, Command Queue).
3.  **Tasks Layer:** Parallel FreeRTOS threads (`Uart_Task`, `Network_Task`).
4.  **Presentation Layer:** Static UI (HTML/CSS/JS) located in the LittleFS filesystem.

*Detailed architectural decisions can be found in [ARCHITECTURE.md](ARCHITECTURE.md).*

---

## 📡 UART Communication Protocol
| Command | Description | Period |
| :--- | :--- | :--- |
| `0x03` | Current Temperatures | 30 sec |
| `0x04` | System Health & Relay Status | 5 min / Event-driven |
| `0x13` | Set Target Temperature | On Request |
| `0x10` | PING (Watchdog) | 65 sec of silence |

*Full specification is available in [lib/protocol/UART_PROTOCOL.md](lib/protocol/UART_PROTOCOL.md).*

---

## 🛠️ Build & Flash (PlatformIO)

### 1. Prerequisites
Ensure you have **VS Code** with the **PlatformIO** extension installed.

### 2. Build Firmware
```bash
# Build the main firmware
pio run
# Upload firmware to the chip
pio run --target upload
```

### 3. Filesystem Setup (UI)
**Important:** Web interface files are uploaded separately to the LittleFS partition.
```bash
# Build LittleFS image (from data/ folder)
pio run --target buildfs
# Upload image to the chip
pio run --target uploadfs
```

---

## 📅 Roadmap
- [ ] **NVS Persistence:** Save target temperature to non-volatile memory.
- [ ] **mDNS Discovery:** Access device via `http://heater.local`.
- [ ] **OTA Updates:** Wireless firmware updates via Web UI.
- [ ] **WebSockets:** Migrate from Polling to WebSockets for sub-100ms updates.

---

## 🤝 Development Standards
The project follows Senior-level engineering practices:
*   Implementation of Singleton and Snapshot patterns.
*   Minimization of heap fragmentation via static memory allocation.
*   Thread safety through Semaphores/Mutexes.

---
© 2026 SmartHeater Project. 
