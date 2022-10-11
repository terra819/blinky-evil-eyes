// EYEPIXEL.h
#ifndef EYEPIXEL_h
#define EYEPIXEL_h

#include <Arduino.h>

class EYEPIXEL {
  private:
    bool lit;
    int nextEventMillis;

  public:
    EYEPIXEL();
    EYEPIXEL(int nextEventMillis); // Creates the eye pair with the given pins. and sets the nextEventMillis
    bool toggleBlink(int currentEventMillis, int newNextEventMillis); // flip lit bool if currentEventMillis is greater than nextEventMillis. reset nextEventMillis with newNextEventMillis
    bool getLit();
    void wake(int newNextEventMillis); // set lit to true and set nextEventMillis
};

#endif
