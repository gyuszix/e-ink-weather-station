// Live check against the real Open-Meteo API, using the exact request the
// firmware builds and the firmware's own parser.
//   api_contract path              -> prints the request path
//   api_contract check <file.json> -> exits non-zero if the response no
//                                     longer parses into a complete reading
#include "arduino_stubs.h"
#include "ArduinoJson.h"
#include "../../weather_data.h"
#include "../../weather_parse.h"
#include "../../weather_request.h"
#include "test.h"

int main(int argc, char **argv) {
  if (argc >= 2 && strcmp(argv[1], "path") == 0) {
    printf("http://%s%s\n", WEATHER_HOST, weatherRequestPath(42.3398f, -71.0892f).c_str());
    return 0;
  }
  if (argc >= 3 && strcmp(argv[1], "check") == 0) {
    String json = readFile(argv[2]);
    WeatherData d{};
    parseWeather(json, d);
    CHECK(d.currentValid);
    CHECK(d.dayChecked);
    CHECK(d.dayValid);
    CHECK(strlen(d.iconCode) == 3);
    CHECK(strlen(d.syncTime) == 5 && d.syncTime[2] == ':');
    CHECK(d.tempCurrent > -90 && d.tempCurrent < 60);
    CHECK(d.tempLow <= d.tempHigh);
    CHECK(d.precipPercent >= 0 && d.precipPercent <= 100);
    CHECK(d.uvIndex >= 0 && d.uvIndex < 20);
    printf("%s: temp %.1f icon %s wind %.1fkt %s, high %.1f low %.1f, date %s, sync %s\n",
           testFailures ? "FAIL" : "ok", d.tempCurrent, d.iconCode, d.windkt, d.windDir,
           d.tempHigh, d.tempLow, d.localDate, d.syncTime);
    return testFailures ? 1 : 0;
  }
  fprintf(stderr, "usage: %s path | check <file.json>\n", argv[0]);
  return 2;
}
