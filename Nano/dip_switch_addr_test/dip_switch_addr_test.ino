#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MCP23X17.h>

uint8_t dip_switch_pins_map[] = { 7, 6, 5, 4, 3, 2, 11, 10, 9, 8 };
Adafruit_MCP23X17 dip_switch;

// Because pins couldn't be numbered 0-8 (because of the limited space for the pcb layout)
// the values need to be fetched one by one instead of in bulk
void readDMXAddr(uint16_t *addr) {
  uint8_t i;
  byte bit;

  *addr = 0;
  for (i = 0; i < 9; i++) {
    bit = dip_switch.digitalRead(dip_switch_pins_map[i]);
    Serial.print(bit);
    Serial.print(" ");
    *addr <<= 1;
    *addr += bit;
  }
  *addr += 1;
}

uint16_t dmx_start = 0;

void setup() {
  Serial.begin(9600);

  dip_switch.begin_I2C(0x20);
  for (uint8_t i = 0; i < sizeof(dip_switch_pins_map); i++) {
    dip_switch.pinMode(dip_switch_pins_map[i], INPUT);
  }

  // 400kHz i2c mode
  Wire.setClock(400000);

  delay(10);

  Serial.println("Ready");
}

unsigned long last_ms = 0;
unsigned long curr_ms;

void loop() {
  readDMXAddr(&dmx_start);

  Serial.print(dmx_start);
  Serial.print("\n");
}
