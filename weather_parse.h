#ifndef WEATHER_PARSE_H
#define WEATHER_PARSE_H

#include <ArduinoJson.h>
#include "weather_data.h"

const char* degreesToCardinal(int deg) {
  if (deg < 23)  return "N";
  if (deg < 68)  return "NE";
  if (deg < 113) return "E";
  if (deg < 158) return "SE";
  if (deg < 203) return "S";
  if (deg < 248) return "SW";
  if (deg < 293) return "W";
  if (deg < 338) return "NW";
  return "N";
}

// floor division, so the epoch day is still right for negative timestamps
long floorDiv(long a, long b) {
  long q = a / b;
  if ((a % b != 0) && ((a < 0) != (b < 0))) q--;
  return q;
}

// Howard Hinnant's civil_from_days: days since 1970-01-01 -> calendar date
void civilFromDays(long z, int &y, int &m, int &d) {
  z += 719468;
  long era      = floorDiv(z, 146097L);
  unsigned doe  = (unsigned)(z - era * 146097L);
  unsigned yoe  = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
  long yr       = (long)yoe + era * 400;
  unsigned doy  = doe - (365 * yoe + yoe / 4 - yoe / 100);
  unsigned mp   = (5 * doy + 2) / 153;
  d = (int)(doy - (153 * mp + 2) / 5 + 1);
  m = (int)(mp < 10 ? mp + 3 : mp - 9);
  y = (int)(yr + (m <= 2));
}

// local calendar date implied by the OWM current reading, as "YYYY-MM-DD"
void localDateFromCurrent(WeatherData &data, char *out, size_t len) {
  long local = data.currentDt + data.timezoneOffset;
  int y, m, d;
  civilFromDays(floorDiv(local, 86400L), y, m, d);
  snprintf(out, len, "%04d-%02d-%02d", y, m, d);
}

void parseCurrent(String &json, WeatherData &data) {
  Serial.print("Current response length: ");
  Serial.println(json.length());

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, json);
  if (error) {
    Serial.print("Current parse error: ");
    Serial.println(error.c_str());
    return;
  }
  Serial.println("Current parse OK");

  data.tempCurrent    = doc["main"]["temp"];
  data.timezoneOffset = doc["timezone"].as<long>();
  data.currentDt      = doc["dt"].as<long>();
  strncpy(data.iconCode, doc["weather"][0]["icon"] | "01d", sizeof(data.iconCode));
  data.windkt         = doc["wind"]["speed"].as<float>() * 1.944;
  int windDeg         = doc["wind"]["deg"].as<int>();
  strncpy(data.windDir, degreesToCardinal(windDeg), sizeof(data.windDir));

  long dt     = data.currentDt + data.timezoneOffset;
  int hours   = (int)(((dt % 86400L) + 86400L) % 86400L) / 3600;
  int minutes = (int)(((dt % 3600L)  + 3600L)  % 3600L)  / 60;
  snprintf(data.syncTime, sizeof(data.syncTime), "%02d:%02d", hours, minutes);

  data.currentValid = true;

  Serial.println("--- Current parsed values ---");
  Serial.print("tempCurrent: ");    Serial.println(data.tempCurrent);
  Serial.print("iconCode: ");       Serial.println(data.iconCode);
  Serial.print("windkt: ");         Serial.println(data.windkt);
  Serial.print("windDir: ");        Serial.println(data.windDir);
  Serial.print("syncTime: ");       Serial.println(data.syncTime);
  Serial.print("timezoneOffset: "); Serial.println(data.timezoneOffset);
}

// Open-Meteo daily block. timezone=auto means these are true local
// midnight-to-midnight figures, covering the hours already past as well as
// the rest of the day, so no part of the calendar day is missing.
void parseDaily(String &json, WeatherData &data) {
  Serial.print("Daily response length: ");
  Serial.println(json.length());

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, json);
  if (error) {
    Serial.print("Daily parse error: ");
    Serial.println(error.c_str());
    return;
  }
  Serial.println("Daily parse OK");

  JsonObject daily = doc["daily"];
  const char* apiDate = daily["time"][0] | "";
  if (apiDate[0] == '\0') {
    Serial.println("Daily response has no date, rejecting");
    return;
  }

  // --- date guard ---
  // Only trust the figures if the day they describe matches the day the clock
  // says it is. Needs the current call to have supplied dt + timezone.
  if (data.currentValid) {
    char expected[11];
    localDateFromCurrent(data, expected, sizeof(expected));
    data.dayChecked = true;
    if (strcmp(apiDate, expected) != 0) {
      Serial.print("Date guard REJECTED: api=");
      Serial.print(apiDate);
      Serial.print(" expected=");
      Serial.println(expected);
      return;
    }
    Serial.print("Date guard OK: ");
    Serial.println(apiDate);
  } else {
    Serial.println("Date guard skipped: no valid clock from current call");
  }

  data.tempHigh      = daily["temperature_2m_max"][0].as<float>();
  data.tempLow       = daily["temperature_2m_min"][0].as<float>();
  data.rainMm        = daily["precipitation_sum"][0].as<float>();
  data.precipPercent = daily["precipitation_probability_max"][0].as<int>();
  data.uvIndex       = (int)daily["uv_index_max"][0].as<float>();
  data.uvMax         = 11;
  strncpy(data.localDate, apiDate, sizeof(data.localDate) - 1);
  data.localDate[sizeof(data.localDate) - 1] = '\0';
  data.dayValid      = true;

  Serial.println("--- Daily parsed values ---");
  Serial.print("localDate: ");     Serial.println(data.localDate);
  Serial.print("tempHigh: ");      Serial.println(data.tempHigh);
  Serial.print("tempLow: ");       Serial.println(data.tempLow);
  Serial.print("rainMm: ");        Serial.println(data.rainMm);
  Serial.print("precipPercent: "); Serial.println(data.precipPercent);
  Serial.print("uvIndex: ");       Serial.println(data.uvIndex);
}

#endif
