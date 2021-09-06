#include "Color.h"

#ifdef RGBW
void Color::set(byte red, byte gre, byte blu, byte wht) {
#else
void Color::set(byte red, byte gre, byte blu) {
#endif
  red_lvl = red;
  gre_lvl = gre;
  blu_lvl = blu;
#ifdef RGBW
  wht_lvl = wht;
#endif
}
