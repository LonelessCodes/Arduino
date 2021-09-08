#ifndef __FIXTURE_H__
#define __FIXTURE_H__

#include <Arduino.h>

class FixtureClass {
  private:
    // Not used.
    // all private information is in the global Fixture__XXX variables for speed and code size optimization.
    // @see Fixture.cpp.

  public:
    void begin();
    void set(
      byte dimmer_v, byte rand_dimmer_v,
      byte color_r, byte color_g, byte color_b,
      byte strobe_v,
      byte effect_v
    );
    void reset();
    void update();
};

extern FixtureClass Fixture;

#endif
