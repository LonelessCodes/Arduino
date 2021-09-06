#include "Strobe.h"

void Strobe::set(byte new_strobe) {
  if (new_strobe == 0) {
    strobe_off_ms = 0;
    strobe_lvl = 1;
  }
  else {
    // new_strobe = 1 - 255
    strobe_off_ms = 953 - (new_strobe * 3.7); // >0 - 18 Hz --- 10ms - 950ms
  }
}

void Strobe::update() {
  if (strobe_off_ms == 0) {
    return;
  }

  // check to see if it's time to change the state of the strobe
  unsigned long current_ms = millis();

  if ((strobe_lvl == 1) && (current_ms - strobe_previous_ms >= strobe_on_ms)) {
    strobe_lvl = 0;  // Turn it off
    strobe_previous_ms = current_ms;  // Remember the time
  }
  else if ((strobe_lvl == 0) && (current_ms - strobe_previous_ms >= strobe_off_ms)) {
    strobe_lvl = 1;  // turn it on
    strobe_previous_ms = current_ms;   // Remember the time
  }
}
