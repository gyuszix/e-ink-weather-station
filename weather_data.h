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
  float windkt;
  char  windDir[4];
  char  iconCode[4];
  char  syncTime[6];
  float tempIndoor;
  float humidityIndoor;
  float batteryVoltage;
  int   batteryPercent;

  // --- freshness / day tracking ---
  char  currentDate[11]; // "YYYY-MM-DD" local date of the current reading
  char  localDate[11];   // "YYYY-MM-DD" local date the day figures describe
  bool  currentValid;    // current conditions fetched and parsed OK
  bool  dayValid;        // day figures present AND confirmed to be today's
  bool  dayChecked;      // the date guard actually ran (needs a valid current block)
};

#endif
