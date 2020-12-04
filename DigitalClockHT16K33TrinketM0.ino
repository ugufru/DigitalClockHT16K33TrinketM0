
// Based on Adafruit 7 Segment Backpack

#include <Adafruit_NeoPixel.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"
#include "RTClib.h"

Adafruit_7segment matrix = Adafruit_7segment();
RTC_PCF8523 rtc;

int hh = 0;
int mm = 0;
int ss = 0;

void setup()
{
  //  initSerial();
  initRTC();
  initDisplay();
}

void error()
{
  matrix.printError();
  matrix.writeDisplay();
  abort();

}

void initSerial()
{
  Serial.begin(9600);
  while (!Serial) ; // wait for serial
}

void initRTC()
{
  if (rtc.begin() == false) error();

  if (! rtc.initialized() || rtc.lostPower()) error();
  //  {
  //    Serial.println("RTC is NOT initialized, let's set the time!");
  // When time needs to be set on a new device, or after a power loss, the
  // following line sets the RTC to the date & time this sketch was compiled
  //    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  // This line sets the RTC with an explicit date & time, for example to set
  // January 21, 2014 at 3am you would call:
  // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  //
  // Note: allow 2 seconds after inserting battery or applying external power
  // without battery before calling adjust(). This gives the PCF8523's
  // crystal oscillator time to stabilize. If you call adjust() very quickly
  // after the RTC is powered, lostPower() may still return true.
  //  }

  // When time needs to be re-set on a previously configured device, the
  // following line sets the RTC to the date & time this sketch was compiled
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  // This line sets the RTC with an explicit date & time, for example to set
  // January 21, 2014 at 3am you would call:
  // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));

  // When the RTC was stopped and stays connected to the battery, it has
  // to be restarted by clearing the STOP bit. Let's do this to ensure
  // the RTC is running.
  rtc.start();
}

void initDisplay()
{
  matrix.begin(0x70);
  matrix.setBrightness(8);
  matrix.blinkRate(0);
  matrix.writeDisplay();
  delay(1000);
}

void loop()
{
  updateTime();
}

void updateTime()
{
  DateTime now = rtc.now();

  int h = now.hour() % 12;
  int m = now.minute();
  int s = now.second();
  boolean pm = now.hour() > 11;

  if (h == 0) h = 12;

  //  Serial.print(", h = ");
  //  Serial.print(h);
  //  Serial.print(", m = ");
  //  Serial.print(m);
  //  Serial.print(", s = ");
  //  Serial.println(s);

  // Draw the clock face.

  if (h < 10)
  {
       matrix.writeDigitRaw(0, 0);
  }
  else
  {
    matrix.writeDigitNum(0, h / 10, false);
  }

  matrix.writeDigitNum(1, h % 10, false);
  matrix.writeDigitNum(3, m / 10, false);
  matrix.writeDigitNum(4, m % 10, false);
  matrix.writeDigitRaw(2, 0xFF);

  int i = 0;
  if (s % 2 == 0) i += 2;
  if (pm) i += 4;
  matrix.writeDigitRaw(2, i);

  matrix.writeDisplay();
}
