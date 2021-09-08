#ifndef __COLOR_H__
#define __COLOR_H__

#include <Arduino.h>

#include "config.h"

class Color {
  public:
    byte red_lvl = 0;
    byte gre_lvl = 0;
    byte blu_lvl = 0;
    void set(byte red, byte gre, byte blu);
};

#endif
