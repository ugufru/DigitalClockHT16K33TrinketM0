
// Based on Adafruit 7 Segment Backpack

#include <Adafruit_DotStar.h>
#include <Adafruit_NeoPixel.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"
#include "RTClib.h"

#define LEFT_BUTTON_PIN 3
#define RIGHT_BUTTON_PIN 4
#define DOTSTAR_DATA_PIN 7
#define DOTSTAR_CLOCK_PIN 8
#define DOTSTAR_BRIGHTNESS 255

Adafruit_DotStar dotstar = Adafruit_DotStar(1, DOTSTAR_DATA_PIN, DOTSTAR_CLOCK_PIN, DOTSTAR_BGR);
Adafruit_7segment matrix = Adafruit_7segment();
RTC_PCF8523 rtc;

void setup()
{
  initSerial();
  initButtons();
  initRTC();
  initDotStar();
  init7SegmentDisplay();
}

void error(const char* message)
{
  matrix.printError();
  matrix.writeDisplay();

  Serial.println(message);

  while (true)
  {
    dotstar.setPixelColor(0, 255, 0, 0);
    dotstar.show();
    delay(50);
    dotstar.setPixelColor(0, 0);
    dotstar.show();
    delay(50);
  }
}

void initSerial()
{
  Serial.begin(9600);
  unsigned long timesup = millis() + 2000;

  while (!Serial && millis() < timesup);

  Serial.println("Serial open...");
}

void initButtons()
{
  pinMode(LEFT_BUTTON_PIN, INPUT_PULLUP);
  pinMode(RIGHT_BUTTON_PIN, INPUT_PULLUP);
  
  Serial.println("Initialized buttons.");
}

void initDotStar()
{
  dotstar.begin();
  dotstar.show();
  dotstar.setBrightness(DOTSTAR_BRIGHTNESS);
  Serial.println("Trinket DotStar initialized.");
}

void initRTC()
{
  if (rtc.begin() == false) error("Could not initialize RTC.");
  if (rtc.initialized()) Serial.print("PCF8523 RTC initialized.");
  if (rtc.lostPower()) Serial.print("PCF8523 RTC lost power.");

  if (! rtc.initialized() || rtc.lostPower())
  {
    Serial.println("RTC is NOT initialized, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    //
    // Note: allow 2 seconds after inserting battery or applying external power
    // without battery before calling adjust(). This gives the PCF8523's
    // crystal oscillator time to stabilize. If you call adjust() very quickly
    // after the RTC is powered, lostPower() may still return true.
  }

  // When time needs to be re-set on a previously configured device, the
  // following line sets the RTC to the date & time this sketch was compiled
  //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  rtc.start();
  Serial.println("Initialized RTC.");
}

void init7SegmentDisplay()
{
  matrix.begin(0x70);
  matrix.setBrightness(8);
  matrix.blinkRate(0);
  matrix.writeDisplay();
  delay(1000);
}

uint8_t previousState = -1;

void loop()
{  
  DateTime now = rtc.now();

  now = checkButtons(now);

  uint8_t state = now.second() % 2;

  if (state != previousState)
  {
    Serial.println(now.timestamp());

    displayTime(now);

//    uint32_t color = Wheel(now.second() * 4.2);
//    color = dotstar.gamma32(color);
//    dotstar.setPixelColor(0, color);
//    dotstar.show();

    previousState = state;
  }

  delay(200);
}

DateTime checkButtons(DateTime now)
{
    int year = now.year();
    int month = now.month();
    int day = now.day();
    int hour = now.hour();
    int minute = now.minute();
    int second = now.second();

  if (digitalRead(LEFT_BUTTON_PIN) == LOW)
  {
    Serial.println("LEFT BUTTON");

    hour = (hour + 1) % 24;
    
    now = DateTime(year, month, day, hour, minute, second);

    rtc.adjust(now);

    displayTime(now);
  }

  if (digitalRead(RIGHT_BUTTON_PIN) == LOW)
  {
    Serial.println("RIGHT BUTTON");  

    minute = (minute + 1) % 60;

    now = DateTime(year, month, day, hour, minute, second);

    rtc.adjust(now);

    displayTime(now); 
}

  return (now);
}

void displayTime(DateTime now)
{
  int h = now.twelveHour() % 12;
  int m = now.minute();
  int s = now.second();
  boolean pm = now.hour() > 11;

  if (h == 0) h = 12;

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

uint32_t Wheel(byte WheelPos)
{
  if (WheelPos < 85)
  {
    return dotstar.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
  else if (WheelPos < 170)
  {
    WheelPos -= 85;
    return dotstar.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  else
  {
    WheelPos -= 170;
    return dotstar.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}
