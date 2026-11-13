# Testing and validation

## Wokwi wiring views (2026-11-13)

See [WOKWI.md](WOKWI.md) for two color-coded diagrams, model limitations,
physical wiring tables, numbered instructions, and opening/reproduction steps.
Official Wokwi CLI 0.29.1 lint passed both files (exit 0, one DevKitC V4
informational notice each). `tests/check_wokwi.py` passed GPIO/net, button,
power, TOML, and asset checks for both views. Passive custom-chip WebAssembly
assets validated and instantiated in the local WebAssembly engine.

PlatformIO build SUCCESS in 2.84 seconds; all 11 existing safety checks passed.
Firmware, PlatformIO settings, and relay/buzzer simulation defaults are unchanged.
No simulation, rendering inspection, flashing, or physical electrical tests were
performed. DHT11 is explicitly a wiring-only placeholder, not a DHT22 replacement
or a functioning sensor simulation. Physical board header positions remain to
be confirmed from its silkscreen; GPIO labels match the selected 38-pin model.

## Phase 2: initial runtime evidence (2026-11-13)

**Initial USB-only firmware runtime test: PASS (user-observed evidence).**
The user confirmed that only USB is connected to the ESP32-WROOM-32. OLED,
DHT11, and buttons are not connected. Relay module, buzzer, heater, humidifier,
motor, and all mains-powered loads are disconnected.

The user reports repeated output in Serial Monitor at 115200 baud:

```text
[sensor] ERROR: disconnected or invalid DHT11 reading
```

This demonstrates execution of the application's sensor polling/error-reporting
path and serial output. It is expected with the DHT11 disconnected and is not
evidence of a firmware defect. Timing was not measured; startup output, continuous
uptime, and the complete alert sequence were not supplied. This is a limited
initial runtime pass, not validation of the full application or sensors.

| Check | Evidence/status |
|---|---|
| USB-only runtime and serial output | User confirmed the repeated message above |
| Sensor-disconnected reporting | Observed by user; expected for this setup |
| Startup banner and initialization order | Pending startup capture |
| DHT11 measurements and recovery | Not tested; sensor disconnected |
| OLED startup/live/error screens | Not tested; display disconnected |
| Buttons and Preferences across power cycles | Not tested |
| Wi-Fi/dashboard and time synchronization | Not tested |
| Relay/buzzer simulation defaults | Source checks; physical outputs not tested |

Documentation validation: PlatformIO build SUCCESS in 2.84 seconds; all 11
existing static safety checks passed. RAM 33,944 bytes (10.4%), flash 608,730
bytes (46.4%). Only documentation changed; firmware and safety defaults were
not modified, and no flash/reset/serial-port operation was performed by the agent.

### Safe serial procedure for the next bench session

1. Retain the confirmed USB-only baseline and leave all output hardware and loads
   disconnected. Do not flash, erase, or change firmware for these tests.
2. Close other serial tools. Confirm the current ESP32 port using
   `platformio device list`; COM5 was previously identified as the CH340 bridge.
3. From the workspace, open a 115200 baud, 8-N-1 monitor with no flow control:

   ```powershell
   & "$env:USERPROFILE\.platformio\penv\Scripts\platformio.exe" device monitor --port COM5 --baud 115200 --dtr 0 --rts 0
   ```

   DTR/RTS are requested inactive to avoid intentional auto-reset; USB driver
   transitions can still reset some boards when opening the port. Do not use
   monitor reset commands or press EN/BOOT automatically. If startup capture is
   needed, obtain the user's agreement before a manual reset/power cycle. An
   already-running board may emit only periodic readings/errors.
4. Capture 30-60 seconds, keeping logs local (for example `runtime.log`, ignored
   by git). Record connected hardware and visible screen behavior. Redact network
   identifiers before sharing. Exit with Ctrl+C; never run upload as a test step.
5. Compare with the expected messages below. Record only what was actually seen.
   Repeated reboot banners, brownout/panic output, or stalled polling need review.

Expected messages from source review, **not captured startup evidence**:

- Banner: `Smart Incubator v1.0 - monitoring prototype`, followed by the safety
  notice that heater, humidifier, and physical buzzer outputs are disabled.
- `[settings] Preferences loaded`, or an explicit storage error/defaults warning.
  A loaded message alone does not prove persistence across power cycles.
- `[sensor] DHT11 driver initialized` means the driver was started, not that a
  sensor is present. The disconnected message is expected about every two seconds
  with this USB-only setup. The DHT protocol transaction itself is synchronous.
