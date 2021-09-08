/*
#include "DMXAddrSwitch.h"

void DMXAddrSwitch::begin() {
  uint8_t i;

  dip_switch.begin_I2C(0x20);
  for (i = 0; i < sizeof(pins_map); i++) {
    dip_switch.pinMode(pins_map[i], INPUT);
  }
}

// Because pins couldn't be numbered 0-8 (because of the limited space for the pcb layout)
// the values need to be fetched one by one instead of in bulk
// get the address every 1 second
void DMXAddrSwitch::receiveAddr() {
  // get address only once a second
  curr_ms = millis();
  if (curr_ms > last_ms + 1000) {
    
    // get the address
    dmx_addr = 0;
    for (uint8_t i = 0; i < 9; i++) {
      dmx_addr <<= 1;
      dmx_addr += dip_switch.digitalRead(pins_map[i]);
    }
    dmx_addr += 1;

    last_ms = curr_ms;
  }
}
*/
