#ifndef __RAND_DIMMER_H__
#define __RAND_DIMMER_H__

#include <Arduino.h>

#include "config.h"

class RandomDimmer {
  public:
    RandomDimmer();
    float rand_dimmer_lvls[NUM_LIGHTS] = {1.0f};
    void  set(byte new_rand_dimmer_lvl);
    void  shuffle();

  private:
    byte         rand_dimmer_lvl = 255;
    unsigned int rand_dimmer_shuffled[NUM_LIGHTS];
};

#endif
