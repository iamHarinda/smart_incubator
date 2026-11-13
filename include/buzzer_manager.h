#pragma once

#include <Arduino.h>

enum class AlertType : uint8_t {
  SensorFailure,
  InvalidReading,
  TemperatureWarning,
  HumidityWarning,
  SystemInitializationError
};

class BuzzerManager {
 public:
  void begin();
  void update(AlertType alert, bool active);
};
