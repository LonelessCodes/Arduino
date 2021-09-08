#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <DMXSerial.h>
#define dmxBaseCh 1

// called this way, it uses the default address 0x40
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

void setup() {
  DMXSerial.init(DMXProbe, 2); // configures RS485 for receive mode
  DMXSerial.maxChannel(dmxBaseCh + 1);

  pwm.begin();
  // In theory the internal oscillator is 25MHz but it really isn't
  // that precise. You can 'calibrate' by tweaking this number till
  // you get the frequency you're expecting!
  pwm.setOscillatorFrequency(27000000);  // The int.osc. is closer to 27MHz
  pwm.setPWMFreq(1600);  // This is the maximum PWM frequency
  
  // if you want to really speed stuff up, you can go into 'fast 400khz I2C' mode
  // some i2c devices dont like this so much so if you're sharing the bus, watch
  // out for this!
  Wire.setClock(400000);
}

byte value = 0;

void loop() {
  // wait for an incomming DMX packet.
  if (DMXSerial.receive()) {
    // Print the actual channel value, padding with a zero if needed
    value = DMXSerial.read(dmxBaseCh);
    pwm.setPin(0, value * 16);
  }
} // loop()
