#ifndef WEATHER_FETCH_H
#define WEATHER_FETCH_H

#include <ArduinoHttpClient.h>
#include <Wire.h>
#include <Adafruit_BME280.h>
#include "weather_data.h"
#include "weather_parse.h"
#include "weather_request.h"
#include "location.h"

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
  String path = weatherRequestPath(LATITUDE, LONGITUDE);
  String response = httpGet(WEATHER_HOST, path);
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
