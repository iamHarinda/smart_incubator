#include "relay_manager.h"

#include "config.h"

void RelayManager::begin() {
  heaterRequested_ = false;
  humidifierRequested_ = false;
  Serial.println(F("[relay] simulation enabled; relay pins are not configured as outputs"));
}

void RelayManager::requestHeater(bool on) {
  heaterRequested_ = on;
  Serial.printf("[relay] SIMULATION heater=%s (no GPIO drive)\n", on ? "ON" : "OFF");
}

void RelayManager::requestHumidifier(bool on) {
  humidifierRequested_ = on;
  Serial.printf("[relay] SIMULATION humidifier=%s (no GPIO drive)\n", on ? "ON" : "OFF");
}

bool RelayManager::heaterRequested() const { return heaterRequested_; }
bool RelayManager::humidifierRequested() const { return humidifierRequested_; }
