# Safety limits

**This prototype is not suitable for incubating eggs unattended and must not
control a heater or humidifier.**

- DHT11 accuracy and response are inadequate for reliable incubation temperature
  control. Do not use its readings to automate heating.
- The firmware has no heater or humidifier control logic. Relay requests are
  simulation only; relay GPIOs are never set as outputs or driven.
- The physical buzzer GPIO is never configured or driven until its electrical
  characteristics are verified.
- Do not connect mains voltage, heater, humidifier, motor, or any load to the relay.
- Relay coil supply must not come from the ESP32 3.3 V output. This project does not
  determine the relay board's active polarity, opto-isolation, or 3.3 V input behavior.
- Wi-Fi dashboard is read-only and has no output-control endpoints.
- Sensor failure causes a displayed/logged error and simulated alert; it does not
  cause any physical output action.

Before any future real heater system is considered, obtain and validate a calibrated
incubation-grade sensor (such as a properly calibrated SHT31/SHT35), suitable
isolated driver circuitry, independent hardware thermostat, thermal fuse,
appropriate fusing, wiring, strain relief, and a safe mains enclosure. Have mains
design and installation checked by a qualified professional. Add defined reset,
watchdog, sensor-failure, over-temperature, and independent cutoff testing.

USB-connected ESP32 and breadboard wiring are for supervised low-voltage prototype
testing only. Never leave the prototype unattended or rely on it to protect animals,
property, or people.
