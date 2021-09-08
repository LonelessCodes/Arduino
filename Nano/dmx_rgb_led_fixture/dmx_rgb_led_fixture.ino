#include <Arduino.h>
#include <Wire.h>
// #include <Adafruit_MCP23X17.h>
#include <DMXSerial.h>

#include "Fixture.h"

#define DIMMER_CH       0
#define RAND_DIMMER_CH  1
#define RED_CH          2
#define GREEN_CH        3
#define BLUE_CH         4
#define STROBE_CH       5
#define EFFECT_CH       6
#define EFFECT_SPEED_CH 7
#define DMX_LENGTH      8

/*
uint8_t dip_switch_pins_map[] = { 7, 6, 5, 4, 3, 2, 11, 10, 9, 8 };
Adafruit_MCP23X17 dip_switch;

// Because pins couldn't be numbered 0-8 (because of the limited space for the pcb layout)
// the values need to be fetched one by one instead of in bulk
void readDMXAddr(uint16_t *addr) {
  uint8_t i;

  *addr = 0;
  for (i = 0; i < 9; i++) {
    *addr <<= 1;
    *addr += dip_switch.digitalRead(dip_switch_pins_map[i]);
  }
  *addr += 1;
}
*/

uint16_t dmx_start = 1;

void setup() {
  uint8_t i;

  // initialize DMX receiver in manual mode
  // configures RS485 for receive mode
  DMXSerial.init(DMXProbe, 2);
  DMXSerial.maxChannel(dmx_start + DMX_LENGTH);

  Fixture.begin();
  Fixture.reset();

  /*
  dip_switch.begin_I2C(0x20);
  for (i = 0; i < sizeof(dip_switch_pins_map); i++) {
    dip_switch.pinMode(dip_switch_pins_map[i], INPUT);
  }
  */

  // 400kHz i2c mode
  Wire.setClock(400000);

  pinMode(LED_BUILTIN, OUTPUT);
} // setup()

void loop() {
  // wait 16ms for an incomming DMX packet, then continue updating => at least 60 Hz refresh rate of animations

  if (DMXSerial.receive()) {
    flash_led(50);
    /*
    // get address only once a second
    curr_ms = millis();
    if (curr_ms > last_ms + 1000) {
      readDMXAddr(&dmx_start);
      last_ms = curr_ms;
    }
    */

    // limit the number of DMX channels to read in
    DMXSerial.maxChannel(dmx_start + DMX_LENGTH);
  
    // read the values and feed them to the managers
    Fixture.set(
      DMXSerial.read(dmx_start + DIMMER_CH),
      DMXSerial.read(dmx_start + RAND_DIMMER_CH),
      DMXSerial.read(dmx_start + RED_CH),
      DMXSerial.read(dmx_start + GREEN_CH),
      DMXSerial.read(dmx_start + BLUE_CH),
      DMXSerial.read(dmx_start + STROBE_CH)

      // DMXSerial.read(dmx_start + EFFECT_CH),
      // DMXSerial.read(dmx_start + EFFECT_SPEED_CH)
    );
  }

  // reset the fixture when no DMX data in 2 seconds
  else if (DMXSerial.noDataSince() > 2000) {
    flash_led(100);

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
      digitalWrite(LED_BUILTIN, led_on = !led_on ? HIGH : LOW);
      last_ms = curr_ms;
    }
}
