#pragma once

#include <Arduino.h>

class WifiManager {
 public:
  void begin();
  void update();
  bool connected() const;
  String localIp() const;
  uint32_t uptimeSeconds() const;

 private:
  bool started_ = false;
  bool previouslyConnected_ = false;
};
