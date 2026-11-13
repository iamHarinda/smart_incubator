#include "sensor_manager.h"

#include "config.h"

Dht11Sensor::Dht11Sensor() : dht_(Config::DHT_DATA_PIN, DHT11) {}

void Dht11Sensor::begin() {
  dht_.begin();
  initialized_ = true;
  Serial.println(F("[sensor] DHT11 driver initialized"));
}

bool Dht11Sensor::initialized() const { return initialized_; }

SensorReading Dht11Sensor::read() {
  SensorReading result;
  result.temperatureC = dht_.readTemperature();
  result.humidityPercent = dht_.readHumidity();
  result.sampledAtMs = millis();
  result.valid = isfinite(result.temperatureC) && isfinite(result.humidityPercent) &&
                 result.temperatureC >= -40.0F && result.temperatureC <= 80.0F &&
                 result.humidityPercent >= 0.0F && result.humidityPercent <= 100.0F;
  return result;
}

SensorManager::SensorManager(SensorInterface& sensor) : sensor_(sensor) {}

void SensorManager::begin() { sensor_.begin(); }

void SensorManager::update(uint32_t now) {
  if (hasRead_ && static_cast<uint32_t>(now - lastReadMs_) < Config::SENSOR_INTERVAL_MS) {
    return;
  }
  lastReadMs_ = now;
  hasRead_ = true;
  reading_ = sensor_.read();
  if (reading_.valid) {
    Serial.printf("[sensor] temperature=%.1f C humidity=%.0f %%\n",
                  reading_.temperatureC, reading_.humidityPercent);
  } else {
    Serial.println(F("[sensor] ERROR: disconnected or invalid DHT11 reading"));
  }
}

const SensorReading& SensorManager::reading() const { return reading_; }
bool SensorManager::healthy() const { return hasRead_ && reading_.valid; }
bool SensorManager::initialized() const { return sensor_.initialized(); }
