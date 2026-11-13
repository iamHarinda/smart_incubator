# Roadmap

## Implemented software

- Phase 1: PlatformIO ESP32 Arduino project, DHT11 abstraction and polling,
  SSD1306 display and sensor error status.
- Phase 2: Five pull-up buttons with debounce, settings screen and Preferences storage.
- Phase 3: Software sensor/range alert events with buzzer simulation.
- Phase 4: Relay simulation with no physical relay GPIO drive.
- Phase 5: Optional Wi-Fi station client and read-only local dashboard.
- Phase 6: Wiring, safety, testing, and hardware validation documentation.

## Not implemented / future work

- Calibrated SHT31/SHT35 sensor support and calibration procedure.
- Explicit incubation start/reset workflow and validated timekeeping behavior.
- Egg-turning motor/driver, limit switches, jam detection, and safety interlocks.
- Any real heater/humidifier control, hardware safety chain, and relay validation.
- Persistent data logging, alarms, dashboard authentication, and OTA updates.
- Additional IR/LDR/PIR/potentiometer functions.

No future electrical output feature should be enabled before hardware, isolation,
logic levels, reset behavior, and independent safety protections are reviewed.