- Button initialization and relay/buzzer simulation notices should follow.
- The installed SSD1306 library's `begin()` does not verify I2C acknowledgement.
  Firmware has no display-success serial message. Its `SSD1306 not detected`
  error text is not a reliable missing-display diagnostic; absence of that error
  does not prove display presence. Confirm OLED rendering visually. Improving
  display probing/logging is future firmware work requiring user approval.
- With driver initialization completed but invalid sensor readings, the software
  alert is `invalid reading: ACTIVE (buzzer simulated)`. It logs on state changes,
  not every poll. A disconnected DHT11 is not separately identified electrically.
- `[wifi] disabled: ...` is expected without credentials. Neither offline operation
  nor a sensor warning should enable physical outputs.

### Next hardware phase: OLED, then DHT11

These are **pending tests**, not completed hardware validation. Use the exact
connection table and diagram in [WIRING.md](WIRING.md). Disconnect USB before
every wiring change; do not move powered breadboard leads.

1. **OLED only:** verify module pin labels and 3.3 V compatibility before wiring
   VCC to 3V3, GND to GND, SDA to GPIO21, SCL to GPIO22. Reconnect USB and observe
   the startup screen, then sensor-error placeholders (DHT11 still absent).
   Record whether text is legible and stable. The configured driver is SSD1306,
   128x64, address 0x3C; controller and actual address remain unverified. If blank,
   power down and check wiring/controller/address; stop before any firmware change
   or diagnostic-scanner upload. See HARDWARE.md for SH1106 considerations.
2. **Add DHT11:** with USB removed, verify the module's labeled VCC/DATA/GND pins,
   3.3 V operation, and onboard DATA pull-up to 3.3 V. Connect VCC to 3V3, GND to
   GND, DATA to GPIO4. Do not assume pin order or use a 5 V data pull-up. If the
   module lacks a suitable pull-up, stop to identify a suitable available resistor
   before continuing. Reconnect and allow several polling intervals to settle.
3. Capture at least 30 seconds of finite temperature/humidity values and compare
   them to the OLED readings. Confirm the invalid-reading alert clears after valid
   samples. This validates basic reporting only, not sensor accuracy/calibration.
4. **Failure/recovery:** power down, disconnect the DHT11, restart, and confirm
   repeated sensor errors plus OLED error placeholders. Power down again, restore
   wiring, and verify readings recover. Keep all relay/buzzer/load hardware absent.
5. Leave buttons disconnected during these first tests. Button navigation,
   persistence, Wi-Fi loss/recovery, and calibrated reference checks remain later
   steps. DHT11 measurements must never be used to activate a heater.

## Physical board upload confirmed on 2026-11-13

- The user confirmed a successful firmware upload to the physical ESP32 through
  COM5: **SUCCESS**, elapsed time **92.43 seconds**. This upload result is
  user-reported; the upload was not rerun during Git validation.
- Windows pre-upload diagnostics detected USB-SERIAL CH340 on COM5 with status
  OK and a signed WCH driver (3.9.2024.9).
- Application runtime, startup serial logs, and external sensors are **not yet
  verified**. Successful flashing does not establish correct DHT11/OLED/button
  behavior, settings persistence, Wi-Fi operation, or electrical safety.
- Relay and buzzer hardware remain disabled in firmware. Physical outputs have
  not been activated or validated. The static source checks are software checks.
- Pre-commit validation: `platformio run -e esp32dev` completed with SUCCESS
  in 3.12 seconds using the installed dependencies. RAM remained 33,944 bytes
  (10.4%); flash remained 608,730 bytes (46.4%). All 11 checks in
  `tests/check_source.ps1` passed. No firmware was uploaded during this validation.

## Build recovery verified on 2026-11-13

- Installed official PlatformIO IDE 3.3.4 and its Microsoft C/C++ dependency
  1.35.6 in the VS Code user profile. Portable Python 3.11.7 and PlatformIO
  Core 6.2.0 are isolated under `%USERPROFILE%\.platformio`; system Python,
  system PATH, and persistent PowerShell execution policy were not changed.
- Espressif32 platform: 8.0.0; Arduino-ESP32: 3.3.12
  (`framework-arduinoespressif32` package 4.30312.261024).
- Libraries: Adafruit GFX 1.13.0, SSD1306 2.5.17, DHT sensor library 1.4.7,
  Unified Sensor 1.1.15, and transitive BusIO 1.17.4. Framework libraries
  Wire, WiFi, Preferences, and WebServer are version 3.3.12.
- Initial build: SUCCESS (474.79 seconds including dependency installation).
  Clean target: SUCCESS (0.68 seconds). Full rebuild: SUCCESS (45.51 seconds).
