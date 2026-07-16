#ifndef WEATHER_DATA_H
#define WEATHER_DATA_H

struct WeatherData {
  float tempCurrent;
  float tempHigh;
  float tempLow;
  float rainMm;
  int   precipPercent;
  int   uvIndex;
  int   uvMax;
  float windKph;
  char  windDir[4];
  char  iconCode[4];
  char  syncTime[6];
  float tempIndoor;
  float humidityIndoor;
};

#endif
