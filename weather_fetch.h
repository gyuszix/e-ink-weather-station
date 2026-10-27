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

  // ---- Open-Meteo current conditions + today's daily figures ----
  // One call. forecast_days=1 + timezone=auto returns exactly one daily
  // bucket, the current local calendar day 00:00-24:00, and makes
  // current.time local, so both blocks agree on what "today" is.
  String path = "/v1/forecast?latitude=";
  path += LATITUDE;
  path += "&longitude=";
  path += LONGITUDE;
  path += "&current=temperature_2m,weather_code,is_day";
  path += ",wind_speed_10m,wind_direction_10m";
  path += "&daily=uv_index_max,temperature_2m_max,temperature_2m_min";
  path += ",precipitation_sum,precipitation_probability_max";
  path += "&wind_speed_unit=kn&timezone=auto&forecast_days=1";

  String response = httpGet("api.open-meteo.com", path);
  if (response.length() > 0) {
    parseWeather(response, data);
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
