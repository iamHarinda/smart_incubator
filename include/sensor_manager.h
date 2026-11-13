#pragma once

#include <Arduino.h>
#include <DHT.h>

struct SensorReading {
  float temperatureC = NAN;
  float humidityPercent = NAN;
  bool valid = false;
  uint32_t sampledAtMs = 0;
};

class SensorInterface {
 public:
  virtual ~SensorInterface() = default;
  virtual void begin() = 0;
  virtual bool initialized() const = 0;
  virtual SensorReading read() = 0;
};

class Dht11Sensor final : public SensorInterface {
 public:
  Dht11Sensor();
  void begin() override;
  bool initialized() const override;
  SensorReading read() override;

 private:
  DHT dht_;
  bool initialized_ = false;
};

class SensorManager {
 public:
  explicit SensorManager(SensorInterface& sensor);
  void begin();
  void update(uint32_t now);
  const SensorReading& reading() const;
  bool healthy() const;
  bool initialized() const;

 private:
  SensorInterface& sensor_;
  SensorReading reading_;
  uint32_t lastReadMs_ = 0;
  bool hasRead_ = false;
};
