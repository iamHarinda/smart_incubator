# Smart Incubator: two wiring diagrams

## What these files provide

- `diagram.json` + `wokwi.toml`: ESP32, full-size breadboard, DHT11 wiring only.
- `wokwi/full/diagram.json` + `wokwi/full/wokwi.toml`: adds OLED and five buttons.
- `tools/generate_wokwi.py`: reproduces both views and custom-chip assets.
- `tests/check_wokwi.py`: verifies the diagram nets against `include/config.h`.

**DHT11 is a wiring placeholder, not a simulated sensor.** Wokwi's official
[supported hardware list](https://docs.wokwi.com/getting-started/supported-hardware)
lists DHT22, not DHT11. Neither diagram substitutes a DHT22 or changes firmware's
DHT11 mode. The labeled custom part uses S / VCC / GND, corresponding to your
module's S / + / -. Its deliberately empty WebAssembly `chip_init` leaves pins
unconfigured and produces no pulses or readings. Expect sensor errors if a
simulation is started. It cannot validate DHT11 measurements, timing, pull-ups,
or electrical compatibility. Custom-chip pins are shown as a generic package,
not the physical DHT11 module shape/header order.

The small `.chip.wasm` files are intentional simulation assets, not ESP32 build
artifacts. They are reproducible from the documented empty module bytes in the
generator; no compiler, third-party binary, or sensor model is hidden in them.

## Board and breadboard differences

Wokwi recommends `board-esp32-devkit-c-v4` for PlatformIO `esp32dev` in its
[PlatformIO guide](https://docs.wokwi.com/vscode/platformio). Its official
[board definition](https://github.com/wokwi/wokwi-boards/blob/main/boards/esp32-devkit-c-v4/board.json)
has two 19-pin headers. Used endpoints are `3V3`, `GND.1`, `4`, `21`, `22`,
`18`, `19`, `23`, `16`, and `17`; numeric endpoints map to the same GPIO numbers.
The hidden TX/RX connections serve only Wokwi's serial monitor; do not add
physical USB/UART wires. Do not confuse DevKitC's `D2` (GPIO9) with GPIO2.

This verifies the selected simulated model's names, not the physical pin order
of your particular 38-pin USB-C clone. A photograph/manufacturer pinout has not
been supplied. Confirm every physical silkscreen label before wiring. USB socket,
regulator, dimensions, ground-pin positions, and header layout may differ.
GPIO16/17 are appropriate for the stated WROOM board; do not transfer this map
to a PSRAM/WROVER variant without checking its reserved pins.

Wokwi's full-size breadboard represents the MB-102 connection pattern, not a
verified model of your exact board. Components are drawn beside the breadboard
for legibility and connect through jumpers; the drawing is not an insertion-hole
template. Only **the left portion of the top + and - rails** is used (rail holes
1-12 in Wokwi). Bottom rails and the right-hand rail sections are unused.
Never assume an MB-102 rail is continuous across a center gap or connected to
the opposite rail. With USB removed, check continuity if a meter is available;
otherwise keep all connections in one visibly uninterrupted local section.
If unsure, stop before powering. No meter or extra hardware is assumed available.

## Color-coded connection table

Firmware GPIO names are the source of truth; header position numbers are not.

| Connection | ESP32 / breadboard | Wire color |
|---|---|---|
| ESP32 3V3 | top-left + rail, Wokwi `tp.1` | Red |
| ESP32 GND | top-left - rail, Wokwi `tn.1` | Black |
| DHT11 + (VCC) | same + rail, `tp.3` | Red |
| DHT11 - (GND) | same - rail, `tn.3` | Black |
| DHT11 S | GPIO4 | Green |
| OLED VCC | same + rail, `tp.6` | Red |
| OLED GND | same - rail, `tn.6` | Black |
| OLED SDA | GPIO21 | Blue |
| OLED SCL | GPIO22 | Yellow |
| MENU contact 1 | GPIO18 | Orange |
| UP contact 1 | GPIO19 | Purple |
| DOWN contact 1 | GPIO23 | Cyan |
| OK contact 1 | GPIO16 | Magenta |
| TURN contact 1 | GPIO17 | Brown |
| Each button contact 2 | same - rail, `tn.8` through `tn.12` respectively | Black |

Each wire's endpoints are also visible in JSON. Labels identify the part, signal,
GPIO, supply, and ground. Suggested colors need not match jumper insulation you
own; electrical endpoints must match. Keep all output hardware disconnected.

## Numbered physical breadboard steps

1. Remove USB. Leave relays, buzzer, heater, humidifier, motors, mains, and unused
   sensors disconnected. Keep the ESP32 beside the breadboard if its width covers
   usable holes; do not force it into a particular row layout.
2. Identify 3V3 and GND by the actual board labels. Connect 3V3 with red wire to
   the selected top-left + rail section and GND with black to its - section.
   The + marking means the assigned **3.3 V**, not 5 V. Do not connect VIN/5V.
3. Inspect DHT11 module labels, 3.3 V supply compatibility, and its DATA pull-up.
   Route S to GPIO4, + to the red rail, - to the black rail. If inserting its
   header into the board, give each pin a separate isolated numbered strip;
   never put two module pins in the same five-hole connected strip. Follow labels,
   not assumed left-to-right order. Confirm a pull-up goes to 3.3 V, not 5 V;
   if absent/unknown, stop and identify an appropriate available resistor first.
4. Inspect for shorts and reconnect USB for the DHT11-only test. Record actual
   serial readings at 115200 baud; a drawn connection is not evidence of success.
   Remove USB before the next step.
5. Verify OLED labels/controller/address and 3.3 V compatibility. Connect VCC/GND
   to the same rails, SDA to GPIO21, SCL to GPIO22. Firmware assumes SSD1306
   128x64 at 0x3C; do not infer your module's order from Wokwi. Reconnect USB,
   observe startup/live/error screens, then remove power again.
6. Identify the two separate contacts of each tactile switch. On Wokwi buttons,
   `1.l` and `1.r` are always joined; `2.l` and `2.r` are always joined. Connect
   GPIO to `1.l` and ground to `2.l`, not two pins of contact 1. Real switch
   orientation must be checked; straddle the breadboard center trench with the
   separate contacts on isolated strips. Five-hole strips on either side of
   the trench are separate; do not bridge them accidentally.
7. Wire MENU/UP/DOWN/OK/TURN using the table, with the other contact of every
   switch grounded. No switch goes to the positive rail. Firmware uses internal
   pull-ups: released HIGH, pressed LOW. Leave spare switch legs unused.
8. Recheck polarity, isolated strips, and every GPIO with USB removed. Power only
   by USB for supervised testing. Check one button at a time; TURN may report
   unavailable until valid time/day exists and never drives a motor. Record
   results using TESTING.md; no new firmware upload is needed for this wiring.

## Open the diagrams

1. Use the official **Wokwi Simulator** VS Code extension. It is not installed
   by this change; extension activation/licensing is managed by Wokwi.
2. Open this repository folder, then click `diagram.json` in the Explorer to
   open the diagram editor. To run later, build first and
   use **Wokwi: Start Simulator**. This runs a virtual MCU, not a USB upload.
3. For the second view, open `wokwi/full/diagram.json`. Before starting that view,
   use **Wokwi: Select Config File** and select `wokwi/full/wokwi.toml`. Its paths
   point to the same root build artifacts. Select root `wokwi.toml` to return to
   the first view. Keep the repository as your workspace; do not overwrite either
   diagram. Build from repository root.
4. Both TOML files reference the existing PlatformIO firmware/ELF with forward
   slashes. No credentials, CLI tokens, or local settings belong in these files.

See the official [diagram editor](https://docs.wokwi.com/vscode/diagram-editor)
and [configuration reference](https://docs.wokwi.com/vscode/project-config).
Wokwi account/license or CLI token requirements may apply to starting simulation.
No simulation session, rendered layout inspection, or electrical test is claimed
by these files. A successful lint only establishes diagram structural validity.

## Reproduce checks

From the repository root in PowerShell (use the installed Core Python):

```powershell
& "$env:USERPROFILE\.platformio\penv\Scripts\python.exe" tests/check_wokwi.py
powershell.exe -NoProfile -ExecutionPolicy Bypass -File tests/check_source.ps1
& "$env:USERPROFILE\.platformio\penv\Scripts\platformio.exe" run -e esp32dev
# With the official Wokwi CLI available:
wokwi-cli lint diagram.json
wokwi-cli lint wokwi/full/diagram.json
```

The net checker checks GPIO mapping, power/ground isolation, separate signals,
button contacts, allowed endpoints/components, and config/asset paths. It cannot
detect physical shorts, actual rail continuity, voltage tolerance, or bad modules.
Firmware source and relay/buzzer safety defaults are unchanged.

## Validation recorded on 2026-11-13

- Official Wokwi CLI 0.29.1 lint returned exit code 0 for both diagrams, with
  no errors or warnings. Each emitted one informational `unsupported-part`
  notice calling DevKitC V4 undocumented; this is also the board recommended
  in Wokwi's current PlatformIO documentation. The notice is retained here.
- Local net/TOML/asset checks passed for both views. The installed WebAssembly
  engine validated and instantiated both passive modules, found zero imports,
  and invoked their empty initializer; this is not Wokwi runtime validation.
- PlatformIO build SUCCESS (2.84 seconds); RAM 33,944 bytes (10.4%), flash
  608,730 bytes (46.4%). All 11 existing firmware safety checks passed.
- Simulation, visual rendering inspection, physical rail continuity, and sensor
  electrical tests were not performed. The DHT11 remains a passive placeholder.
