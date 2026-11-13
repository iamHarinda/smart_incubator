# Hardware inventory and compatibility

## Available hardware

- ESP32-WROOM-32 development board, 38-pin, USB-C
- 0.96-inch I2C OLED (controller and I2C address to be verified)
- DHT11 temperature/humidity module
- 2-channel 5 V Songle relay module with JD-VCC jumper
- Buzzer (voltage and active/passive type unverified)
- B10K potentiometer, five tactile buttons, LEDs/resistors, MB-102 breadboard,
  jumper wires, USB-C cable
- IR obstacle sensor, LDR sensor, PIR motion sensor

Only the ESP32, DHT11, OLED, and buttons are part of this monitoring prototype's
active wiring. Other listed sensors and potentiometer have no implemented function.
The buzzer and relay controls are not physically driven.

## Compatibility checks before wiring

- Verify the board is an ESP32-WROOM-32 variant and identify its 3.3 V and GND pins.
- Confirm OLED controller, I2C address (usually 0x3C, but not assumed), and supply
  requirements. The firmware initially uses Adafruit SSD1306 at address 0x3C.
- Check the DHT11 module pin labels and whether it has an onboard data pull-up.
  The DHT library's `begin()` call does not prove a sensor is physically present;
  the first valid measurement is the operational health check.
- Confirm the button wiring and do not connect any button to a supply rail.
- Do not infer the relay module's input polarity, 3.3 V logic compatibility, JD-VCC
  isolation, or safe coil supply from its appearance.
- The ESP32 GPIOs are 3.3 V only. Never apply 5 V to an ESP32 GPIO.

### SH1106 OLED variant

If inspection or an I2C scan confirms an SH1106 rather than SSD1306, change the
display driver/library and its initialization in `src\display_manager.cpp` (for
example, use a maintained U8g2 SH1106 128×64 I2C driver) and update
`platformio.ini` dependencies. Preserve the existing screen layout and I2C pin
configuration. Do not assume SSD1306 initialization is compatible with SH1106.
