#ifndef WEATHER_REQUEST_H
#define WEATHER_REQUEST_H

#define WEATHER_HOST "api.open-meteo.com"

// Open-Meteo current conditions + today's daily figures, in one call.
// forecast_days=1 + timezone=auto returns exactly one daily bucket, the
// current local calendar day 00:00-24:00, and makes current.time local, so
// both blocks agree on what "today" is.
String weatherRequestPath(float latitude, float longitude) {
  String path = "/v1/forecast?latitude=";
  path += latitude;
  path += "&longitude=";
  path += longitude;
  path += "&current=temperature_2m,weather_code,is_day";
  path += ",wind_speed_10m,wind_direction_10m";
  path += "&daily=uv_index_max,temperature_2m_max,temperature_2m_min";
  path += ",precipitation_sum,precipitation_probability_max";
  path += "&wind_speed_unit=kn&timezone=auto&forecast_days=1";
  return path;
}

#endif
