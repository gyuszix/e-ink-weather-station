#include "arduino_stubs.h"
#include "../../weather_request.h"
#include "test.h"

TEST(path_contains_every_field_the_parser_reads) {
  String p = weatherRequestPath(42.3398f, -71.0892f);
  const char *needed[] = {
    "latitude=42.34", "longitude=-71.09",
    "temperature_2m", "weather_code", "is_day", "wind_speed_10m", "wind_direction_10m",
    "uv_index_max", "temperature_2m_max", "temperature_2m_min",
    "precipitation_sum", "precipitation_probability_max",
    "wind_speed_unit=kn", "timezone=auto", "forecast_days=1",
  };
  for (const char *n : needed) {
    testChecks++;
    if (p.find(n) == std::string::npos) {
      testFailures++;
      printf("  FAIL path is missing \"%s\": %s\n", n, p.c_str());
    }
  }
  CHECK(p.rfind("/v1/forecast?", 0) == 0);
}

int main() { return runAllTests(); }
