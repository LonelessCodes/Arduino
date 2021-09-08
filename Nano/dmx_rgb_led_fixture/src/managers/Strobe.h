#ifndef __STROBE_H__
#define __STROBE_H__

#include <Arduino.h>

class Strobe {
  private:
    unsigned long strobe_on_ms        = 50;
    unsigned long strobe_off_ms       = 0;
    unsigned long strobe_previous_ms  = 0;

  public:
    bool strobe_lvl = 1;

    void set(byte new_strobe);
    void update();
};

#endif
