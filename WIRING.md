# Prototype wiring

## Next bench test: OLED and DHT11 only

Both devices are currently disconnected; neither hardware test has passed.
The first phase is OLED alone, then add DHT11 after removing USB power.
Verify module labels and 3.3 V compatibility; the diagram shows signal names,
not physical header order. Keep all relay/buzzer/motor/load connections absent.

```text
ESP32-WROOM-32                OLED (SSD1306 assumed)
3V3 ------------------------ VCC
GND ------------------------ GND
GPIO21 --------------------- SDA
GPIO22 --------------------- SCL

ESP32-WROOM-32                DHT11 MODULE (verify pin labels)
3V3 ------------------------ VCC
GND ------------------------ GND
GPIO4 ---------------------- DATA
                              |
                   module DATA pull-up to 3V3
                   (verify present; never pull up to 5 V)
```

The two modules share the ESP32's 3V3 and GND rails. Check breadboard rail breaks
and polarity with USB disconnected. Never infer a module's VCC/DATA/GND order
from this drawing. If a module's voltage compatibility or pull-up is uncertain,
stop before powering it. GPIO numbers are not development-board header numbers.
See [TESTING.md](TESTING.md) for the observation and failure/recovery procedure.

Disconnect USB power while changing breadboard wiring. Verify the ESP32 board's
pin labels; board header positions vary. Connect all low-voltage prototype grounds
as described below. Do not connect any mains equipment.

| Component | ESP32 connection | Notes |
|---|---:|---|
| OLED SDA | GPIO21 | I2C data |
| OLED SCL | GPIO22 | I2C clock |
| OLED VCC | 3V3 | Only after confirming the OLED accepts 3.3 V |
| OLED GND | GND | |
| DHT11 DATA | GPIO4 | Verify module pinout and pull-up |
| DHT11 VCC | 3V3 | As specified for the module |
| DHT11 GND | GND | |
| MENU button | GPIO18 to GND | `INPUT_PULLUP`; released = HIGH |
| UP button | GPIO19 to GND | `INPUT_PULLUP` |
| DOWN button | GPIO23 to GND | `INPUT_PULLUP` |
| OK button | GPIO16 to GND | `INPUT_PULLUP` |
| TURN button | GPIO17 to GND | Simulated request only |

Each button is a normally-open switch between its assigned GPIO and GND. Do not
wire a button to 3.3 V when internal pull-ups are enabled. GPIO34 is reserved as
an input-only potentiometer assignment but is unused; leave it disconnected.

## Leave disconnected

- Relay module inputs and relay contacts (GPIO26 heater reservation, GPIO27
  humidifier reservation): no firmware output is enabled and no electrical
  compatibility/isolation has been verified.
- Buzzer (GPIO25 reservation): voltage, current, and type are unknown.
- Mains, heater, humidifier, motors, and all relay contact terminals.
- IR, LDR, PIR, LEDs, and potentiometer are currently unused.

The relay module's JD-VCC jumper does not establish that its logic input is
ESP32-compatible or that isolation is present. Do not remove the jumper or apply
power based on this guide. See [SAFETY.md](SAFETY.md).
