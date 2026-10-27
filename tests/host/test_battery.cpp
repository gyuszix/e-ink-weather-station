#include "arduino_stubs.h"
#include "../../weather_data.h"
#include "../../battery.h"
#include "test.h"

// The divider halves the cell voltage, so the ADC sees volts * 500 mV.
static WeatherData readAt(float cellVolts) {
  WeatherData d{};
  stubAdcMilliVolts = (uint32_t)(cellVolts * 1000.0f / VOLTAGE_DIVIDER_RATIO + 0.5f);
  readBattery(d);
  return d;
}

TEST(curve_points) {
  CHECK_NEAR(voltageToPercent(4.30), 100, 0.01);
  CHECK_NEAR(voltageToPercent(4.20), 100, 0.01);
  CHECK_NEAR(voltageToPercent(3.90), 80, 0.5);
  CHECK_NEAR(voltageToPercent(3.80), 60, 0.5);
  CHECK_NEAR(voltageToPercent(3.70), 40, 0.5);
  CHECK_NEAR(voltageToPercent(3.50), 20, 0.5);
  CHECK_NEAR(voltageToPercent(3.30), 0, 0.01);
  CHECK_NEAR(voltageToPercent(3.00), 0, 0.01);
}

TEST(curve_interpolates) {
  CHECK_NEAR(voltageToPercent(3.60), 30, 0.5);
  CHECK_NEAR(voltageToPercent(4.05), 90, 0.5);
}

TEST(curve_is_monotonic) {
  float prev = -1;
  for (int mv = 3000; mv <= 4300; mv += 5) {
    float pct = voltageToPercent(mv / 1000.0f);
    CHECK(pct >= prev);
    prev = pct;
  }
}

TEST(offset_applied_and_capped) {
  CHECK(readAt(4.20).batteryPercent == 100);
  CHECK(readAt(4.00).batteryPercent == 100);   // 86.7 + 15 caps at 100
  CHECK(readAt(3.70).batteryPercent == 40 + BATTERY_PERCENT_OFFSET);
  // just above the floor: float 3.30f compares below the double 3.30 literal
  CHECK_NEAR(readAt(3.32).batteryPercent, 2 + BATTERY_PERCENT_OFFSET, 1);
  CHECK_NEAR(readAt(3.70).batteryVoltage, 3.70, 0.002);
}

TEST(offset_ramps_out_below_curve_floor) {
  // #1: the display must still be able to reach 0%
  int mid = readAt((BATTERY_CURVE_EMPTY_V + BATTERY_EMPTY_V) / 2).batteryPercent;
  CHECK(mid > 0 && mid < BATTERY_PERCENT_OFFSET);
  CHECK(readAt(BATTERY_EMPTY_V).batteryPercent == 0);
  CHECK(readAt(3.00).batteryPercent == 0);
  CHECK(readAt(0.00).batteryPercent == 0);
}

TEST(percent_never_decreases_with_voltage) {
  int prev = -1;
  for (int mv = 3000; mv <= 4300; mv += 10) {
    int pct = readAt(mv / 1000.0f).batteryPercent;
    CHECK(pct >= prev);
    prev = pct;
  }
}

int main() { return runAllTests(); }
