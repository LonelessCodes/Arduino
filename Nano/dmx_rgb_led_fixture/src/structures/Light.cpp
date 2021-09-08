#include "Light.h"
#include "../helpers.h"

void Light::fade(unsigned long new_duration_ms, float new_to_lvl) {
  if (new_to_lvl == lvl) {
    active_fade = false;
    return;
  }

  from_lvl = lvl;
  to_lvl = new_to_lvl;
  start_ms = millis();
  duration_ms = new_duration_ms;
  active_fade = true;
}

void Light::update() {
  if (!active_fade) {
    return;
  }

  float time = (float)(millis() - start_ms) / duration_ms;
  if (time > 1.0f) {
    lvl = to_lvl;
    active_fade = false;
  }
  lvl = lerp_f(from_lvl, to_lvl, time);
}

void Light::set(float new_lvl) {
  lvl = new_lvl;
  active_fade = false;
}
