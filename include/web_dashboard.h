#pragma once

#include <WebServer.h>

#include "incubation_manager.h"
#include "relay_manager.h"
#include "sensor_manager.h"
#include "settings_manager.h"
#include "wifi_manager.h"

class WebDashboard {
 public:
  void begin(SensorManager& sensors, SettingsManager& settings,
             RelayManager& relays, IncubationManager& incubation,
             WifiManager& wifi);
  void update();

 private:
  WebServer server_{80};
  bool started_ = false;
  SensorManager* sensors_ = nullptr;
  SettingsManager* settings_ = nullptr;
  RelayManager* relays_ = nullptr;
  IncubationManager* incubation_ = nullptr;
  WifiManager* wifi_ = nullptr;
  void handleRoot();
  void handleApi();
};
