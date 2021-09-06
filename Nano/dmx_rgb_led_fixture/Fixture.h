#ifndef __FIXTURE_H__
#define __FIXTURE_H__

#include "config.h"

class FixtureClass {
  private:
    // Not used.
    // all private information is in the global _fixture_XXX variables for speed and code size optimization.
    // @see Fixture.cpp.

  public:
    void begin();
    void set(
      byte dimmer_v, byte rand_dimmer_v,
      byte color_r, byte color_g, byte color_b,
#ifdef RGBW
      byte color_w,
#endif
      byte strobe_v
    );
    void reset();
    void update();
};

extern FixtureClass Fixture;

#endif
