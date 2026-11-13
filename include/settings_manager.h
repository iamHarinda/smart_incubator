#pragma once

#include <Arduino.h>
#include <Preferences.h>

struct IncubationSettings {
  float targetTemperatureC;
  uint8_t earlyHumidityPercent;
  uint8_t lockdownHumidityPercent;
  bool turningEnabled;
  uint64_t startEpoch;
};

class SettingsManager {
 public:
  void begin();
  const IncubationSettings& settings() const;
  bool storageReady() const;
  bool setTargetTemperature(float value);
  bool setEarlyHumidity(uint8_t value);
  bool setLockdownHumidity(uint8_t value);
  void setTurningEnabled(bool enabled);
  bool setStartEpoch(uint64_t epoch);
  static IncubationSettings defaults();
  static bool validate(const IncubationSettings& settings);

 private:
  bool save();
  Preferences preferences_;
  IncubationSettings settings_ = defaults();
  bool storageReady_ = false;
};
