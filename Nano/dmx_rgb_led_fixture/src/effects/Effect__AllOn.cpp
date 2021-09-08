#include "../../config.h"
#include "Effect__AllOn.h"

namespace __Effect__AllOn {
  void start(Light *lights) {
    for (int i = 0; i < NUM_LIGHTS; i++) {
      lights[i].set(1.0f);
    }
  }
}

EffectType Effect__AllOn =  { &__Effect__AllOn::start, nullptr, nullptr };
