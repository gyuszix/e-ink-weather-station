#include <Adafruit_BME280.h>

#include "weather_data.h"
#include "weather_fetch.h"
#include "display_ui.h"

WeatherData weather;

void setup() {
  Serial.begin(115200);
  delay(1000);
  display.init(115200, true, 50, false);
  display.setRotation(1);
  fetchWeatherData(weather);
  drawWeatherScreen(weather);
  display.hibernate();
}

void loop() {
  delay(30UL * 60UL * 1000UL);
  fetchWeatherData(weather);
  drawWeatherScreen(weather);
  display.hibernate();
}