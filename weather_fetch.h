#ifndef WEATHER_FETCH_H
#define WEATHER_FETCH_H

#include "weather_data.h"

void fetchWeatherData(WeatherData &data) {
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
  strncpy(data.windDir,  "SW",    sizeof(data.windDir));
  strncpy(data.iconCode, "09n",   sizeof(data.iconCode));
  strncpy(data.syncTime, "14:30", sizeof(data.syncTime));
}

#endif
