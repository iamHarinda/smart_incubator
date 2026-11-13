#include "incubation_manager.h"

#include "config.h"

void IncubationManager::begin(SettingsManager& settings) { settings_ = &settings; }

void IncubationManager::update(uint64_t currentEpoch) {
  currentEpoch_ = currentEpoch >= 1700000000ULL ? currentEpoch : 0;
  if (currentEpoch_ != 0 && settings_ != nullptr && settings_->settings().startEpoch == 0) {
    if (settings_->setStartEpoch(currentEpoch_)) {
      Serial.println(F("[incubation] day tracking anchored to synchronized system time"));
    }
  }
}

int IncubationManager::day() const {
  if (settings_ == nullptr || currentEpoch_ == 0 || settings_->settings().startEpoch == 0 ||
      currentEpoch_ < settings_->settings().startEpoch) {
    return 0;
  }
  const uint64_t elapsedDays =
      (currentEpoch_ - settings_->settings().startEpoch) / 86400ULL;
  if (elapsedDays >= Config::INCUBATION_DAYS) return Config::INCUBATION_DAYS;
  return static_cast<int>(elapsedDays) + 1;
}

bool IncubationManager::timeValid() const { return day() > 0; }

bool IncubationManager::turningAllowed() const {
  return settings_ != nullptr && settings_->settings().turningEnabled && day() > 0 &&
         day() <= Config::TURNING_LAST_DAY;
}
