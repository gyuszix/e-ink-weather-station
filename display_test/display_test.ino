#include <GxEPD2_3C.h>
#include <Fonts/FreeSansBold12pt7b.h>
#include "GxEPD2_display_selection_new_style.h"

void setup() {
  Serial.begin(115200);
  delay(3000);                       // let USB CDC enumerate
  Serial.println(">>> 1: serial up");

  Serial.println(">>> 2: calling display.init");
  display.init(115200, true, 50, false);
  Serial.println(">>> 3: display.init returned");

  display.setRotation(1);
  Serial.println(">>> 4: rotation set");

  display.setFullWindow();
  display.firstPage();
  Serial.println(">>> 5: entering draw loop");
  do {
    display.fillScreen(GxEPD_WHITE);
    display.setTextColor(GxEPD_BLACK);
    display.setFont(&FreeSansBold12pt7b);
    display.setCursor(10, 40);
    display.print("HELLO");
  } while (display.nextPage());
  Serial.println(">>> 6: draw complete");

  display.hibernate();
  Serial.println(">>> 7: done");
}

void loop() {
  delay(5000);
  Serial.println("still alive");
}
