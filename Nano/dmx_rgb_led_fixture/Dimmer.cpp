#include "Dimmer.h"

void Dimmer::set(byte val) {
  dimmer_lvl = (float)val / 255;
}