- RAM: 33,944 / 327,680 bytes (10.4%). Flash: 608,730 / 1,310,720 bytes (46.4%).
- All 11 existing source contract checks passed. Manual source inspection also
  confirmed dashboard GET handlers only serve HTML/status and do not call relay
  request methods. All 12 assigned GPIOs are unique. Relay and buzzer managers
  contain no physical output calls; both simulation defaults remain true.
- No firmware source changes were required. The existing VS Code build task
  now uses the Core executable directly instead of depending on `pio` on PATH.
- No compiler/linker warnings appeared in the clean build log, saved locally at
  `.pio/build-recovery.log` (ignored by git). Download interruptions and an HTTP
  503 recovered through PlatformIO mirror retries. The extension installation
  emitted a Node `url.parse()` deprecation warning.
- Dependencies remain unpinned in `platformio.ini`; these are the versions
  verified here, not a guarantee of future resolution on a fresh installation.
- No upload, physical hardware test, or live dashboard test was performed.
  Extension installation was verified, but activation of its sidebar in the
  already-running VS Code window was not confirmed. Use **Developer: Reload
  Window** if the PlatformIO toolbar is missing. The existing build task works
  independently of extension activation.

Build from the workspace in PowerShell without changing PATH:

```powershell
& "$env:USERPROFILE\.platformio\penv\Scripts\platformio.exe" --version
& "$env:USERPROFILE\.platformio\penv\Scripts\platformio.exe" run -e esp32dev -t clean
& "$env:USERPROFILE\.platformio\penv\Scripts\platformio.exe" run -e esp32dev
powershell.exe -NoProfile -ExecutionPolicy Bypass -File tests\check_source.ps1
```

Alternatively use **Terminal > Run Build Task > PlatformIO: Build ESP32**.

Only after explicit upload approval, connect the ESP32 over USB with relay,
buzzer, and loads disconnected. Identify its actual port (do not assume COM3):

```powershell
& "$env:USERPROFILE\.platformio\penv\Scripts\platformio.exe" device list
# Replace COMx below with the identified ESP32 port, after upload approval.
& "$env:USERPROFILE\.platformio\penv\Scripts\platformio.exe" run -e esp32dev -t upload --upload-port COMx
& "$env:USERPROFILE\.platformio\penv\Scripts\platformio.exe" device monitor --port COMx --baud 115200
```

## Build and static checks

- Build for `esp32dev` with PlatformIO: `pio run`.
- Run source contract checks in PowerShell:
  `powershell.exe -NoProfile -ExecutionPolicy Bypass -File tests\check_source.ps1`.
- PlatformIO dependencies are declared in `platformio.ini`; do not change framework
  versions or dependencies without re-running the build.
- Review `src\relay_manager.cpp` and `src\buzzer_manager.cpp`: neither may configure
  output pins or call GPIO/tone output APIs.
- Confirm GPIO uniqueness against `include\config.h` and the assignment table in
  [WIRING.md](WIRING.md).
- No physical board was flashed or verified during project creation. Do not report
  breadboard, sensor, display, relay, or Wi-Fi hardware behavior as tested unless
  separately performed.

## Safe bench checks (after reviewing wiring)

With only low-voltage USB power and no relay/buzzer/mains connections:

1. Build, then obtain explicit approval before uploading. Select the ESP32 serial
   port in PlatformIO and use **PlatformIO: Upload**; do not enable automatic upload.
2. Open the serial monitor at 115200 baud. Verify startup logs and safe-output notice.
3. Check DHT11 readings update about every two seconds; unplug its data lead only
   while powered down if deliberately testing a sensor failure.
4. Verify the display shows startup, live values, sensor error, and settings screens.
   If the display does not initialize, check controller, address, wiring, and 3.3 V.
5. Test all five buttons and verify debounce produces one action per press. The TURN
   button must only produce a serial simulation notice.
6. Change settings, power-cycle, and verify values persist. Check temperature range
   35–40 °C and humidity bounds reject out-of-range values.
7. Leave Wi-Fi credentials unset to verify offline startup. If credentials are
   configured, check dashboard connection/disconnection and confirm it is read-only.
8. Verify relay and buzzer behavior only in logs; do not attach or energize the modules.
9. Inspect all wiring with power removed before reconnecting USB.

These are instructions, not completed hardware tests. Automated hardware tests,
calibration, and physical safety validation remain outstanding.

## Upload after approval

1. Confirm the user explicitly approves flashing the board.
2. Connect only the low-voltage ESP32 by USB; keep relay, buzzer, and loads disconnected.
3. In VS Code, choose **PlatformIO: Build**, then **PlatformIO: Upload** and select
   the correct serial port if prompted.
4. Open **PlatformIO: Monitor** at 115200 baud and review startup/error messages.
5. Stop immediately if wiring, board identity, or serial output differs from expectations.
