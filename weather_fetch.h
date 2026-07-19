#ifndef WEATHER_FETCH_H
#define WEATHER_FETCH_H

#include <Wire.h>
#include <Adafruit_BME280.h>
#include "weather_data.h"

Adafruit_BME280 bme;
bool bmeReady = false;

void fetchWeatherData(WeatherData &data) {

  if (!bmeReady) {
    bmeReady = bme.begin(0x76);
    if (!bmeReady) {
      Serial.println("BME280 not found");
    }
  }
  // --- stub: hardcoded test values ---
  // replace this entire function body with WiFi + OWM API call later
  data.tempCurrent   = 42;
  data.tempHigh      = 29;
  data.tempLow       = 17;
  data.rainMm        = 49.4;
  data.precipPercent = 28;
  data.uvIndex       = 6;
  data.uvMax         = 11;
  data.windKph       = 19;

  // data.tempIndoor    = 21;
  // data.humidityIndoor= 58;
  if (bmeReady) {
    data.tempIndoor       = bme.readTemperature();
    data.humidityIndoor   = bme.readHumidity();
  }

  strncpy(data.windDir,  "SW",    sizeof(data.windDir));
  strncpy(data.iconCode, "09n",   sizeof(data.iconCode));
  strncpy(data.syncTime, "14:30", sizeof(data.syncTime));
}

#endif
