#define FASTLED_ESP8266_RAW_PIN_ORDER // this means we'll be using the raw esp8266 pin order -> GPIO_12, which is d6 on nodeMCU
#define LED_PIN 12                    // led data in connected to GPIO_12 (d6/nodeMCU)
#define LED_PWR_LIMIT 500             // 500mA - Power limit in mA (voltage is set in setup() to 5v)
#define NUM_LEDS 66
#include <FastLED.h>

CRGB leds[NUM_LEDS];

#include "EYEPIXEL.h"

// Begin Config eyes
int ledsPerPair = 2;
int showIntervalMin = 2500;
int showIntervalMax = 5000;
int hideIntervalMin = 50000;
int hideIntervalMax = 100000;
EYEPIXEL eyePixels[66]; // must match NUM_LEDS
// End Config eyes

int currentMillis = 0;

void setup()
{
  Serial.begin(9600);
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS).setCorrection(TypicalSMD5050).setTemperature(DirectSunlight).setDither(1);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, LED_PWR_LIMIT);
  FastLED.clear();
  FastLED.show();
  randomSeed(analogRead(A0));

  // Set current clock time
  currentMillis = millis();

  // Populate eye pixels
  for (int eyePixelsIndex = 0; eyePixelsIndex < NUM_LEDS;)
  {
    // pair up the eyes with the same nextEventMillis
    int nextEventMillis = getNextMillis(false);
    // loop each eye pair, changing the nextEventMillis for each set
    for (int i = 0; i < ledsPerPair; i++)
    {
      eyePixels[eyePixelsIndex] = EYEPIXEL(nextEventMillis);
      eyePixelsIndex++;
    }
  }
  Serial.println(F("Done loading eye pairs"));
  showStrip();
}

/* MAIN LOOP */

void loop()
{
  // Set current clock time
  currentMillis = millis();

  for (int eyePixelsIndex = 0; eyePixelsIndex < NUM_LEDS;)
  {
    // get lit for first pixel in pair to see if we should get the hide or show nextEventMillis range
    bool lit = eyePixels[eyePixelsIndex].getLit();
    // pair up the eyes with the same nextEventMillis
    int nextEventMillis = getNextMillis(lit);
    // loop each eye pair, changing the nextEventMillis for each set
    for (int i = 0; i < ledsPerPair; i++)
    {
      bool lit = eyePixels[eyePixelsIndex].toggleBlink(currentMillis, nextEventMillis);
      if (lit)
      {
        setPixel(eyePixelsIndex, 0x00, 0xff, 0x00);
      }
      else
      {
        setPixel(eyePixelsIndex, 0, 0, 0);
      }
      eyePixelsIndex++;
    }
  }

  showStrip();
}

int getNextMillis(bool lit)
{
  if (!lit)
  {
    int pairShowInterval = random(showIntervalMin, showIntervalMax);
    return currentMillis + pairShowInterval;
  }
  else
  {
    int pairHideInterval = random(hideIntervalMin, hideIntervalMax);
    return currentMillis + pairHideInterval;
  }
}

// Apply LED color changes
void showStrip()
{
  FastLED.show();
}

void setPixel(int Pixel, byte red, byte green, byte blue)
{
  leds[Pixel].r = red;
  leds[Pixel].g = green;
  leds[Pixel].b = blue;
}

void setAll(byte red, byte green, byte blue)
{
  for (int i = 0; i < NUM_LEDS; i++)
  {
    setPixel(i, red, green, blue);
  }
  showStrip();
}