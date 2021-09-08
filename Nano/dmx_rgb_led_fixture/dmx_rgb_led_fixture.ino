#include <Arduino.h>
#include <Wire.h>
#include <DMXSerial.h>

#include "src/Fixture.h"
#include "src/DMXAddrSwitch.h"

#define DIMMER_CH       0
#define RAND_DIMMER_CH  1
#define RED_CH          2
#define GREEN_CH        3
#define BLUE_CH         4
#define STROBE_CH       5
#define EFFECT_CH       6
#define EFFECT_SPEED_CH 7
#define DMX_LENGTH      8

void setup() {
  uint8_t i;

  // initialize DMX receiver in manual mode
  // configures RS485 for receive mode
  DMXSerial.init(DMXProbe, 2);
  DMXSerial.maxChannel(DMXAddrSwitch::dmx_addr + DMX_LENGTH);

  // DMXAddrSwitch::begin();

  Fixture.begin();
  Fixture.reset();

  // 400kHz i2c mode
  Wire.setClock(400000);

  pinMode(LED_BUILTIN, OUTPUT);
} // setup()

void loop() {
  // wait for an incomming DMX packet

  if (DMXSerial.receive()) {
    flash_led(50);

    // fetch new dmx address from the dip switch
    // DMXAddrSwitch::receiveAddr();

    // limit the number of DMX channels to read in
    DMXSerial.maxChannel(DMXAddrSwitch::dmx_addr + DMX_LENGTH);
  
    // read the values and feed them to the managers
    Fixture.set(
      DMXSerial.read(DMXAddrSwitch::dmx_addr + DIMMER_CH),
      DMXSerial.read(DMXAddrSwitch::dmx_addr + RAND_DIMMER_CH),
      DMXSerial.read(DMXAddrSwitch::dmx_addr + RED_CH),
      DMXSerial.read(DMXAddrSwitch::dmx_addr + GREEN_CH),
      DMXSerial.read(DMXAddrSwitch::dmx_addr + BLUE_CH),
      DMXSerial.read(DMXAddrSwitch::dmx_addr + STROBE_CH),
      DMXSerial.read(DMXAddrSwitch::dmx_addr + EFFECT_CH)
      // DMXSerial.read(dmx_start + EFFECT_SPEED_CH)
    );
  }

  // reset the fixture when no DMX data in 1 second
  else if (DMXSerial.noDataSince() > 1000) {
    flash_led(250);

    Fixture.reset();
  }

  Fixture.update();
} // loop()

unsigned long last_ms = 0;
unsigned long curr_ms;
bool led_on = false;

void flash_led(unsigned long time) {
  curr_ms = millis();
  if (curr_ms > last_ms + time) {
    digitalWrite(LED_BUILTIN, (led_on = !led_on) ? HIGH : LOW);
    last_ms = curr_ms;
  }
}
