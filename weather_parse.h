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

// WMO weather code (Open-Meteo) -> the OWM-style icon code the bitmaps are
// keyed on. Freezing drizzle/rain map to snow, as OWM did.
void wmoToIcon(int code, bool isDay, char *out, size_t len) {
  const char* base;
  if      (code == 0)                                  base = "01";
  else if (code == 1)                                  base = "02";
  else if (code == 2)                                  base = "03";
  else if (code == 3)                                  base = "04";
  else if (code == 45 || code == 48)                   base = "50";
  else if (code == 56 || code == 57)                   base = "13";
  else if (code >= 51 && code <= 55)                   base = "09";
  else if (code == 66 || code == 67)                   base = "13";
  else if (code >= 61 && code <= 65)                   base = "10";
  else if ((code >= 71 && code <= 77) || code == 85 || code == 86) base = "13";
  else if (code >= 80 && code <= 82)                   base = "09";
  else if (code >= 95 && code <= 99)                   base = "11";
  else                                                 base = "01";
  snprintf(out, len, "%s%c", base, isDay ? 'd' : 'n');
}

// Open-Meteo current block. timezone=auto makes current.time local, as
// "YYYY-MM-DDTHH:MM" (start of the 15-minute model interval).
void parseCurrent(JsonDocument &doc, WeatherData &data) {
  JsonObject current = doc["current"];
  const char* time = current["time"] | "";
  if (strlen(time) < 16) {
    Serial.println("Current block missing or has no time, rejecting");
    return;
  }
  if (!current["temperature_2m"].is<float>()) {
    Serial.println("Current block has no temperature, rejecting");
    return;
  }

  data.tempCurrent = current["temperature_2m"].as<float>();
  wmoToIcon(current["weather_code"] | 0, (current["is_day"] | 1) == 1,
            data.iconCode, sizeof(data.iconCode));
  data.windkt      = current["wind_speed_10m"].as<float>();   // wind_speed_unit=kn
  int windDeg      = current["wind_direction_10m"].as<int>();
  strncpy(data.windDir, degreesToCardinal(windDeg), sizeof(data.windDir));

  strncpy(data.currentDate, time, 10);
  data.currentDate[10] = '\0';
  strncpy(data.syncTime, time + 11, 5);
  data.syncTime[5] = '\0';

  data.currentValid = true;

  Serial.println("--- Current parsed values ---");
  Serial.print("tempCurrent: ");    Serial.println(data.tempCurrent);
  Serial.print("iconCode: ");       Serial.println(data.iconCode);
  Serial.print("windkt: ");         Serial.println(data.windkt);
  Serial.print("windDir: ");        Serial.println(data.windDir);
  Serial.print("syncTime: ");       Serial.println(data.syncTime);
}

// Open-Meteo daily block. timezone=auto means these are true local
// midnight-to-midnight figures, covering the hours already past as well as
// the rest of the day, so no part of the calendar day is missing.
void parseDaily(JsonDocument &doc, WeatherData &data) {
  JsonObject daily = doc["daily"];
  const char* apiDate = daily["time"][0] | "";
  if (apiDate[0] == '\0') {
    Serial.println("Daily response has no date, rejecting");
    return;
  }

  // --- date guard ---
  // Only trust the figures if the day they describe matches the day the
  // current block says it is.
  if (data.currentValid) {
    const char* expected = data.currentDate;
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
    Serial.println("Date guard skipped: no valid current block");
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

// Single Open-Meteo response carrying both the current and daily blocks.
void parseWeather(String &json, WeatherData &data) {
  Serial.print("Weather response length: ");
  Serial.println(json.length());

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, json);
  if (error) {
    Serial.print("Weather parse error: ");
    Serial.println(error.c_str());
    return;
  }
  Serial.println("Weather parse OK");

  parseCurrent(doc, data);
  parseDaily(doc, data);
}

#endif
