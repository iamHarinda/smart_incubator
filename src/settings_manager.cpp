#include "settings_manager.h"

#include "config.h"

IncubationSettings SettingsManager::defaults() {
  return {Config::DEFAULT_TARGET_TEMPERATURE_C,
          Config::DEFAULT_EARLY_HUMIDITY_PERCENT,
          Config::DEFAULT_LOCKDOWN_HUMIDITY_PERCENT, true, 0};
}

bool SettingsManager::validate(const IncubationSettings& settings) {
  return isfinite(settings.targetTemperatureC) &&
         settings.targetTemperatureC >= 35.0F && settings.targetTemperatureC <= 40.0F &&
         settings.earlyHumidityPercent >= 20 && settings.earlyHumidityPercent <= 80 &&
         settings.lockdownHumidityPercent >= 20 && settings.lockdownHumidityPercent <= 90;
}

void SettingsManager::begin() {
  if (!preferences_.begin("incubator", false)) {
    Serial.println(F("[settings] ERROR: Preferences unavailable; using defaults"));
    settings_ = defaults();
    return;
  }
  storageReady_ = true;

  IncubationSettings stored = defaults();
  stored.targetTemperatureC = preferences_.getFloat("targetC", stored.targetTemperatureC);
  stored.earlyHumidityPercent =
      preferences_.getUChar("earlyHum", stored.earlyHumidityPercent);
  stored.lockdownHumidityPercent =
      preferences_.getUChar("lockHum", stored.lockdownHumidityPercent);
  stored.turningEnabled = preferences_.getBool("turning", stored.turningEnabled);
  stored.startEpoch = preferences_.getULong64("startEpoch", 0);

  if (validate(stored)) {
    settings_ = stored;
    Serial.println(F("[settings] Preferences loaded"));
  } else {
    settings_ = defaults();
    Serial.println(F("[settings] WARNING: invalid stored settings; using defaults"));
  }
}

const IncubationSettings& SettingsManager::settings() const { return settings_; }
bool SettingsManager::storageReady() const { return storageReady_; }

bool SettingsManager::save() {
  if (!storageReady_) {
    Serial.println(F("[settings] ERROR: Preferences storage is unavailable"));
    return false;
  }
  if (!validate(settings_)) {
    Serial.println(F("[settings] ERROR: rejected invalid settings"));
    return false;
  }
  if (!preferences_.putFloat("targetC", settings_.targetTemperatureC) ||
      !preferences_.putUChar("earlyHum", settings_.earlyHumidityPercent) ||
      !preferences_.putUChar("lockHum", settings_.lockdownHumidityPercent) ||
      !preferences_.putBool("turning", settings_.turningEnabled) ||
      !preferences_.putULong64("startEpoch", settings_.startEpoch)) {
    Serial.println(F("[settings] ERROR: failed to persist settings"));
    return false;
  }
  return true;
}

bool SettingsManager::setTargetTemperature(float value) {
  const float previous = settings_.targetTemperatureC;
  settings_.targetTemperatureC = value;
  if (!validate(settings_) || !save()) {
    settings_.targetTemperatureC = previous;
    return false;
  }
  return true;
}

bool SettingsManager::setEarlyHumidity(uint8_t value) {
  const uint8_t previous = settings_.earlyHumidityPercent;
  settings_.earlyHumidityPercent = value;
  if (!validate(settings_) || !save()) {
    settings_.earlyHumidityPercent = previous;
    return false;
  }
  return true;
}

bool SettingsManager::setLockdownHumidity(uint8_t value) {
  const uint8_t previous = settings_.lockdownHumidityPercent;
  settings_.lockdownHumidityPercent = value;
  if (!validate(settings_) || !save()) {
    settings_.lockdownHumidityPercent = previous;
    return false;
  }
  return true;
}

void SettingsManager::setTurningEnabled(bool enabled) {
  const bool previous = settings_.turningEnabled;
  settings_.turningEnabled = enabled;
  if (!save()) {
    settings_.turningEnabled = previous;
  }
}

bool SettingsManager::setStartEpoch(uint64_t epoch) {
  if (epoch == 0) return false;
  const uint64_t previous = settings_.startEpoch;
  settings_.startEpoch = epoch;
  if (!save()) {
    settings_.startEpoch = previous;
    return false;
  }
  return true;
}
