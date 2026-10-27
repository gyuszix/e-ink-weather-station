#ifndef BATTERY_H
#define BATTERY_H

#include "weather_data.h"

#define BATTERY_ADC_PIN 4

// Set this to match your actual divider: ratio = (R1 + R2) / R2,
// where R2 is the resistor to GND. Two equal resistors = 2.0.
#define VOLTAGE_DIVIDER_RATIO 2.0

#define LOW_BATTERY_THRESHOLD 20  // percent

// batteryPercent on boards without battery sensing (anything but the ESP32-S3)
#define BATTERY_UNKNOWN -1

// The ESP32 ADC is noisy enough that a single sample can be tens of mV out.
// This runs once per wake, so averaging costs nothing that matters.
#define BATTERY_ADC_SAMPLES 24

// Calibration headroom, in percentage points, added to the curve below.
// The board kept running long after the panel showed 0%, so the curve was
// reading empty well before the cell actually was. Tune this one number
// against what the board really does; 0 disables it.
//
// NOTE: this compensates in software for a reading that is also biased low
// in hardware. The divider is 100k/100k, so the ADC sees a ~50k source
// impedance where it wants <10k, and the sampling capacitor never fully
// charges. A 100nF capacitor across R2 fixes that at the source and will
// raise the measured voltage, at which point this offset should come down.
#define BATTERY_PERCENT_OFFSET 15

// Voltage at which the raw curve reads 0, and the voltage we now treat as
// genuinely flat. The offset is ramped out between the two so that 0% is
// still reachable instead of the display bottoming out at the offset.
#define BATTERY_CURVE_EMPTY_V 3.30
#define BATTERY_EMPTY_V       3.15

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

// Must be called before the radio comes up. WiFi TX pulls hundreds of mA and
// a small cell sags under that, so a sample taken with the radio active
// measures the sag rather than the cell's resting voltage.
void readBattery(WeatherData &data) {
#if !defined(ARDUINO_ARCH_ESP32)
  // analogReadMilliVolts() is ESP32-only, and only the S3 build has the divider
  data.batteryVoltage = 0;
  data.batteryPercent = BATTERY_UNKNOWN;
#else
  uint32_t total = 0;
  analogReadMilliVolts(BATTERY_ADC_PIN);   // discard first conversion
  for (int i = 0; i < BATTERY_ADC_SAMPLES; i++) {
    total += analogReadMilliVolts(BATTERY_ADC_PIN);
    delayMicroseconds(200);
  }
  uint32_t mv = total / BATTERY_ADC_SAMPLES;

  float voltage = (mv / 1000.0) * VOLTAGE_DIVIDER_RATIO;

  float percent;
  if (voltage >= BATTERY_CURVE_EMPTY_V) {
    percent = voltageToPercent(voltage) + BATTERY_PERCENT_OFFSET;
  } else {
    // below the curve's floor: ramp the offset out towards a true 0%
    float frac = (voltage - BATTERY_EMPTY_V) /
                 (BATTERY_CURVE_EMPTY_V - BATTERY_EMPTY_V);
    if (frac < 0) frac = 0;
    percent = BATTERY_PERCENT_OFFSET * frac;
  }
  if (percent > 100) percent = 100;
  if (percent < 0)   percent = 0;

  data.batteryVoltage = voltage;
  data.batteryPercent = (int)percent;

  Serial.print("Battery: ");
  Serial.print(voltage);
  Serial.print("V -> ");
  Serial.print(data.batteryPercent);
  Serial.print("% (raw curve ");
  Serial.print((int)voltageToPercent(voltage));
  Serial.print("%, offset +");
  Serial.print(BATTERY_PERCENT_OFFSET);
  Serial.println(")");
#endif
}

#endif
