#ifndef BATTERY_H
#define BATTERY_H

#include "weather_data.h"

#define BATTERY_ADC_PIN 4

// Set this to match your actual divider: ratio = (R1 + R2) / R2,
// where R2 is the resistor to GND. Two equal resistors = 2.0.
#define VOLTAGE_DIVIDER_RATIO 2.0

#define LOW_BATTERY_THRESHOLD 20  // percent

float voltageToPercent(float voltage) {
  // Rough LiPo discharge curve, piecewise linear between known points.
  if (voltage >= 4.2) return 100;
  if (voltage <= 3.3) return 0;

  struct Point { float v; float pct; };
  Point curve[] = {
    {4.2, 100},
    {3.9, 80},
    {3.8, 60},
    {3.7, 40},
    {3.5, 20},
    {3.3, 0},
  };

  for (int i = 0; i < 5; i++) {
    if (voltage <= curve[i].v && voltage >= curve[i + 1].v) {
      float span = curve[i].v - curve[i + 1].v;
      float frac = (voltage - curve[i + 1].v) / span;
      return curve[i + 1].pct + frac * (curve[i].pct - curve[i + 1].pct);
    }
  }
  return 0;
}

void readBattery(WeatherData &data) {
  uint32_t mv = analogReadMilliVolts(BATTERY_ADC_PIN);
  float voltage = (mv / 1000.0) * VOLTAGE_DIVIDER_RATIO;

  data.batteryVoltage = voltage;
  data.batteryPercent = (int)voltageToPercent(voltage);
}

#endif
