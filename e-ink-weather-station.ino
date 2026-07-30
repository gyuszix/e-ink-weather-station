#include "wifi_connect.h"
#include "weather_data.h"
#include "weather_fetch.h"
#include "display_ui.h"

WeatherData weather;

void runCycle() {
  display.init(115200, true, 50, false);
  display.setRotation(1);
  connectWiFi();
  delay(2000);
  fetchWeatherData(weather);
  display.init(115200, true, 50, false);   // re-init after WiFi so SPI is clean
  display.setRotation(1);
  drawWeatherScreen(weather);
  display.hibernate();
}

void setup() {
  Serial.begin(115200);
  delay(3000);

  runCycle();

#if defined(ARDUINO_ARCH_ESP32)
  // ESP32: deep sleep, wakes with a full reboot back into setup()
  // esp_sleep_enable_timer_wakeup(30UL * 60UL * 1000000ULL);  // 30 sec TEST — change to 30UL * 60UL * 1000000ULL for real use
  esp_sleep_enable_timer_wakeup(30UL * 60UL * 1000000ULL);
  esp_deep_sleep_start();
#endif
}

void loop() {
#if !defined(ARDUINO_ARCH_ESP32)
  delay(30UL * 60UL * 1000UL);
  runCycle();
#endif
}