#include "display_manager.h"

#include <Wire.h>

#include "config.h"

namespace {
const char* menuLabel(uint8_t index) {
  switch (index) {
    case 0: return "Target temperature";
    case 1: return "Early humidity";
    case 2: return "Lockdown humidity";
    case 3: return "Egg turning";
    default: return "Unknown";
  }
}
}

bool DisplayManager::begin() {
  Wire.begin(Config::OLED_SDA_PIN, Config::OLED_SCL_PIN);
  available_ = display_.begin(SSD1306_SWITCHCAPVCC, Config::OLED_ADDRESS);
  if (!available_) {
    Serial.println(F("[display] ERROR: SSD1306 not detected at configured I2C address"));
    return false;
  }
  display_.clearDisplay();
  display_.setTextColor(SSD1306_WHITE);
  showStartup();
  return true;
}

void DisplayManager::showStartup() {
  if (!available_) return;
  display_.clearDisplay();
  display_.setTextSize(1);
  display_.setCursor(0, 8);
  display_.println(F("SMART INCUBATOR"));
  display_.println();
  display_.println(F("Monitor prototype"));
  display_.println(F("Outputs disabled"));
  display_.display();
  startupAtMs_ = millis();
  startupShown_ = true;
}

void DisplayManager::render(const SensorReading& reading, bool sensorHealthy,
                            const IncubationSettings& settings, int incubationDay,
                            const String& ipAddress, bool menuOpen, uint8_t menuIndex,
                            bool menuEditing, const String& notice) {
  if (!available_) return;
  if (startupShown_ && static_cast<uint32_t>(millis() - startupAtMs_) < 1400U) return;
  startupShown_ = false;

  display_.clearDisplay();
  display_.setTextSize(1);
  display_.setCursor(0, 0);
  if (menuOpen) {
    display_.printf("SETTINGS %u/4 %s\n", static_cast<unsigned>(menuIndex + 1U),
                    menuEditing ? "EDIT" : "SELECT");
    display_.drawFastHLine(0, 10, Config::OLED_WIDTH, SSD1306_WHITE);
    display_.setCursor(0, 15);
    display_.print(F("> "));
    display_.println(menuLabel(menuIndex));
    display_.setCursor(0, 29);
    switch (menuIndex) {
      case 0:
        display_.printf("%.1f C", settings.targetTemperatureC);
        break;
      case 1:
        display_.printf("%u %%", static_cast<unsigned>(settings.earlyHumidityPercent));
        break;
      case 2:
        display_.printf("%u %%", static_cast<unsigned>(settings.lockdownHumidityPercent));
        break;
      case 3:
        display_.println(settings.turningEnabled ? F("Enabled") : F("Disabled"));
        break;
    }
    display_.setCursor(0, 48);
    display_.println(menuEditing ? F("UP/DOWN change OK done") :
                                   F("UP/DOWN select OK edit"));
    display_.println(F("MENU exit"));
  } else {
    display_.println(F("SMART INCUBATOR"));
    display_.drawFastHLine(0, 10, Config::OLED_WIDTH, SSD1306_WHITE);
    display_.setCursor(0, 14);
    if (sensorHealthy) {
      display_.printf("Temperature: %.1f C\n", reading.temperatureC);
      display_.printf("Humidity:    %.0f %%\n", reading.humidityPercent);
    } else {
      display_.println(F("Temperature: --.- C"));
      display_.println(F("Humidity:    -- %"));
    }
    display_.printf("Day: %02d / 21\n", incubationDay);
    display_.print(F("System: "));
    display_.println(sensorHealthy ? F("MONITORING") : F("SENSOR ERROR"));
    if (notice.length() > 0) {
      display_.setCursor(0, 55);
      display_.print(notice.substring(0, 21));
    } else {
      display_.setCursor(0, 55);
      display_.print(F("IP: "));
      display_.print(ipAddress.substring(0, 17));
    }
  }
  display_.display();
}

bool DisplayManager::available() const { return available_; }
