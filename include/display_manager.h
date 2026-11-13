#pragma once

#include <Adafruit_SSD1306.h>

#include "sensor_manager.h"
#include "settings_manager.h"

class DisplayManager {
 public:
  bool begin();
  void showStartup();
  void render(const SensorReading& reading, bool sensorHealthy,
              const IncubationSettings& settings, int incubationDay,
              const String& ipAddress, bool menuOpen, uint8_t menuIndex,
              bool menuEditing, const String& notice);
  bool available() const;

 private:
  Adafruit_SSD1306 display_{128, 64, &Wire, -1};
  bool available_ = false;
  uint32_t startupAtMs_ = 0;
  bool startupShown_ = false;
};
