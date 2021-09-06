#ifndef __DIMMER_H__
#define __DIMMER_H__

#include <Arduino.h>

class Dimmer {
  public:
    float dimmer_lvl = 1.0f;

    void set(byte val);
};

#endif
