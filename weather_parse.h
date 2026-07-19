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

void parseCurrent(String &json, WeatherData &data) {
  Serial.print("Current response length: ");
  Serial.println(json.length());

  StaticJsonDocument<1024> doc;
  DeserializationError error = deserializeJson(doc, json);
  if (error) {
    Serial.print("Current parse error: ");
    Serial.println(error.c_str());
    return;
  }
  Serial.println("Current parse OK");

  data.tempCurrent    = doc["main"]["temp"];
  data.timezoneOffset = doc["timezone"].as<long>();
  strncpy(data.iconCode, doc["weather"][0]["icon"] | "01d", sizeof(data.iconCode));
  data.windkt         = doc["wind"]["speed"].as<float>() * 1.944;
  int windDeg         = doc["wind"]["deg"].as<int>();
  strncpy(data.windDir, degreesToCardinal(windDeg), sizeof(data.windDir));

  long dt     = doc["dt"].as<long>() + data.timezoneOffset;
  int hours   = (dt % 86400L) / 3600;
  int minutes = (dt % 3600) / 60;
  snprintf(data.syncTime, sizeof(data.syncTime), "%02d:%02d", hours, minutes);

  Serial.println("--- Current parsed values ---");
  Serial.print("tempCurrent: ");    Serial.println(data.tempCurrent);
  Serial.print("iconCode: ");       Serial.println(data.iconCode);
  Serial.print("windkt: ");         Serial.println(data.windkt);
  Serial.print("windDir: ");        Serial.println(data.windDir);
  Serial.print("syncTime: ");       Serial.println(data.syncTime);
  Serial.print("timezoneOffset: "); Serial.println(data.timezoneOffset);
}

void parseOWM(String &json, WeatherData &data) {
  Serial.print("OWM response length: ");
  Serial.println(json.length());

  StaticJsonDocument<4096> doc;
  DeserializationError error = deserializeJson(doc, json);
  if (error) {
    Serial.print("OWM parse error: ");
    Serial.println(error.c_str());
    return;
  }
  Serial.println("OWM parse OK");

  float maxTemp   = -100;
  float minTemp   =  100;
  float totalRain =  0;
  float maxPop    =  0;

  // today's local date in days since epoch
  long dt0   = doc["list"][0]["dt"].as<long>() + data.timezoneOffset;
  long today = dt0 / 86400L;

  for (int i = 0; i < 8; i++) {
    JsonObject entry = doc["list"][i];

    // convert slot to local date
    long slotDt  = entry["dt"].as<long>() + data.timezoneOffset;
    long slotDay = slotDt / 86400L;

    // skip if not today in local time
    if (slotDay != today) continue;

    float tmax = entry["main"]["temp_max"];
    float tmin = entry["main"]["temp_min"];
    if (tmax > maxTemp) maxTemp = tmax;
    if (tmin < minTemp) minTemp = tmin;

    if (entry.containsKey("rain")) {
      totalRain += entry["rain"]["3h"].as<float>();
    }

    float pop = entry["pop"].as<float>();
    if (pop > maxPop) maxPop = pop;
  }

  data.tempHigh      = maxTemp;
  data.tempLow       = minTemp;
  data.rainMm        = totalRain;
  data.precipPercent = (int)(maxPop * 100);

  Serial.println("--- OWM parsed values ---");
  Serial.print("tempHigh: ");      Serial.println(data.tempHigh);
  Serial.print("tempLow: ");       Serial.println(data.tempLow);
  Serial.print("rainMm: ");        Serial.println(data.rainMm);
  Serial.print("precipPercent: "); Serial.println(data.precipPercent);
}

void parseUV(String &json, WeatherData &data) {
  Serial.print("UV response length: ");
  Serial.println(json.length());

  StaticJsonDocument<512> doc;
  DeserializationError error = deserializeJson(doc, json);
  if (error) {
    Serial.print("UV parse error: ");
    Serial.println(error.c_str());
    return;
  }
  Serial.println("UV parse OK");

  data.uvIndex = (int)doc["daily"]["uv_index_max"][0].as<float>();
  data.uvMax   = 11;

  Serial.println("--- UV parsed values ---");
  Serial.print("uvIndex: "); Serial.println(data.uvIndex);
}

#endif