#ifndef WIFI_CONNECT_H
#define WIFI_CONNECT_H

#if defined(ARDUINO_ARCH_ESP32)
  #include <WiFi.h>
#elif defined(ARDUINO_ARCH_RENESAS)
  #include <WiFiS3.h>
#endif

#include "secrets.h"

void connectWiFi() {
  if (WiFi.status() == WL_CONNECTED) return;

  Serial.print("Connecting to WIFI");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    // Wait for a real IP
    while (WiFi.localIP() == IPAddress(0, 0, 0, 0)) {
      delay(500);
      Serial.print(".");
    }
    Serial.println(" connected.");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println(" failed.");
  }
}

#endif