#pragma once

#include <Arduino.h>

#include "settings_manager.h"

class IncubationManager {
 public:
  void begin(SettingsManager& settings);
  void update(uint64_t currentEpoch);
  int day() const;
  bool timeValid() const;
  bool turningAllowed() const;

 private:
  SettingsManager* settings_ = nullptr;
  uint64_t currentEpoch_ = 0;
};
