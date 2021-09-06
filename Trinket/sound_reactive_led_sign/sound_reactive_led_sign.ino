#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#define N_PIXELS   9  // Number of pixels you are using
#define LED_PIN    0  // NeoPixel LED strand is connected to GPIO #0 / D0
#define MIC_PIN    2  // Microphone

#define WAIT       30

int counter = 0;
byte strobe = 0;

Adafruit_NeoPixel  strip = Adafruit_NeoPixel(N_PIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

byte w_r, w_g, w_b;

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos, byte bri) {
  if (WheelPos < 85) {
    w_r = WheelPos * 3;
    w_g = 255 - WheelPos * 3;
    w_b = 0;
  }
  else if (WheelPos < 170) {
    WheelPos -= 85;

    w_r = 255 - WheelPos * 3;
    w_g = 0;
    w_b = WheelPos * 3;
  }
  else {
    WheelPos -= 170;

    w_r = 0;
    w_g = WheelPos * 3;
    w_b = 255 - WheelPos * 3;
  }
  return strip.Color(w_r + ((255 - w_r) * bri / 255), w_g + ((255 - w_g) * bri / 255), w_b + ((255 - w_b) * bri / 255));
}

void setup() {
  // This is the auto-speed doubler line, keep it in, it will
  // automatically double the speed when 16Mhz is selected!
  if (F_CPU == 16000000) clock_prescale_set(clock_div_1);

  pinMode(MIC_PIN, INPUT);

  strip.begin();
  strip.setBrightness(255);
}

void loop() {
  uint16_t i, j;

  for (j = 0; j < 256; j++) {
    // read the analog in value 100 times and count high levels
    counter = 0;

    for (int i = 0; i < 128; i++) if (1 - digitalRead(MIC_PIN)) counter++;

    if (counter > 30) strobe = 255;

    for (i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(
        i, 
        Wheel((i * 3 + j) & 255, strobe)
      );
    }
    strip.show();

    if (strobe >= 32) strobe -= 32;
    else strobe = 0;

    delay(WAIT);
  }
}
