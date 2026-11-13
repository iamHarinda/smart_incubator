#pragma once

#include <Arduino.h>

namespace Config {
constexpr uint32_t SERIAL_BAUD = 115200;
constexpr uint32_t SENSOR_INTERVAL_MS = 2000;
constexpr uint32_t DISPLAY_INTERVAL_MS = 500;
constexpr uint32_t BUTTON_DEBOUNCE_MS = 35;
constexpr uint32_t BUTTON_LONG_PRESS_MS = 1000;
constexpr uint8_t OLED_ADDRESS = 0x3C;
constexpr int OLED_WIDTH = 128;
constexpr int OLED_HEIGHT = 64;

constexpr uint8_t OLED_SDA_PIN = 21;
constexpr uint8_t OLED_SCL_PIN = 22;
constexpr uint8_t DHT_DATA_PIN = 4;
constexpr uint8_t MENU_BUTTON_PIN = 18;
constexpr uint8_t UP_BUTTON_PIN = 19;
constexpr uint8_t DOWN_BUTTON_PIN = 23;
constexpr uint8_t OK_BUTTON_PIN = 16;
constexpr uint8_t TURN_BUTTON_PIN = 17;
constexpr uint8_t POTENTIOMETER_PIN = 34;
constexpr uint8_t HEATER_RELAY_PIN = 26;
constexpr uint8_t HUMIDIFIER_RELAY_PIN = 27;
constexpr uint8_t BUZZER_PIN = 25;

constexpr float DEFAULT_TARGET_TEMPERATURE_C = 37.5F;
constexpr uint8_t DEFAULT_EARLY_HUMIDITY_PERCENT = 50;
constexpr uint8_t DEFAULT_LOCKDOWN_HUMIDITY_PERCENT = 65;
constexpr uint8_t INCUBATION_DAYS = 21;
constexpr uint8_t TURNING_LAST_DAY = 18;

constexpr float WARNING_TEMPERATURE_LOW_C = 36.0F;
constexpr float WARNING_TEMPERATURE_HIGH_C = 39.0F;
constexpr uint8_t WARNING_HUMIDITY_LOW_PERCENT = 35;
constexpr uint8_t WARNING_HUMIDITY_HIGH_PERCENT = 75;

constexpr bool RELAY_SIMULATION_MODE = true;
constexpr bool BUZZER_SIMULATION_MODE = true;
constexpr bool WIFI_ENABLED = true;
}  // namespace Config
