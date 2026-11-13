#include "buzzer_manager.h"

#include "config.h"

void BuzzerManager::begin() {
  Serial.println(F("[buzzer] simulation enabled; physical buzzer pin is not driven"));
}

void BuzzerManager::update(AlertType alert, bool active) {
  static bool states[5] = {false, false, false, false, false};
  const uint8_t index = static_cast<uint8_t>(alert);
  if (index >= 5 || states[index] == active) return;
  states[index] = active;
  Serial.printf("[alert] %s: %s (buzzer simulated)\n",
                alert == AlertType::SensorFailure ? "sensor failure" :
                alert == AlertType::InvalidReading ? "invalid reading" :
                alert == AlertType::TemperatureWarning ? "temperature warning" :
                alert == AlertType::HumidityWarning ? "humidity warning" :
                                                       "system initialization error",
                active ? "ACTIVE" : "CLEARED");
}
