#include <Arduino.h>
#include <time.h>

#include "button_manager.h"
#include "buzzer_manager.h"
#include "config.h"
#include "display_manager.h"
#include "incubation_manager.h"
#include "relay_manager.h"
#include "sensor_manager.h"
#include "settings_manager.h"
#include "web_dashboard.h"
#include "wifi_manager.h"

namespace {
Dht11Sensor dhtSensor;
SensorManager sensors(dhtSensor);
DisplayManager displayManager;
ButtonManager buttons;
SettingsManager settings;
RelayManager relays;
BuzzerManager buzzer;
IncubationManager incubation;
WifiManager wifi;
WebDashboard dashboard;

uint32_t lastDisplayUpdate = 0;
bool menuOpen = false;
bool menuEditing = false;
uint8_t menuIndex = 0;
String notice;
uint32_t noticeUntil = 0;
bool clockConfigured = false;

void setNotice(const String& message) {
  notice = message;
  noticeUntil = millis() + 2500U;
}

void adjustSetting(int direction) {
  const IncubationSettings& current = settings.settings();
  bool success = false;
  switch (menuIndex) {
    case 0:
      success = settings.setTargetTemperature(current.targetTemperatureC +
                                               direction * 0.1F);
      break;
    case 1:
      success = settings.setEarlyHumidity(
          static_cast<uint8_t>(static_cast<int>(current.earlyHumidityPercent) + direction));
      break;
    case 2:
      success = settings.setLockdownHumidity(
          static_cast<uint8_t>(static_cast<int>(current.lockdownHumidityPercent) + direction));
      break;
    default:
      break;
  }
  if (!success) {
    Serial.println(F("[settings] change rejected or persistence failed"));
    setNotice("Setting not saved");
  }
}

void handleButtonEvents() {
  ButtonEvent event;
  while (buttons.nextEvent(event)) {
    if (event.type == ButtonEventType::LongPress) {
      if (event.button == ButtonId::Menu && menuOpen) {
        menuOpen = false;
        menuEditing = false;
        Serial.println(F("[ui] settings closed"));
      }
      continue;
    }

    if (event.button == ButtonId::Menu) {
      menuOpen = !menuOpen;
      menuEditing = false;
      Serial.println(menuOpen ? F("[ui] settings opened") : F("[ui] settings closed"));
    } else if (menuOpen && !menuEditing && event.button == ButtonId::Up) {
      menuIndex = static_cast<uint8_t>((menuIndex + 3U) % 4U);
    } else if (menuOpen && !menuEditing && event.button == ButtonId::Down) {
      menuIndex = static_cast<uint8_t>((menuIndex + 1U) % 4U);
    } else if (menuOpen && menuEditing && event.button == ButtonId::Up) {
      if (menuIndex == 3) settings.setTurningEnabled(true);
      else adjustSetting(1);
    } else if (menuOpen && menuEditing && event.button == ButtonId::Down) {
      if (menuIndex == 3) settings.setTurningEnabled(false);
      else adjustSetting(-1);
    } else if (menuOpen && event.button == ButtonId::Ok) {
      menuEditing = !menuEditing;
    } else if (!menuOpen && event.button == ButtonId::Turn) {
      if (incubation.turningAllowed()) {
        Serial.println(F("[turn] SIMULATED egg-turn request; no motor is installed"));
        setNotice("Turn request simulated");
      } else {
        Serial.println(F("[turn] ignored: turning disabled, day unknown, or lockdown"));
        setNotice("Turn unavailable");
      }
    }
  }
}

void updateAlerts() {
  const bool healthy = sensors.healthy();
  buzzer.update(AlertType::SensorFailure, !sensors.initialized());
  buzzer.update(AlertType::InvalidReading, sensors.initialized() && !healthy);
  const SensorReading& reading = sensors.reading();
  buzzer.update(AlertType::TemperatureWarning,
                healthy && (reading.temperatureC < Config::WARNING_TEMPERATURE_LOW_C ||
                            reading.temperatureC > Config::WARNING_TEMPERATURE_HIGH_C));
  buzzer.update(AlertType::HumidityWarning,
                healthy && (reading.humidityPercent < Config::WARNING_HUMIDITY_LOW_PERCENT ||
                            reading.humidityPercent > Config::WARNING_HUMIDITY_HIGH_PERCENT));
  buzzer.update(AlertType::SystemInitializationError,
                !displayManager.available() || !settings.storageReady());
}
}  // namespace

void setup() {
  Serial.begin(Config::SERIAL_BAUD);
  Serial.println();
  Serial.println(F("Smart Incubator v1.0 - monitoring prototype"));
  Serial.println(F("Safety: no heater, humidifier, or physical buzzer outputs are enabled."));

  settings.begin();
  sensors.begin();
  buttons.begin();
  relays.begin();
  buzzer.begin();
  displayManager.begin();
  incubation.begin(settings);
  wifi.begin();
  dashboard.begin(sensors, settings, relays, incubation, wifi);
}

void loop() {
  const uint32_t now = millis();
  sensors.update(now);
  buttons.update(now);
  handleButtonEvents();
  wifi.update();

  if (wifi.connected() && !clockConfigured) {
    configTime(0, 0, "pool.ntp.org");
    clockConfigured = true;
    Serial.println(F("[time] SNTP sync requested; incubation day remains unknown until valid"));
  }
  const time_t epoch = time(nullptr);
  incubation.update(epoch > 0 ? static_cast<uint64_t>(epoch) : 0);
  dashboard.update();
  updateAlerts();

  if (notice.length() > 0 && static_cast<int32_t>(now - noticeUntil) >= 0) {
    notice = "";
  }
  if (static_cast<uint32_t>(now - lastDisplayUpdate) >= Config::DISPLAY_INTERVAL_MS) {
    lastDisplayUpdate = now;
    displayManager.render(sensors.reading(), sensors.healthy(), settings.settings(),
                          incubation.day(), wifi.localIp(), menuOpen, menuIndex,
                          menuEditing, notice);
  }
}
