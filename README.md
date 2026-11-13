# Smart Incubator — monitoring prototype

PlatformIO / Arduino firmware for an ESP32-WROOM-32 prototype. This is a
**monitoring and configuration project, not a safe incubator controller**.
The DHT11 is not accurate enough for incubation control. No heater, humidifier,
egg-turning motor, or physical buzzer is activated by this firmware.

## Implemented

- DHT11 temperature/humidity sampling on a non-blocking 2-second schedule.
- SSD1306 128×64 I2C display with startup, readings, sensor-error, and settings screens.
- Five debounced buttons; settings persisted in ESP32 `Preferences`.
- Configurable illustrative chicken-egg settings and warning thresholds.
- Simulated-only relay requests and software alert events.
- Optional Wi-Fi station connection and read-only, responsive local dashboard.
- SNTP-based incubation day tracking; the first valid synchronized time anchors day 1
  and that epoch is stored in Preferences. Until time is valid, the displayed day is 00.
- No outputs are configured as active GPIOs. Wi-Fi is not required for local monitoring.

See [HARDWARE.md](HARDWARE.md), [WIRING.md](WIRING.md), [SAFETY.md](SAFETY.md),
[TESTING.md](TESTING.md), and [ROADMAP.md](ROADMAP.md) before connecting hardware.

Color-coded Wokwi wiring views and beginner breadboard instructions are in
[WOKWI.md](WOKWI.md). The DHT11 part is a clearly labeled wiring-only placeholder;
it does not emulate sensor readings, and no DHT22 is substituted.

## Build

1. Install Visual Studio Code, then open **Extensions** and install **PlatformIO IDE**
   (publisher: PlatformIO).
2. Open this folder as a PlatformIO project.
3. Allow PlatformIO to install the `espressif32` platform and libraries declared in
   `platformio.ini`.
4. Build using **PlatformIO: Build** or run `pio run`. A
   **PlatformIO: Build ESP32** workspace task is also provided.
5. To use Wi-Fi, copy `include\secrets.h.example` to `include\secrets.h` and replace
   the placeholders. `include\secrets.h` is ignored by git. Wi-Fi remains disabled
   when no SSID is configured.
6. To upload, first follow the approval and safety steps in [TESTING.md](TESTING.md).
   No firmware has been flashed as part of this project setup.

## Serial monitor

Set the monitor to **115200 baud** (`pio device monitor -b 115200`). Firmware logs
sensor state, settings persistence, connection state, and simulated alert/relay
events. A disconnected Wi-Fi network does not stop local sensor/button/display
operation.

## Limitations

- DHT11 readings are demonstration-only; no autonomous temperature/humidity control.
- Relays and buzzer are software simulations. The GPIOs are never configured or driven.
- The turn button only logs a simulated request, and only when a valid incubation day
  and turning-enabled range are known.
- Incubation-day start is anchored on the first successful SNTP time after startup;
  verify/reset this behavior for any future real incubation workflow.
- Physical hardware operation and electrical compatibility have not been verified.
