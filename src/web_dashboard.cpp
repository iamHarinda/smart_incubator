#include "web_dashboard.h"

#include "config.h"

namespace {
const char DASHBOARD_HTML[] PROGMEM = R"HTML(
<!doctype html><html lang="en"><head><meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>Smart Incubator Monitor</title>
<style>
body{font:16px system-ui,sans-serif;background:#f3f6f8;color:#18212b;margin:0;padding:1rem}
main{max-width:680px;margin:auto}.card{background:white;border-radius:12px;padding:1rem 1.2rem;margin:1rem 0;box-shadow:0 2px 12px #16324a12}
h1{font-size:1.35rem}.reading{font-size:1.5rem;font-weight:650}.muted{color:#5d6b78}
.grid{display:grid;grid-template-columns:repeat(auto-fit,minmax(145px,1fr));gap:.8rem}
</style></head><body><main><h1>Smart Incubator <span class="muted">monitor</span></h1>
<div id="status" class="card">Connecting…</div><div class="grid">
<section class="card"><div class="muted">Temperature</div><div id="temp" class="reading">—</div></section>
<section class="card"><div class="muted">Humidity</div><div id="humidity" class="reading">—</div></section>
<section class="card"><div class="muted">Incubation day</div><div id="day" class="reading">—</div></section>
<section class="card"><div class="muted">Uptime</div><div id="uptime" class="reading">—</div></section>
</div><section class="card"><strong>Configuration</strong><p id="settings">—</p>
<strong>Relay simulation</strong><p id="relays">—</p>
<p class="muted">Read-only monitor. No heater or humidifier controls are available.</p></section>
</main><script>
async function refresh(){try{const r=await fetch('/api/status',{cache:'no-store'});if(!r.ok)throw Error();
const d=await r.json();document.getElementById('status').textContent='Sensor: '+(d.sensorHealthy?'OK':'ERROR')+' · Wi-Fi: connected';
document.getElementById('temp').textContent=d.sensorHealthy?d.temperatureC.toFixed(1)+' °C':'Sensor error';
document.getElementById('humidity').textContent=d.sensorHealthy?d.humidityPercent.toFixed(0)+' %':'Sensor error';
document.getElementById('day').textContent=d.day?d.day+' / 21':'-- / 21 (time not synchronized)';
document.getElementById('uptime').textContent=d.uptimeSeconds+' s';
document.getElementById('settings').textContent=d.targetTemperatureC.toFixed(1)+' °C target · early humidity '+d.earlyHumidityPercent+'% · lockdown '+d.lockdownHumidityPercent+'% · turning '+(d.turningEnabled?'enabled':'disabled');
document.getElementById('relays').textContent='Heater '+(d.heaterSimulated?'ON':'OFF')+' · humidifier '+(d.humidifierSimulated?'ON':'OFF')+' (simulation only)';
}catch(e){document.getElementById('status').textContent='Dashboard disconnected; device continues local operation.'}}
refresh();setInterval(refresh,2000);
</script></body></html>)HTML";
}

void WebDashboard::begin(SensorManager& sensors, SettingsManager& settings,
                         RelayManager& relays, IncubationManager& incubation,
                         WifiManager& wifi) {
  sensors_ = &sensors;
  settings_ = &settings;
  relays_ = &relays;
  incubation_ = &incubation;
  wifi_ = &wifi;
  server_.on("/", HTTP_GET, [this]() { handleRoot(); });
  server_.on("/api/status", HTTP_GET, [this]() { handleApi(); });
  server_.onNotFound([this]() { server_.send(404, "text/plain", "Not found"); });
}

void WebDashboard::update() {
  if (!wifi_->connected()) {
    started_ = false;
    return;
  }
  if (!started_) {
    server_.begin();
    started_ = true;
    Serial.println(F("[web] read-only dashboard started on port 80"));
  }
  server_.handleClient();
}

void WebDashboard::handleRoot() {
  server_.send_P(200, "text/html; charset=utf-8", DASHBOARD_HTML);
}

void WebDashboard::handleApi() {
  const SensorReading& reading = sensors_->reading();
  const IncubationSettings& settings = settings_->settings();
  String json;
  json.reserve(320);
  json = "{\"sensorHealthy\":";
  json += sensors_->healthy() ? "true" : "false";
  json += ",\"temperatureC\":";
  json += sensors_->healthy() ? String(reading.temperatureC, 1) : "null";
  json += ",\"humidityPercent\":";
  json += sensors_->healthy() ? String(reading.humidityPercent, 1) : "null";
  json += ",\"day\":" + String(incubation_->day());
  json += ",\"uptimeSeconds\":" + String(wifi_->uptimeSeconds());
  json += ",\"targetTemperatureC\":" + String(settings.targetTemperatureC, 1);
  json += ",\"earlyHumidityPercent\":" + String(settings.earlyHumidityPercent);
  json += ",\"lockdownHumidityPercent\":" + String(settings.lockdownHumidityPercent);
  json += ",\"turningEnabled\":";
  json += settings.turningEnabled ? "true" : "false";
  json += ",\"heaterSimulated\":";
  json += relays_->heaterRequested() ? "true" : "false";
  json += ",\"humidifierSimulated\":";
  json += relays_->humidifierRequested() ? "true" : "false";
  json += "}";
  server_.send(200, "application/json", json);
}
