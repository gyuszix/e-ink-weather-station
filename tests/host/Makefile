# Host-side tests for the sketch's pure-logic headers.
#   make test       unit tests against recorded fixtures (offline)
#   make contract   live check against the real Open-Meteo API

ARDUINOJSON_VERSION := 7.4.3
DEPS  := .deps
BUILD := build

CXX      ?= c++
CXXFLAGS += -std=c++17 -Wall -Wextra -Werror -Wno-unused-function -Wno-unused-parameter \
            -I$(DEPS) -DFIXTURES='"$(CURDIR)/fixtures"'

ARDUINOJSON := $(DEPS)/ArduinoJson.h
HEADERS     := $(wildcard ../../*.h) arduino_stubs.h test.h
TESTS       := test_weather_parse test_battery_esp32 test_battery_noadc test_request

.PHONY: test contract clean

test: $(addprefix $(BUILD)/,$(TESTS))
	@set -e; for t in $^; do echo "== $$t"; $$t; done

contract: $(BUILD)/api_contract
	@url=$$($(BUILD)/api_contract path); echo "GET $$url"; \
	curl -sf --retry 3 --retry-delay 5 "$$url" -o $(BUILD)/live.json; \
	$(BUILD)/api_contract check $(BUILD)/live.json

$(ARDUINOJSON):
	@mkdir -p $(DEPS)
	curl -sfL -o $@ https://github.com/bblanchon/ArduinoJson/releases/download/v$(ARDUINOJSON_VERSION)/ArduinoJson-v$(ARDUINOJSON_VERSION).h

$(BUILD)/test_battery_esp32: test_battery.cpp $(HEADERS)
	@mkdir -p $(BUILD)
	$(CXX) $(CXXFLAGS) -DARDUINO_ARCH_ESP32 $< -o $@

$(BUILD)/test_battery_noadc: test_battery.cpp $(HEADERS)
	@mkdir -p $(BUILD)
	$(CXX) $(CXXFLAGS) $< -o $@

$(BUILD)/%: %.cpp $(HEADERS) $(ARDUINOJSON)
	@mkdir -p $(BUILD)
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm -rf $(BUILD) $(DEPS)
