# Testing and validation

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
