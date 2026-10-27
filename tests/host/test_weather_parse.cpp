#include "arduino_stubs.h"
#include "ArduinoJson.h"
#include "../../weather_data.h"
#include "../../weather_parse.h"
#include "test.h"

#ifndef FIXTURES
#define FIXTURES "fixtures"
#endif

static WeatherData parseFixture(const char *name) {
  WeatherData d{};
  String json = readFile(std::string(FIXTURES "/") + name + ".json");
  parseWeather(json, d);
  return d;
}

TEST(clear_day_parses_every_field) {
  WeatherData d = parseFixture("clear_day");
  CHECK(d.currentValid);
  CHECK(d.dayChecked);
  CHECK(d.dayValid);
  CHECK_NEAR(d.tempCurrent, 9.8, 0.001);
  CHECK_STR(d.iconCode, "01d");
  CHECK_NEAR(d.windkt, 3.7, 0.001);
  CHECK_STR(d.windDir, "E");
  CHECK_STR(d.syncTime, "17:30");
  CHECK_STR(d.currentDate, "2026-10-27");
  CHECK_STR(d.localDate, "2026-10-27");
  CHECK_NEAR(d.tempHigh, 13.5, 0.001);
  CHECK_NEAR(d.tempLow, 4.0, 0.001);
  CHECK_NEAR(d.rainMm, 0.0, 0.001);
  CHECK(d.precipPercent == 1);
  CHECK(d.uvIndex == 3);
  CHECK(d.uvMax == 11);
}

TEST(freezing_rain_at_night) {
  WeatherData d = parseFixture("freezing_rain_night");
  CHECK(d.currentValid);
  CHECK(d.dayValid);
  CHECK_NEAR(d.tempCurrent, -0.6, 0.001);
  CHECK_STR(d.iconCode, "13n");
  CHECK_STR(d.windDir, "N");      // 339 deg wraps back to north
  CHECK_STR(d.syncTime, "23:45");
  CHECK_NEAR(d.rainMm, 4.7, 0.001);
  CHECK(d.precipPercent == 92);
  CHECK(d.uvIndex == 0);          // 0.85 truncates
}

TEST(date_guard_rejects_other_day) {
  WeatherData d = parseFixture("date_mismatch");
  CHECK(d.currentValid);
  CHECK(d.dayChecked);
  CHECK(!d.dayValid);
  CHECK_STR(d.syncTime, "00:00");
}

TEST(missing_current_block) {
  WeatherData d = parseFixture("no_current");
  CHECK(!d.currentValid);
  CHECK(!d.dayChecked);           // no clock to check against
  CHECK(d.dayValid);
}

TEST(current_without_temperature_is_rejected) {
  WeatherData d = parseFixture("current_no_temp");
  CHECK(!d.currentValid);
}

TEST(missing_daily_block) {
  WeatherData d = parseFixture("no_daily");
  CHECK(d.currentValid);
  CHECK(!d.dayValid);
}

TEST(truncated_json_sets_nothing) {
  WeatherData d = parseFixture("truncated");
  CHECK(!d.currentValid);
  CHECK(!d.dayValid);
  CHECK(!d.dayChecked);
}

TEST(empty_body_sets_nothing) {
  WeatherData d{};
  String empty;
  parseWeather(empty, d);
  CHECK(!d.currentValid);
  CHECK(!d.dayValid);
}

TEST(wmo_codes_map_to_icons) {
  struct { int code; const char *icon; } cases[] = {
    {0, "01"}, {1, "02"}, {2, "03"}, {3, "04"},
    {45, "50"}, {48, "50"},
    {51, "09"}, {53, "09"}, {55, "09"}, {56, "13"}, {57, "13"},
    {61, "10"}, {63, "10"}, {65, "10"}, {66, "13"}, {67, "13"},
    {71, "13"}, {73, "13"}, {75, "13"}, {77, "13"},
    {80, "09"}, {81, "09"}, {82, "09"}, {85, "13"}, {86, "13"},
    {95, "11"}, {96, "11"}, {99, "11"},
    {-1, "01"}, {42, "01"}, {100, "01"},  // unknown codes fall back to clear
  };
  for (auto &c : cases) {
    char out[4], expected[4];
    wmoToIcon(c.code, true, out, sizeof(out));
    snprintf(expected, sizeof(expected), "%sd", c.icon);
    CHECK_STR(out, expected);
    wmoToIcon(c.code, false, out, sizeof(out));
    snprintf(expected, sizeof(expected), "%sn", c.icon);
    CHECK_STR(out, expected);
  }
}

TEST(cardinal_boundaries) {
  struct { int deg; const char *dir; } cases[] = {
    {0, "N"}, {22, "N"}, {23, "NE"}, {67, "NE"}, {68, "E"}, {112, "E"},
    {113, "SE"}, {158, "S"}, {203, "SW"}, {248, "W"}, {293, "NW"},
    {337, "NW"}, {338, "N"}, {360, "N"},
  };
  for (auto &c : cases) CHECK_STR(degreesToCardinal(c.deg), c.dir);
}

int main() { return runAllTests(); }
