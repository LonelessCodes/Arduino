// #define DMX_USE_PORT1

#include <DMXSerial.h>
#define dmxModePin 2 // configures RS485 for receive mode
#define dmxBaseCh 1

void setup() {
  DMXSerial.init(DMXProbe, dmxModePin);
  DMXSerial.maxChannel(dmxBaseCh + 1);

  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
}

byte value = 0;

void loop() {
  // wait for an incomming DMX packet.
  if (DMXSerial.receive()) {
    // Print the actual channel value, padding with a zero if needed
    value = DMXSerial.read(dmxBaseCh);
    if (value >= 128) digitalWrite(LED_BUILTIN, HIGH);
    else digitalWrite(LED_BUILTIN, LOW);
  }
} // loop()

// End.
