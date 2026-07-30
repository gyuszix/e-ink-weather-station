// //test the esp32

// #include <GxEPD2_3C.h>
// #include <Fonts/FreeSansBold12pt7b.h>
// #include "GxEPD2_display_selection_new_style.h"

// void setup() {
//   Serial.begin(115200);
//   delay(3000);                       // let USB CDC enumerate
//   Serial.println(">>> 1: serial up");

//   Serial.println(">>> 2: calling display.init");
//   display.init(115200, true, 50, false);
//   Serial.println(">>> 3: display.init returned");

//   display.setRotation(1);
//   Serial.println(">>> 4: rotation set");

//   display.setFullWindow();
//   display.firstPage();
//   Serial.println(">>> 5: entering draw loop");
//   do {
//     display.fillScreen(GxEPD_WHITE);
//     display.setTextColor(GxEPD_BLACK);
//     display.setFont(&FreeSansBold12pt7b);
//     display.setCursor(10, 40);
//     display.print("HELLO");
//   } while (display.nextPage());
//   Serial.println(">>> 6: draw complete");

//   display.hibernate();
//   Serial.println(">>> 7: done");
// }

// void loop() {
//   delay(5000);
//   Serial.println("still alive");
// }

// //test the bme270
// #include <Wire.h>

// void setup() {
//   Serial.begin(115200);
//   delay(3000);
//   pinMode(5, INPUT_PULLUP);   // SDA
//   pinMode(6, INPUT_PULLUP);   // SCL
//   Wire.begin(5, 6);
//   Wire.setClock(100000);      // slow, reliable I2C speed
//   Serial.println("I2C scanner starting...");
// }

// void loop() {
//   byte count = 0;
//   for (byte addr = 1; addr < 127; addr++) {
//     Wire.beginTransmission(addr);
//     if (Wire.endTransmission() == 0) {
//       Serial.print("Found device at 0x");
//       Serial.println(addr, HEX);
//       count++;
//     }
//   }
//   if (count == 0) Serial.println("No I2C devices found");
//   else { Serial.print(count); Serial.println(" device(s) found"); }
//   Serial.println("---");
//   delay(5000);
// }


#include <Wire.h>
#include <Adafruit_BME280.h>

Adafruit_BME280 bme;
bool bmeReady = false;

void setup() {
  Serial.begin(115200);
  delay(3000);
  Serial.println(">>> BME280 smoke test");

  pinMode(5, INPUT_PULLUP);   // SDA
  pinMode(6, INPUT_PULLUP);   // SCL
  Wire.begin(5, 6);
  Wire.setClock(100000);

  bmeReady = bme.begin(0x76);
  if (!bmeReady) {
    Serial.println(">>> BME280 not found");
  } else {
    Serial.println(">>> BME280 OK");
  }
}

void loop() {
  if (bmeReady) {
    Serial.print("Temp: ");
    Serial.print(bme.readTemperature());
    Serial.print(" C   Humidity: ");
    Serial.print(bme.readHumidity());
    Serial.print(" %   Pressure: ");
    Serial.print(bme.readPressure() / 100.0);
    Serial.println(" hPa");
  } else {
    Serial.println("not ready");
  }
  delay(2000);
}