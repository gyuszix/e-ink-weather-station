#ifndef WEATHER_FETCH_H
#define WEATHER_FETCH_H

#include <ArduinoHttpClient.h>
#include <Wire.h>
#include <Adafruit_BME280.h>
#include "weather_data.h"
#include "weather_parse.h"
#include "secrets.h"

Adafruit_BME280 bme;
bool bmeReady = false;

String httpGet(const char* host, String path) {
  WiFiClient wifi;
  HttpClient client = HttpClient(wifi, host, 80);
  client.get(path);
  int statusCode = client.responseStatusCode();
  String response = client.responseBody();
  if (statusCode != 200) {
    Serial.print("HTTP error: ");
    Serial.println(statusCode);
    return "";
  }
  return response;
}

void fetchWeatherData(WeatherData &data) {
  // stale values must never survive a failed fetch
  data.currentValid = false;
  data.dayValid     = false;
  data.dayChecked   = false;

  // ---- OWM current weather (temp, icon, wind, clock, timezone) ----
  String currentPath = "/data/2.5/weather?lat=";
  currentPath += LATITUDE;
  currentPath += "&lon=";
  currentPath += LONGITUDE;
  currentPath += "&appid=";
  currentPath += OWM_API_KEY;
  currentPath += "&units=metric";

  String currentResponse = httpGet("api.openweathermap.org", currentPath);
  if (currentResponse.length() > 0) {
    parseCurrent(currentResponse, data);
  }

  // ---- Open-Meteo daily figures for today (high/low/rain/precip/UV) ----
  // forecast_days=1 + timezone=auto returns exactly one bucket: the current
  // local calendar day, 00:00-24:00. Replaces the OWM 3-hour forecast, which
  // only ever returned *future* slots and so could not see the earlier part
  // of today.
  String dailyPath = "/v1/forecast?latitude=";
  dailyPath += LATITUDE;
  dailyPath += "&longitude=";
  dailyPath += LONGITUDE;
  dailyPath += "&daily=uv_index_max,temperature_2m_max,temperature_2m_min";
  dailyPath += ",precipitation_sum,precipitation_probability_max";
  dailyPath += "&timezone=auto&forecast_days=1";

  String dailyResponse = httpGet("api.open-meteo.com", dailyPath);
  if (dailyResponse.length() > 0) {
    parseDaily(dailyResponse, data);
  }

  // ---- BME280 indoor ----
  if (!bmeReady) {
#if defined(ARDUINO_ARCH_ESP32)
    pinMode(5, INPUT_PULLUP);   // SDA
    pinMode(6, INPUT_PULLUP);   // SCL
    Wire.begin(5, 6);
    Wire.setClock(100000);
#endif
    bmeReady = bme.begin(0x76);
    if (!bmeReady) Serial.println("BME280 not found");
  }
  if (bmeReady) {
    data.tempIndoor     = bme.readTemperature();
    data.humidityIndoor = bme.readHumidity();
  }
}

#endif
