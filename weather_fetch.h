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

void fetchOWM(WeatherData &data) {
  WiFiClient wifi;
  HttpClient client = HttpClient(wifi, "api.openweathermap.org", 80);

  String owmPath = "/data/2.5/forecast?lat=";
  owmPath += LATITUDE;
  owmPath += "&lon=";
  owmPath += LONGITUDE;
  owmPath += "&appid=";
  owmPath += OWM_API_KEY;

#if defined(ARDUINO_ARCH_ESP32)
  owmPath += "&units=metric&cnt=16";
#else
  owmPath += "&units=metric&cnt=8";
#endif

  client.get(owmPath);
  int statusCode = client.responseStatusCode();
  if (statusCode != 200) {
    Serial.print("OWM HTTP error: ");
    Serial.println(statusCode);
    client.stop();
    return;
  }

  Serial.print("OWM response length: ");
  Serial.println(client.contentLength());

JsonDocument doc;


  DeserializationError error = deserializeJson(doc, client);
  if (error) {
    Serial.print("OWM parse error: ");
    Serial.println(error.c_str());
    client.stop();
    return;
  }

  parseOWM(doc, data);
  client.stop();
}

void fetchWeatherData(WeatherData &data) {
  // ---- OWM current weather ----
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

  // ---- OWM forecast (stream parsed) ----
  fetchOWM(data);

  // ---- Open-Meteo UV ----
  String uvPath = "/v1/forecast?latitude=";
  uvPath += LATITUDE;
  uvPath += "&longitude=";
  uvPath += LONGITUDE;
  uvPath += "&daily=uv_index_max&timezone=auto&forecast_days=1";

  String uvResponse = httpGet("api.open-meteo.com", uvPath);
  if (uvResponse.length() > 0) {
    parseUV(uvResponse, data);
  }

  // ---- BME280 indoor ----
  if (!bmeReady) {
    bmeReady = bme.begin(0x76);
    if (!bmeReady) Serial.println("BME280 not found");
  }
  if (bmeReady) {
    data.tempIndoor     = bme.readTemperature();
    data.humidityIndoor = bme.readHumidity();
  }
}

#endif