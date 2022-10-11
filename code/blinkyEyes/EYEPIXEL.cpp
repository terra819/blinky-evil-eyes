// EYEPIXEL.cpp
#include "EYEPIXEL.h"

EYEPIXEL::EYEPIXEL() {}

EYEPIXEL::EYEPIXEL(int nextEventMillis)
{
  lit = true;
  nextEventMillis = nextEventMillis;
}

bool EYEPIXEL::toggleBlink(int currentEventMillis, int newNextEventMillis)
{
  // flip lit bool if currentMillis is greater than nextEventMillis. reset nextEventMillis
  if (currentEventMillis >= nextEventMillis)
  {
    lit = !lit;
    nextEventMillis = newNextEventMillis;
  }
  return lit;
}

void EYEPIXEL::wake(int newNextEventMillis)
{
  // set lit to true and reset nextEventMillis
  lit = true;
  nextEventMillis = newNextEventMillis;
}

bool EYEPIXEL::getLit()
{
  return lit;
}