#include "wifi_manager.h"

#include <WiFi.h>

#include "config.h"

#if __has_include("secrets.h")
#include "secrets.h"
#endif

#ifndef WIFI_SSID
#define WIFI_SSID ""
#endif
#ifndef WIFI_PASSWORD
#define WIFI_PASSWORD ""
#endif

void WifiManager::begin() {
  if (!Config::WIFI_ENABLED || strlen(WIFI_SSID) == 0) {
    Serial.println(F("[wifi] disabled: configure include/secrets.h to enable station mode"));
    return;
  }
  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  started_ = true;
  Serial.println(F("[wifi] station connection started (non-blocking)"));
}

void WifiManager::update() {
  if (!started_) return;
  const bool nowConnected = WiFi.status() == WL_CONNECTED;
  if (nowConnected != previouslyConnected_) {
    previouslyConnected_ = nowConnected;
    if (nowConnected) {
      Serial.printf("[wifi] connected, IP=%s\n", WiFi.localIP().toString().c_str());
    } else {
      Serial.println(F("[wifi] disconnected; local firmware operation continues"));
    }
  }
}

bool WifiManager::connected() const { return started_ && WiFi.status() == WL_CONNECTED; }

String WifiManager::localIp() const {
  return connected() ? WiFi.localIP().toString() : String("offline");
}

uint32_t WifiManager::uptimeSeconds() const { return millis() / 1000UL; }
