/* LED "Color Organ" for Adafruit Trinket and NeoPixel LEDs.

  Hardware requirements:
  - Adafruit Trinket or Gemma mini microcontroller (ATTiny85).
  - Adafruit Electret Microphone Amplifier (ID: 1063)
  - Several Neopixels, you can mix and match
   o Adafruit Flora RGB Smart Pixels (ID: 1260)
   o Adafruit NeoPixel Digital LED strip (ID: 1138)
   o Adafruit Neopixel Ring (ID: 1463)

  Software requirements:
  - Adafruit NeoPixel library

  Connections:
  - 5 V to mic amp +
  - GND to mic amp -
  - Analog pinto microphone output (configurable below)
  - Digital pin to LED data input (configurable below)

  Written by Adafruit Industries.  Distributed under the BSD license.
  This paragraph must be included in any redistribution.
*/
#include <Adafruit_NeoPixel.h>
#include <avr/power.h>

#define N_PIXELS   9  // Number of pixels you are using
#define LED_PIN    0  // NeoPixel LED strand is connected to GPIO #0 / D0
#define MIC_PIN    2  // Microphone
#define LENGTH    (15 * 1000) // Length of each animation
#define MODES      5

uint8_t 
  mode  = 0,
  offset = 0;
uint32_t color = 0x00ff96; // Start red
uint32_t prevTime;

Adafruit_NeoPixel  strip = Adafruit_NeoPixel(N_PIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  // This is the auto-speed doubler line, keep it in, it will
  // automatically double the speed when 16Mhz is selected!
  if (F_CPU == 16000000) clock_prescale_set(clock_div_1);

  pinMode(MIC_PIN, INPUT);

  strip.begin();
  prevTime = millis();
}

void setAll(byte red, byte green, byte blue) {
  for(int i = 0; i < N_PIXELS; i++ ) {
    strip.setPixelColor(i, strip.Color(red, green, blue)); 
  }
  strip.show();
}

void chaseAnim() {
  uint32_t t;
  uint32_t i;
  
  for (i = 0; i < 54; i++) {
    uint32_t c = 0;
    if (((offset + i) & 7) < 4) c = color; // 4 pixels on...
    strip.setPixelColor(   i, c); // First eye
    strip.setPixelColor(31 - i, c); // Second eye (flipped)
  }
  strip.show();
  offset++;
  delay(90);
}

void fadeInOutRGBAnim(){
  for(int j = 0; j < 3; j++ ) { 
    // Fade IN
    for(int k = 0; k < 256; k++) { 
      switch(j) { 
        case 0: setAll(k,0,0); break;
        case 1: setAll(0,k,0); break;
        case 2: setAll(0,0,k); break;
      }
      strip.show();
      delay(3);
    }
    // Fade OUT
    for(int k = 255; k >= 0; k--) { 
      switch(j) { 
        case 0: setAll(k,0,0); break;
        case 1: setAll(0,k,0); break;
        case 2: setAll(0,0,k); break;
      }
      strip.show();
      delay(3);
    }
  }
}

void bounce(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int i) {
  setAll(0,0,0);
  strip.setPixelColor(i, strip.Color(red/10, green/10, blue/10));
  for(int j = 1; j <= EyeSize; j++) {
    strip.setPixelColor(i + j, strip.Color(red, green, blue)); 
  }
  strip.setPixelColor(i+EyeSize+1, strip.Color(red/10, green/10, blue/10)); 
  strip.show();
  delay(SpeedDelay);
}

void CylonBounce(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay) {  
  for(int i = 0; i < N_PIXELS - EyeSize - 2; i++) {
    bounce(red, green, blue, EyeSize, SpeedDelay, i);
  }

  delay(ReturnDelay);

  for(int i = N_PIXELS - EyeSize - 2; i > 0; i--) {
    bounce(red, green, blue, EyeSize, SpeedDelay, i);
  }
  
  delay(ReturnDelay);
}

void Twinkle(byte red, byte green, byte blue, int Count, int SpeedDelay, boolean OnlyOne) {
  setAll(0,0,0);
  
  for (int i=0; i < Count; i++) {
    strip.setPixelColor(random(N_PIXELS), strip.Color(random(0,255),random(0,255),random(0,255)));
    strip.show();
    delay(SpeedDelay);
    if(OnlyOne) { 
      setAll(0,0,0); 
    }
   }
  
  delay(SpeedDelay);
}

void meteorRain(byte red, byte green, byte blue, byte meteorSize, byte meteorTrailDecay, boolean meteorRandomDecay, int SpeedDelay) {  
  setAll(0,0,0);
  
  for(int i = 0; i < N_PIXELS+N_PIXELS; i++) {
        
    // fade brightness all LEDs one step
    for(int j=0; j<N_PIXELS; j++) {
      if( (!meteorRandomDecay) || (random(10)>5) ) {
        fadeToBlack(j, meteorTrailDecay );        
      }
    }
    
    // draw meteor
    for(int j = 0; j < meteorSize; j++) {
      if( ( i-j <N_PIXELS) && (i-j>=0) ) {
        strip.setPixelColor(i-j, strip.Color(red, green, blue));
      } 
    }
   
    strip.show();
    delay(SpeedDelay);
  }
}

void fadeToBlack(int ledNo, byte fadeValue) {
  // NeoPixel
  uint32_t oldColor;
  uint8_t r, g, b;
  int value;
  
  oldColor = strip.getPixelColor(ledNo);
  r = (oldColor & 0x00ff0000UL) >> 16;
  g = (oldColor & 0x0000ff00UL) >> 8;
  b = (oldColor & 0x000000ffUL);

  r=(r<=10)? 0 : (int) r-(r*fadeValue/256);
  g=(g<=10)? 0 : (int) g-(g*fadeValue/256);
  b=(b<=10)? 0 : (int) b-(b*fadeValue/256);
  
  strip.setPixelColor(ledNo, strip.Color(r,g,b));
}

void loop() {
  uint32_t now = millis();
  if (prevTime + LENGTH < now) {
    prevTime = now;
    mode++;
    if (mode >= MODES) mode = 0;
  }

  uint8_t  i;
  switch (mode) {
    case 0:
      chaseAnim();
      break;
    case 1:
      fadeInOutRGBAnim();
      break;
    case 2:
      CylonBounce(0xff, 0, 0, 4, 10, 50);
      break;
    case 3:
      Twinkle(0xff, 0, 0, 20, 100, false);
      break;
    case 4:
      meteorRain(0xff,0xff,0xff,10, 64, true, 30);
      break;
  }
}
