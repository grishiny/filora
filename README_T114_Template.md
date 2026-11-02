# Heltec T114 — Bare-bones PlatformIO Template

A minimal PlatformIO Arduino project for the **Heltec Mesh Node T114** (nRF52840 + SX1262). The T114 is not in PlatformIO's official board registry and there is almost no documentation on setting one up outside of the [Meshtastic](https://meshtastic.org/) firmware. This repo extracts exactly what is needed to get a clean standalone project running.

This template targets the variant **without GPS and without TFT screen**. If you need to build for these, check the [meshtastic variant files for these versions](https://github.com/meshtastic/firmware/tree/develop/variants/nrf52840/heltec_mesh_node_t114).

What's working from the VSCode PlatformIO Extension?
* Build
* Upload
* Monitor

An example main.cpp is provided which inits LoRa, flashes the neopixel and prints hello to serial. Also ships with RadioLib by default for the SX1262.

---

## Hardware

| | |
|---|---|
| MCU | Nordic nRF52840 (Cortex-M4F, 64 MHz) |
| Radio | Semtech SX1262 |
| Flash | MX25R1635F QSPI (2 MB) |
| RGB LEDs | 2× WS2812B (NeoPixel) |
| Button | 1× user button |
| Bluetooth | BLE 5.0 (integrated) |
| USB | USB-C, CDC serial + UF2 bootloader |

---

## Prerequisites

1. **[PlatformIO](https://platformio.org/)** — CLI or VS Code extension
2. **Python 3** — required by PlatformIO's build scripts
3. **nrfutil** — only needed for USB serial flashing (not needed for UF2 drag-and-drop)
   ```
   oi
   ```
4. The board must have the **Adafruit nRF52 bootloader** flashed. Factory T114 units ship with it.

---

## Quick Start

```bash
git clone https://github.com/Nateyo/Heltec-T114-PlatformIO-Base
cd Heltec-T114-PlatformIO-Base
pio run
```

PlatformIO will automatically download the platform, BSP, and libraries on first build.

Build output is in `.pio/build/heltec-mesh-node-t114/`:

| File | Use |
|---|---|
| `firmware.uf2` | Drag-and-drop flash via USB |
| `firmware.hex` | Flash via JLink / nrfjprog |
| `firmware.bin` | Raw binary |

---

## Flashing

### Option A — UF2 (manual drag-and-drop)

1. Double-tap the reset button. The board mounts as a USB drive.
2. Drag `firmware.uf2` onto the drive.
3. The board reboots automatically into your firmware.

### Option B — nrfutil via USB (easiest!)

```bash
pio run --target upload
```

This uses the `nrfutil` DFU protocol over USB serial. The bootloader is not touched.

---

## Serial Monitor

```bash
pio device monitor
```

Or use the PlatformIO IDE serial monitor. Baud rate is 115200.

---

## Pin Reference

### LoRa (SX1262) — SPI0

| Signal | Pin |
|---|---|
| CS / NSS | P0.24 |
| DIO1 | P0.20 |
| BUSY | P0.17 |
| RESET | P0.25 |
| MISO | P0.23 |
| MOSI | P0.22 |
| SCK | P0.19 |

DIO2 is wired internally as the RF switch. DIO3 drives the TCXO at 1.8 V. Neither should be driven from the MCU.

### I²C

| Bus | SDA | SCL | Notes |
|---|---|---|---|
| Wire (0) | P0.26 | P0.27 | RTC footprint — unpopulated on V1 |
| Wire1 (1) | P0.16 | P0.13 | Header pins, general use |

### Other

| Function | Pin |
|---|---|
| Green LED (active-low) | P1.03 |
| NeoPixel data | P0.14 |
| User button | P1.10 |
| Battery ADC | P0.04 (AIN2) |
| ADC enable | P0.06 (drive HIGH to enable divider) |
| QSPI SCK | P1.14 |
| QSPI CS | P1.15 |
| UART1 RX | P1.07 |
| UART1 TX | P1.05 |

---

## Using the LoRa Radio

[RadioLib](https://github.com/jgromes/RadioLib) is included as a dependency. The pin constants from `variant.h` map directly to RadioLib's `Module` constructor:

```cpp
#include <RadioLib.h>

SX1262 radio = new Module(SX126X_CS, SX126X_DIO1, SX126X_RESET, SX126X_BUSY);

void setup() {
    // 915 MHz, 10 dBm, BW 125 kHz, SF 7, CR 4/5
    int state = radio.begin(915.0, 125.0, 7, 5, RADIOLIB_SX126X_SYNC_WORD_PRIVATE, 10);
}
```

Change `915.0` to `868.0` for EU868 or `923.0` for AS923.

---

## Project Structure

```
t114/
├── boards/
│   └── heltec_mesh_node_t114.json   # Board definition (not in PlatformIO registry)
├── variants/
│   └── heltec_mesh_node_t114/
│       ├── variant.h                # Pin definitions
│       └── variant.cpp              # Pin map + hardware init
├── extra_scripts/
│   └── nrf52_uf2.py                 # Post-build: .hex → .uf2
├── bin/
│   └── uf2conv.py                   # UF2 conversion utility
├── src/
│   └── main.cpp                     # Your application
└── platformio.ini
```

---

## Why a Custom BSP?

The `platform_packages` entry in `platformio.ini` uses [Meshtastic's fork](https://github.com/meshtastic/Adafruit_nRF52_Arduino) of the Adafruit nRF52 Arduino BSP rather than the upstream version. The fork is needed because:

- The upstream BSP does not include the `heltec_mesh_node_t114` variant
- It adds C++17 compiler support (`-std=gnu++17`)
- It carries the correct SoftDevice (S140 v6.1.1) and linker configuration for this board

The fork tracks upstream closely. You can follow its commit history to assess drift.

---

## License

Variant files carry the original LGPL-2.1 header from the Adafruit nRF52 Arduino BSP.  
`bin/uf2conv.py` is sourced from the [Meshtastic firmware](https://github.com/meshtastic/firmware) repository.  
Everything else in this repo is released under MIT.
