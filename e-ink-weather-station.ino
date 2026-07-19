#include "wifi_connect.h"
#include "weather_data.h"
#include "weather_fetch.h"
#include "display_ui.h"

WeatherData weather;

void setup() {
  Serial.begin(115200);
  delay(1000);
  display.init(115200, true, 50, false);
  display.setRotation(1);
  connectWiFi();
  delay(2000);
  fetchWeatherData(weather);
  drawWeatherScreen(weather);
  display.hibernate();
}

// ---- Uno R4 WiFi loop ----
void loop() {
  delay(30UL * 60UL * 1000UL);
  connectWiFi();
  delay(2000);
  fetchWeatherData(weather);
  drawWeatherScreen(weather);
  display.hibernate();
}

// ---- ESP32 loop  ----
// void loop() {
//   fetchWeatherData(weather);
//   drawWeatherScreen(weather);
//   display.hibernate();
//   esp_sleep_enable_timer_wakeup(30UL * 60UL * 1000000ULL);
//   esp_deep_sleep_start();
// }