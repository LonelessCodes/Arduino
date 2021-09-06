#ifndef __LIGHT_H__
#define __LIGHT_H__

#include <Arduino.h>

class Light {
  private:
    unsigned long duration_ms, start_ms;
    float from_lvl, to_lvl;
    bool active_fade = false;

  public:
    float lvl = 1.0f;

    void fade(unsigned long new_duration_ms, float new_to_lvl);
    void update();
    void set(float new_lvl);
};

#endif
