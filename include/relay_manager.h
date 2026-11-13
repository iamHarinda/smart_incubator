#pragma once

#include <Arduino.h>

class RelayManager {
 public:
  void begin();
  void requestHeater(bool on);
  void requestHumidifier(bool on);
  bool heaterRequested() const;
  bool humidifierRequested() const;

 private:
  bool heaterRequested_ = false;
  bool humidifierRequested_ = false;
};
