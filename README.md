# E-Ink Weather Station

**Version 0.5** — feature-complete, cleaned up, ready for enclosure and PCB.

A low-power weather station built around a 2.9 inch three-colour e-paper display.
It pulls current conditions, a daily forecast, and a UV index from free weather
APIs over WiFi, reads indoor temperature and humidity from a local sensor, draws
everything to the e-paper panel, then goes to sleep and repeats every 30 minutes.

The whole thing is designed to be plugged into power (or, as of v0.2, run
entirely off a battery), left on a shelf, and forgotten about. The e-paper holds
its image with zero power, so between updates the device draws almost nothing.

This README is written first as a record of what I built and the decisions and
dead-ends along the way, and second as a guide for anyone who wants to build the
same thing. The [Troubleshooting](#troubleshooting) section in particular is
where most of the real lessons live.

---

<img width="4032" height="3024" alt="IMG_2173" src="https://github.com/user-attachments/assets/7234c0e1-eb15-4186-982e-b9114584e0f9" />


## Version history

- **v0.5** — All planned functionality complete. Code cleaned up and README
  brought fully up to date. Cut the number of cables needed by making
  custom-length cables for each connection instead of generic jumper wires.
  This is the last breadboard revision — next steps are a 3D-printed case and
  a custom PCB, both tracked in [Stage 3](#stage-3-permanent-build-planned).
- **v0.2** — Battery power. Added a LiPo cell, USB-C TP4056 charger module, and
  a voltage divider for battery percentage monitoring. Status bar now reports
  battery level alongside indoor temp/humidity and sync time.
- **v0.1** — Initial build. USB-powered, two supported boards (UNO R4 WiFi /
  ESP32-S3 Super Mini), WiFi + weather APIs + BME280 + e-paper display.

---

## What it does

```
        +--------------------------------------------------+
        |                                                  |
        |   OUTSIDE  20.1 C                    HI 21.3 C    |
        |   [icon]   feels like ...            LO 18.9 C    |
        |                                                  |
        |   WIND  4 kt NE        RAIN 0.4mm / 25%          |
        |   UV    2              SYNC 13:40                 |
        |                                                  |
        |   TEMP 22.4 C  HUM 48%  BAT 86%  SYNC 13:40      |
        |                                                  |
        +--------------------------------------------------+
                 (2.9" e-paper, black / white / red)
```

<img width="2561" height="1046" alt="IMG_2044" src="https://github.com/user-attachments/assets/19e5b499-c8af-479d-8019-f9ff28301670" />

On each wake cycle the device:

1. Connects to WiFi.
2. Fetches current weather from OpenWeatherMap.
3. Fetches a short forecast (today's high/low, rain, precipitation chance).
4. Fetches the daily UV index max from Open-Meteo.
5. Reads indoor temperature and humidity from a BME280 sensor.
6. Reads battery voltage and converts it to a percentage (ESP32-S3 only).
7. Draws it all to the e-paper display.
8. Sleeps for 30 minutes, then does it again.

---

## Two supported boards

This project runs on **either** of two boards. The code auto-detects which one
it is compiled for and adapts. Pick whichever you have.

| | Arduino UNO R4 WiFi | ESP32-S3 Super Mini |
| --- | --- | --- |
| Update method | `delay()` in `loop()` | deep sleep, reboots into `setup()` |
| I2C pull-ups | built in on hardware I2C pins | must be enabled in code |
| Power use between updates | higher (stays powered) | very low (deep sleep) |
| Battery friendly | not really | yes |
| Battery monitoring | not implemented | yes, via voltage divider (v0.2) |
| Ease of bring-up | smoother | fiddlier (see troubleshooting) |

If you care about running on a battery, use the ESP32-S3. If you just want the
easiest path and it will be plugged into the wall, the UNO R4 is more forgiving.

---

## Bill of materials

**Stage 1 (prototyping) - what you need to build what this README documents:**

- One of: **ESP32-S3 Super Mini** OR **Arduino UNO R4 WiFi**
- **Waveshare 2.9 inch e-paper display** (three-colour, black/white/red)
- **BME280** temperature/humidity sensor
- **Jumper wires** (Dupont) for breadboarding
- **USB-C cable**

**Stage 2 (battery) - added in v0.2:**

- **3.7V LiPo cell** (e.g. 800mAh, JST-PH 2.0mm connector), ideally with a
  built-in protection circuit
- **TP4056 USB-C charging module** with protection (DW01A + FS8205A) — this
  handles charging and passes battery power through to the board
- **Two resistors of equal value** (100kΩ recommended) for a voltage divider,
  used to safely scale battery voltage down for reading on an ADC pin
- **Breadboard + jumper wires**, for prototyping the divider before soldering
  it permanently

**Stage 3 (permanent build) - added later:**

- **Header pins** (female recommended, so modules stay removable)
- **Solid-core hookup wire** (22 AWG) for the soldered build
- **Soldering equipment** (iron, solder, flux, wick)
- Protoboard, or a custom PCB
- Enclosure (3D-printed case or project box) and nylon standoffs to mount it

---

## Reference Pictures

<img width="1170" height="1286" alt="IMG_1956" src="https://github.com/user-attachments/assets/6b53be2c-926c-42f7-98b8-63d8a29c58f5" />
<img width="1170" height="1005" alt="IMG_1957" src="https://github.com/user-attachments/assets/239c389a-6a38-482f-9513-d1b20d11b131" />
<img width="1934" height="1356" alt="Screenshot 2026-07-30 at 18 20 31" src="https://github.com/user-attachments/assets/961ab921-9b94-4fea-8dfe-c8e11fffe1dc" />
<img width="979" height="396" alt="Pasted image 20260707141931" src="https://github.com/user-attachments/assets/009787e5-e559-4dfa-bff7-1f1422306b36" />

## Pinouts

Reference pinouts for the two boards. Wire according to whichever you are using.

### Arduino UNO R4 WiFi

<!-- IMAGE PLACEHOLDER: Arduino UNO R4 WiFi pinout -->
<!-- Add your pinout image here, e.g.: ![UNO R4 pinout](docs/r4_pinout.png) -->

### ESP32-S3 Super Mini

<!-- IMAGE PLACEHOLDER: ESP32-S3 Super Mini pinout -->
<!-- Add your pinout image here, e.g.: ![ESP32-S3 pinout](docs/s3_pinout.png) -->

---

## Wiring

The wiring below is what this build uses. **It is not the only valid wiring.**
The control pins (CS, DC, RST, BUSY for the display, and the two I2C pins for the
sensor) can be moved to other free GPIOs if you prefer, as long as you update the
code to match. See [Changing the pins](#changing-the-pins) for how.

Note that **the display's SPI data/clock lines are fixed hardware SPI pins** on
each board and are not set in code. You wire them but they are not part of the
configurable pins.

### Arduino UNO R4 WiFi

```
                 ARDUINO UNO R4 WiFi
              +-------------------------+
   E-paper <--| D7  D8  D9  D10(SS)     |
              | BUSY DC RST  CS          |
              |                         |
   BME280  <--| A4(SDA)  A5(SCL)        |   <- fixed hardware I2C
              |                         |
              | 3V3  GND                |
              +-------------------------+
```

E-paper display:

| Display | UNO R4 pin |
|---------|-----------|
| VCC     | 3.3V      |
| GND     | GND       |
| MOSI    | D11 (hardware SPI) |
| SCK     | D13 (hardware SPI) |
| CS      | D10 (SS)  |
| DC      | D8        |
| RST     | D9        |
| BUSY    | D7        |

BME280 sensor:

| BME280 | UNO R4 pin |
|--------|-----------|
| VCC    | 3.3V      |
| GND    | GND       |
| SDA    | A4 (hardware I2C) |
| SCL    | A5 (hardware I2C) |
| CSB    | 3.3V (forces I2C mode) |
| SDO    | GND (sets I2C address 0x76) |

### ESP32-S3 Super Mini

```
                  ESP32-S3 SUPER MINI
              +-------------------------+
              |        [USB-C]          |
   E-paper <--| GP12 GP11 GP10 GP9 GP8 GP7
              | SDA  SCL   CS   DC RST BUSY
              |                         |
   BME280  <--| GP5(SDA)  GP6(SCL)      |   <- needs pull-ups in code
              |                         |
              | 3V3  GND                |
              +-------------------------+
```

E-paper display:

| Display | ESP32-S3 pin |
|---------|-------------|
| BUSY    | GP7         |
| RES     | GP8         |
| D/C     | GP9         |
| CS      | GP10        |
| SCL     | GP11        |
| SDA     | GP12        |
| GND     | GND         |
| VCC     | 3.3V        |

BME280 sensor:

| BME280 | ESP32-S3 pin |
|--------|-------------|
| VCC    | 3.3V        |
| GND    | GND         |
| SCL    | GP6         |
| SDA    | GP5         |
| CSB    | 3.3V (forces I2C mode) |
| SDO    | GND (sets I2C address 0x76) |

Two details that apply to both sensors regardless of board: **CSB must be tied
to 3.3V** or the BME280 goes into SPI mode and will not answer on I2C, and
**SDO to GND** sets the I2C address to 0x76, which is what the code expects.

### Battery wiring (ESP32-S3 only, v0.2)

The battery does not connect to the S3 directly — it goes through the TP4056
charging module first, which also handles USB-C charging input.

```
Battery(+) ---- B+  [TP4056]  OUT+ ---- ESP32-S3 5V
Battery(-) ---- B-             OUT- ---- ESP32-S3 GND
```

For battery percentage monitoring, a voltage divider taps off the same battery
line and feeds a scaled-down reading into a free ADC pin:

```
Battery(+) ----[R1: 100k]----+----[R2: 100k]---- GND
                              |
                          ESP32-S3 GP4 (ADC)
```

Both resistors are equal value, so the ADC pin sees exactly half the battery
voltage. The firmware multiplies the reading back up by 2 to get the real
battery voltage, then converts that to an approximate percentage. GP4 was
chosen because it doesn't conflict with the display's data/clock pins
(GP7-GP12) or the BME280 I2C pins (GP5/GP6), and it sits on ADC1, which is
unaffected by WiFi being active (ADC2 pins share hardware with the radio and
can behave unreliably while WiFi is on).

The divider draws a small continuous current from the battery whenever it's
connected (roughly 0.02mA at 100k/100k, negligible against an 800mAh cell), so
there's no need to switch it in and out.

---

## secrets.h

The project reads your private configuration from a `secrets.h` file that is
kept out of version control. Create it next to the main sketch with this
structure:

```cpp
#ifndef SECRETS_H
#define SECRETS_H

const char* WIFI_SSID     = "your_wifi_name";
const char* WIFI_PASSWORD = "your_wifi_password";
const char* OWM_API_KEY   = "your_openweathermap_api_key";

const float LATITUDE  = 00.0000;
const float LONGITUDE = 00.0000;

#endif
```

- **WIFI_SSID / WIFI_PASSWORD** are your network login.
- **OWM_API_KEY** is a free OpenWeatherMap API key. Register for a free account
  on their site and generate a key. The UV data comes from Open-Meteo, which
  needs no key at all.
- **LATITUDE / LONGITUDE** are your location, used for all three API calls. Look
  them up for wherever you want the weather from.

Keep this file private. Do not commit real credentials to a public repository.

---

## Libraries

Installed via the Arduino library manager (or `arduino-cli lib install`). The
versions below are what this build was compiled and tested against.

| Library | Version | Notes |
| --------- | --------- | ------- |
| GxEPD2 | 1.6.9 | e-paper driver, by Jean-Marc Zingg |
| Adafruit GFX Library | 1.12.6 | graphics primitives and fonts |
| Adafruit BME280 Library | 2.3.0 | sensor driver |
| Adafruit Unified Sensor | 1.1.15 | Adafruit sensor dependency |
| Adafruit BusIO | 1.17.4 | Adafruit I2C/SPI dependency |
| ArduinoJson | 7.4.3 | JSON parsing (note: v7 API) |
| ArduinoHttpClient | 0.6.1 | HTTP requests |

The `WiFi`, `Networking`, `Wire`, and `SPI` libraries come bundled with the
board core (the ESP32 core, or the Renesas core for the UNO R4).

---

## Flashing

Both boards use `arduino-cli`. Replace `<PORT>` with your actual serial port
(find it with `arduino-cli board list`).

### Arduino UNO R4 WiFi

```bash
arduino-cli compile --fqbn arduino:renesas_uno:unor4wifi --clean .
arduino-cli upload -p <PORT> --fqbn arduino:renesas_uno:unor4wifi .
arduino-cli monitor -p <PORT> --config baudrate=115200
```

### ESP32-S3 Super Mini

The ESP32-S3 needs two extra things: the `CDCOnBoot=cdc` flag so that serial
output is routed to the USB port, and `dtr=on,rts=on` on the monitor so the USB
serial actually streams. Without these you will see no serial output at all.

```bash
arduino-cli compile --fqbn esp32:esp32:esp32s3:CDCOnBoot=cdc --clean .
arduino-cli upload -p <PORT> --fqbn esp32:esp32:esp32s3:CDCOnBoot=cdc .
arduino-cli monitor -p <PORT> --config baudrate=115200,dtr=on,rts=on
```

---

## Changing the pins

If you want to use different GPIOs than the wiring above, you change them in two
places depending on whether it is a display pin or a sensor pin.

**Display pins (CS, DC, RST, BUSY)** are set in
`GxEPD2_display_selection_new_style.h`. This file is the stock GxEPD2 selection
header and contains constructor lines for dozens of boards. Only two branches
matter for this project: the `ARDUINO_ARCH_RENESAS` section for the UNO R4, and
the `ARDUINO_ESP32S3_DEV` section for the ESP32-S3. Find the constructor line for
your board and edit the pin numbers there, for example:

```cpp
display(GxEPD2_DRIVER_CLASS(/*CS=*/ 10, /*DC=*/ 9, /*RST=*/ 8, /*BUSY=*/ 7));
```

The rest of that file is boilerplate for boards this project does not use. It is
left intact so the file stays a drop-in match for the upstream GxEPD2 example,
but none of it affects your build unless that board is selected at compile time.

**Sensor pins (I2C SDA/SCL)** are set in `weather_fetch.h`, in the BME280 init
block. On the ESP32-S3 these are passed explicitly to `Wire.begin(sda, scl)`
along with the internal pull-up setup. On the UNO R4 the hardware I2C pins are
used automatically and nothing needs to be specified.

**Battery ADC pin** is set in `battery.h` (ESP32-S3 only). Change the
`analogReadMilliVolts()` pin argument if you move the divider to a different
GPIO — just make sure whatever you pick is on ADC1, not ADC2, per the note in
the battery wiring section above.

If you also change the display panel itself (not just the pins), you would change
the `GxEPD2_DRIVER_CLASS` define near the top of the selection header to match
your panel. This build uses `GxEPD2_290_C90c` (GDEM029C90, 128x296, SSD1680).

---

## Troubleshooting

This is the section I wish I'd had at the start. Most of these cost me real time.

### The R4 has built-in I2C pull-ups, the ESP32-S3 does not

I2C needs pull-up resistors on both data lines (SDA and SCL) to work. On the
**UNO R4** the hardware I2C pins (A4/A5) already have pull-ups as part of the
board design, so a plain `bme.begin(0x76)` just works.

On the **ESP32-S3 Super Mini** the GPIOs are general-purpose with no pull-ups on
the board. Wired the exact same way that worked on the R4, the sensor was
completely invisible: an I2C scanner found nothing at all. The fix is to enable
the ESP32's internal pull-ups in code before initialising the sensor:

```cpp
pinMode(5, INPUT_PULLUP);   // SDA
pinMode(6, INPUT_PULLUP);   // SCL
Wire.begin(5, 6);
Wire.setClock(100000);
bme.begin(0x76);
```

The internal pull-ups are weak (~45k) but fine for one sensor on short wires. If
you add more I2C devices or use longer wires and it gets unreliable, add proper
external 4.7k resistors instead.

Lesson: "it worked on the other board" does not mean the wiring is electrically
complete. Different boards provide different things for free.

### The ESP32-S3 has built-in deep sleep, the R4 does not

The whole low-power story depends on the ESP32's **deep sleep**. When the ESP32
calls `esp_deep_sleep_start()` it powers almost everything down and, on the timer
waking it, does a **full reboot back into `setup()`**. It never resumes in
`loop()`. So the correct pattern for the ESP32 is to put the whole fetch/draw
sequence in a function, call it from `setup()`, then sleep at the end of
`setup()`. `loop()` is effectively unused on the ESP32.

The **UNO R4** has no equivalent deep sleep in this setup, so it uses a plain
`delay(30 minutes)` in `loop()` and just stays powered the whole time. This is
why the R4 is not battery-friendly and the ESP32 is.

A trap this creates: if you leave fetch/draw code in **both** `setup()` and
`loop()`, the ESP32 runs the whole cycle twice per wake (once in setup, once in
the first loop iteration before sleeping). It looks like it is waking twice as
often as it should. Keep the work in one place.

### No serial output at all on the ESP32-S3

The ESP32-S3 uses native USB (USB-Serial/JTAG), not a separate USB-to-serial
chip. Two consequences bit me:

1. Serial output does not go to USB by default. You must compile with
   `CDCOnBoot=cdc` in the FQBN, or `Serial.println` goes nowhere you can see.
2. Even with that, the serial monitor needs `dtr=on,rts=on` before it will
   actually stream data. With those off you connect and see nothing.

Combined, the fix that finally showed output was compiling with `CDCOnBoot=cdc`
and monitoring with `--config baudrate=115200,dtr=on,rts=on`.

### The USB port keeps disappearing / upload fails partway

Also a native-USB side effect. On reset, sleep, or reboot, the ESP32-S3's USB
disconnects and reconnects, so the serial port vanishes and reappears (and
sometimes changes name). During development this means uploads occasionally fail
with "port busy" or "no such file", and the monitor drops with "port closed" on
every sleep.

This is annoying but harmless. It only affects development while tethered to the
computer. Once the device is flashed and running on its own power, nothing is
watching the port and it does not matter. If an upload fails, re-check the port
with `arduino-cli board list` and retry; putting the board into bootloader mode
(hold BOOT, tap RESET, release BOOT) before uploading helps.

### "OWM parse error: NoMemory" and the high/low not updating

The forecast JSON response is a few kilobytes. The original code parsed it with a
`StaticJsonDocument`, which allocates on the stack. On the UNO R4 the stack is too
small for that and parsing failed with `NoMemory`, so the forecast fields (high,
low, rain, precipitation) never populated.

Two fixes stack here. First, parse from the HTTP stream directly instead of
buffering the whole response into a String first, so the full response never sits
in RAM at once. Second, since this build uses **ArduinoJson v7**, use
`JsonDocument` (which manages memory on the heap automatically) rather than the
deprecated `StaticJsonDocument`/`DynamicJsonDocument`. On v6 you would use
`DynamicJsonDocument` instead.

### The display shows old content after adding WiFi

The display drew fine in a minimal test sketch, but in the full sketch it kept
showing the previous image and never updated, even though the serial log said the
refresh ran. The cause: bringing up WiFi **after** `display.init()` disturbed the
SPI setup the display relies on, on the ESP32. The fix is to re-initialise the
display right before drawing, after all the network activity is done:

```cpp
connectWiFi();
fetchWeatherData(weather);
display.init(115200, true, 50, false);   // re-init after WiFi
display.setRotation(1);
drawWeatherScreen(weather);
```

### The e-paper "hangs" on first draw

If `display.init` or the first refresh seems to hang forever, the display is most
likely waiting on the **BUSY** pin, which reports when a refresh is done. A bad
BUSY connection means the wait never ends. On these three-colour panels a full
refresh is genuinely slow (15-20 seconds) and flashes black/white/red before
settling, so give it time before assuming it is stuck. If it truly hangs, reseat
the BUSY wire first.

### General: flaky jumper wires

Several "bugs" in this build turned out to be jumper wires not making good
contact, on both the display data lines and the sensor. E-paper fails silently
when a data line is loose (the software reports success, nothing draws), which
makes it look like a code problem. When something works intermittently or draws
nothing with no error, reseat every wire at both ends, or swap the jumpers for
fresh ones, before chasing the code. This was enough of a recurring time sink
that it's part of what motivated the move to custom-length cables in v0.5 — a
proper crimped connection at the right length is far less prone to this than a
generic jumper wire pushed into a breadboard.

---

## Stage 2: battery power (v0.2)

The ESP32-S3 makes this viable because of deep sleep. Awake, the board with WiFi
active pulls brief bursts up to a few hundred milliamps; asleep it drops to
microamps, and the e-paper holds its image on no power. Average draw is dominated
by the short awake windows every 30 minutes, so a modest cell lasts a long time.

Current delivery is a non-issue: any hobby LiPo delivers far more than the peak
bursts need. What matters is **capacity** (mAh), which sets runtime between
charges. This build uses a 3.7V 800mAh cell.

**What was added:**

- A **TP4056 USB-C charging module**, wired between the battery and the board's
  5V/GND pins. The battery connects to the module's `B+`/`B-` pads, and the
  module's `OUT+`/`OUT-` feed the board — the same pins USB-C power would use,
  so the board can't tell the difference between the two power sources.
- A **voltage divider** (two 100kΩ resistors) tapped off the battery line,
  feeding a scaled-down reading into a free ADC-capable GPIO (GP4 in this
  build). This is purely for monitoring — it has no role in actually powering
  the board. See the [battery wiring](#battery-wiring-esp32-s3-only-v02)
  section above for the full circuit.
- The TP4056's onboard protection (DW01A + FS8205A) provides overcharge,
  over-discharge, and short-circuit protection, and this build's battery cell
  also has its own built-in protection circuit — belt and suspenders.

**Firmware changes for battery operation:**

- Battery voltage is read early in `runCycle()`, converted to a percentage, and
  stored on the shared `WeatherData` struct alongside the other readings, so it
  flows through the same pattern as temperature, humidity, etc.
- The status bar now shows `TEMP HUM BAT SYNC` in one line, with the battery
  percentage flagged in a different colour below a low-battery threshold.
- Charge current on the TP4056 defaults to 1A, which is roughly 1.25C for an
  800mAh cell — a bit hot for long-term battery health. Swapping the R3
  resistor on the module down (~2.4kΩ for ~500mA / 0.5C) is a planned future
  change, not yet done.

The battery does not change anything about the deep sleep cycle itself — the
30 minute wake interval and the fetch/draw sequence are unchanged from v0.1.
It just means the board no longer needs to be tethered to power.

The UNO R4 is not part of this stage; it does not deep sleep in this build and
stays fully powered.

## Stage 2.5: cable cleanup (v0.5)

With the breadboard wiring settled, the generic Dupont jumpers were replaced
with custom-length cables cut and crimped to fit the actual layout, rather than
whatever length happened to be in the jumper wire kit. This is purely a
tidiness/reliability change — no GPIO assignments moved.

The other change is a shared GND and VCC rail: instead of running a separate
ground wire and a separate power wire from the ESP32-S3 to each of the display,
the BME280, and the voltage divider individually, a single GND cable and a
single VCC cable now feed all three, branching at a small shared junction
close to the board rather than daisy-chaining or running three of each back to
the microcontroller. This cuts the total wire count noticeably without
changing any signal wiring, and makes the eventual protoboard/PCB layout
simpler since there's one ground plane and one power rail to route instead of
three of each.

## Stage 3: permanent build (planned)

Options, roughly cheapest to most polished:

- **Soldered protoboard.** Modules sit in female headers so they stay removable,
  wires soldered underneath. Cheap, permanent, forgiving of mistakes. The natural
  next step from a breadboard and the likely choice here.
- **Custom PCB.** Design in KiCad, order a small batch. Cleanest and reproducible,
  worth it only if the design settles or more than one unit is wanted.
- **Enclosure.** Whatever the board choice, a 3D-printed case or a project box with
  a window cut for the display, board on nylon standoffs, is what makes it look
  finished rather than like a prototype.

When building the protoboard, transcribe the wiring from the tables above, solder
the headers down first, then the wire runs, and continuity-check every connection
with a multimeter before plugging the modules in. Plan the layout so the underside
wire runs are short and do not cross.

---

## Credits

The e-paper driver library, **GxEPD2**, is written by **Jean-Marc Zingg**. This
project would not exist without it. The display selection header in this repo is
adapted from the example that ships with that library.
<https://github.com/ZinggJM/GxEPD2>

Weather data from **OpenWeatherMap** (current conditions and forecast) and
**Open-Meteo** (UV index). Indoor readings from a Bosch **BME280** via Adafruit's
libraries.

---

## License

This project is licensed under the MIT License. See the LICENSE file for details.

Note that the GxEPD2 library it depends on is licensed separately by its author
under GPL-3.0.
