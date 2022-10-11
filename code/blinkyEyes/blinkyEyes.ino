#include "arduino_secrets.h"
/*
  Arduino IoT Cloud Variables description

  The following variables are automatically generated and updated when changes are made to the Thing

  bool awake;

  Variables which are marked as READ/WRITE in the Cloud Thing will also have functions
  which are called when their values are changed from the Dashboard.
  These functions are generated with the Thing and added at the end of this sketch.
*/

#include "thingProperties.h"
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
  // Initialize serial and wait for port to open:
  Serial.begin(9600);
  // This delay gives the chance to wait for a Serial Monitor without blocking if none is found
  delay(1500);

  // Defined in thingProperties.h
  initProperties();

  // Connect to Arduino IoT Cloud
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);

  /*
     The following function allows you to obtain more information
     related to the state of network and IoT Cloud connection and errors
     the higher number the more granular information you’ll get.
     The default is 0 (only errors).
     Maximum is 4
  */
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();

  // start fastled setup
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
  ArduinoCloud.update();

  // Set current clock time
  currentMillis = millis();

  Blink();
  showStrip();
}

/*
  Since Awake is READ_WRITE variable, onAwakeChange() is
  executed every time a new value is received from IoT Cloud.
*/
void onAwakeChange()
{
  // Add your code here to act upon Awake change
  if (awake)
  {
    Wake();
  }
}

void Blink()
{
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
}

void Wake()
{
  // Light all pairs at once, but still have their show time varying
  for (int eyePixelsIndex = 0; eyePixelsIndex < NUM_LEDS;)
  {
    // for wake we want the wake range to be more varied than a regular blink, so we're
    // using showMin and hideMin for the calc
    int pairShowInterval = random(showIntervalMin, hideIntervalMin);
    int nextEventMillis = currentMillis + pairShowInterval;
    // loop each eye pair, changing the nextEventMillis for each set
    for (int i = 0; i < ledsPerPair; i++)
    {
      eyePixels[eyePixelsIndex].wake(nextEventMillis);
      eyePixelsIndex++;
    }
  }
  awake = false;
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
