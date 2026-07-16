#ifndef DISPLAY_UI_H
#define DISPLAY_UI_H

#include <GxEPD2_3C.h>
#include <Fonts/FreeSansBold9pt7b.h>
#include <Fonts/FreeSans12pt7b.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSansBold12pt7b.h>
#include "GxEPD2_display_selection_new_style.h"
#include "bitmaps.h"
#include "weather_data.h"

const unsigned char* getBitmap(const char* icon) {
  if (strcmp(icon, "01d") == 0 || strcmp(icon, "01n") == 0) return icon_01d;
  if (strcmp(icon, "02d") == 0 || strcmp(icon, "02n") == 0) return icon_02d;
  if (strcmp(icon, "03d") == 0 || strcmp(icon, "03n") == 0) return icon_03d;
  if (strcmp(icon, "04d") == 0 || strcmp(icon, "04n") == 0) return icon_04d;
  if (strcmp(icon, "09d") == 0 || strcmp(icon, "09n") == 0) return icon_09d;
  if (strcmp(icon, "10d") == 0 || strcmp(icon, "10n") == 0) return icon_10d;
  if (strcmp(icon, "11d") == 0 || strcmp(icon, "11n") == 0) return icon_11d;
  if (strcmp(icon, "13d") == 0 || strcmp(icon, "13n") == 0) return icon_13d;
  if (strcmp(icon, "50d") == 0 || strcmp(icon, "50n") == 0) return icon_50d;
  return icon_01d;
}

void drawLayout(WeatherData &data) {
  int W = display.width();
  int H = display.height();

  int rowDivider    = 58;
  int statusDivider = 104;
  int vDiv1         = 92;
  int vDiv2         = 158;
  int cw            = W / 4;
  int c1 = 0, c2 = cw, c3 = cw * 2, c4 = cw * 3;

  display.setFont(&FreeSansBold9pt7b);
  display.setTextColor(GxEPD_RED);
  display.setTextSize(3);
  display.setCursor(4, 48);
  display.print((int)data.tempCurrent);
  display.setTextSize(1);

  display.setTextColor(GxEPD_BLACK);
  display.setFont(&FreeSans9pt7b);
  display.setCursor(80, 11);
  display.print("o");
  display.setFont(&FreeSans12pt7b);
  display.setCursor(68, 30);
  display.print("C");

  display.drawLine(vDiv1, 0, vDiv1, rowDivider, GxEPD_BLACK);
  display.drawBitmap(101, 2, getBitmap(data.iconCode), 50, 50, GxEPD_BLACK);
  display.drawLine(vDiv2, 0, vDiv2, rowDivider, GxEPD_BLACK);

  // hi/lo — label at fixed x, value at fixed x so they line up
  display.setFont(&FreeSans12pt7b);
  display.setTextColor(GxEPD_BLACK);
  display.setCursor(162, 24);
  display.print("HIGH:");
  display.setTextColor(GxEPD_RED);
  display.setFont(&FreeSansBold12pt7b);
  display.setCursor(224, 24);
  display.print((int)data.tempHigh);
  display.setTextColor(GxEPD_BLACK);
  display.setFont(&FreeSans9pt7b);
  display.setCursor(display.getCursorX(), 16);   // superscript
  display.print("o");
  display.setCursor(display.getCursorX(), 24);
  display.print("C");

  display.setFont(&FreeSans12pt7b);
  display.setTextColor(GxEPD_BLACK);
  display.setCursor(162, 45);
  display.print("LOW: ");
  display.setTextColor(GxEPD_RED);
  display.setFont(&FreeSansBold12pt7b);
  display.setCursor(224, 45);
  display.print((int)data.tempLow);
  display.setTextColor(GxEPD_BLACK);
  display.setFont(&FreeSans9pt7b);
  display.setCursor(display.getCursorX(), 37);   // superscript
  display.print("o");
  display.setCursor(display.getCursorX(), 45);
  display.print("C");

  display.drawLine(0, rowDivider, W, rowDivider, GxEPD_BLACK);
  display.drawLine(c2, rowDivider, c2, statusDivider, GxEPD_BLACK);
  display.drawLine(c3, rowDivider, c3, statusDivider, GxEPD_BLACK);
  display.drawLine(c4, rowDivider, c4, statusDivider, GxEPD_BLACK);

  int labelY = rowDivider + 15;
  int valueY = rowDivider + 38;

  display.setFont(&FreeSans9pt7b);
  display.setTextColor(GxEPD_BLACK);
  display.setCursor(c1 + 3, labelY);
  display.print("PRECIP");
  display.setFont(&FreeSansBold12pt7b);
  display.setCursor(c1 + 10, valueY);
  display.setTextColor(GxEPD_RED);
  display.print(data.precipPercent);
  display.setTextColor(GxEPD_BLACK);
  display.setFont(&FreeSans9pt7b);
  display.print("%");

  display.setFont(&FreeSans9pt7b);
  display.setTextColor(GxEPD_BLACK);
  display.setCursor(c2 + 3, labelY);
  display.print("RAIN");
  display.setFont(&FreeSansBold12pt7b);
  display.setCursor(c2 + 3, valueY);
  display.setTextColor(GxEPD_RED);
  display.print((int)data.rainMm, 1);
  display.setTextColor(GxEPD_BLACK);
  display.setFont(&FreeSans9pt7b);
  display.print("mm");

  display.setFont(&FreeSans9pt7b);
  display.setTextColor(GxEPD_BLACK);
  display.setCursor(c3 + 3, labelY);
  display.print("UV IDX");
  display.setFont(&FreeSansBold12pt7b);
  display.setTextColor(GxEPD_RED);
  display.setCursor(c3 + 10, valueY);
  display.print(data.uvIndex);
  display.setTextColor(GxEPD_BLACK);
  display.setFont(&FreeSans9pt7b);
  display.print("/");
  display.print(data.uvMax);

  display.setFont(&FreeSans9pt7b);
  display.setTextColor(GxEPD_BLACK);
  display.setCursor(c4 + 3, labelY);
  display.print("WIND");
  display.setFont(&FreeSansBold12pt7b);
  display.setCursor(c4 + 10, valueY);
  display.setTextColor(GxEPD_RED);
  display.print((int)data.windKph);
  display.setTextColor(GxEPD_BLACK);
  display.setFont(&FreeSans9pt7b);
  display.print("kph");

  display.drawLine(0, statusDivider, W, statusDivider, GxEPD_BLACK);
  display.setFont(&FreeSans9pt7b);
  display.setTextColor(GxEPD_BLACK);
  display.setCursor(3, 122);
  display.print("OWM");
  display.setCursor(50, 122);
  display.print("BME280");
  display.setCursor(162, 122);
  display.print("SYNC ");
  display.print(data.syncTime);
}

void drawWeatherScreen(WeatherData &data) {
  display.setFullWindow();
  display.firstPage();
  do {
    display.fillScreen(GxEPD_WHITE);
    drawLayout(data);
  } while (display.nextPage());
}

#endif