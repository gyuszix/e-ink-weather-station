// Just enough of the Arduino API to compile the sketch's pure-logic headers
// on a desktop compiler. Behaviour mirrors the Arduino core where the tests
// depend on it (e.g. String += float keeps two decimals, String(f, n) keeps n).
#ifndef ARDUINO_STUBS_H
#define ARDUINO_STUBS_H

#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

class String : public std::string {
 public:
  using std::string::string;
  using std::string::operator+=;
  String() = default;
  String(const std::string &s) : std::string(s) {}
  String(float f, unsigned char decimals) {
    char buf[32];
    snprintf(buf, sizeof(buf), "%.*f", decimals, f);
    assign(buf);
  }
  String &operator+=(float f) {
    char buf[32];
    snprintf(buf, sizeof(buf), "%.2f", f);
    append(buf);
    return *this;
  }
};

// Swallows everything; set SERIAL_ECHO=1 in the environment to see the logs.
struct SerialStub {
  bool echo() const {
    static const bool on = getenv("SERIAL_ECHO") != nullptr;
    return on;
  }
  void print(const char *s) { if (echo()) fputs(s, stdout); }
  void print(char *s) { print((const char *)s); }
  void print(const String &s) { print(s.c_str()); }
  template <class T> void print(T v) { if (echo()) fputs(std::to_string(v).c_str(), stdout); }
  template <class T> void println(T v) { print(v); println(); }
  void println() { if (echo()) fputc('\n', stdout); }
};
inline SerialStub Serial;

// ADC: tests set the value the next reads return.
inline uint32_t stubAdcMilliVolts = 0;
inline uint32_t analogReadMilliVolts(int) { return stubAdcMilliVolts; }
inline void delayMicroseconds(unsigned) {}

#endif
