#include "config.h"
#include "Dimmer.h"
#include "RandomDimmer.h"
#include "Color.h"
#include "Strobe.h"
#include "Fixture.h"

#include <Arduino.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver _fixture_driver[] = {
  Adafruit_PWMServoDriver(0x40),
  Adafruit_PWMServoDriver(0x41),
};
uint8_t _fixture_driver_len = sizeof(_fixture_driver) / sizeof(Adafruit_PWMServoDriver);

Dimmer _fixture_dimmer;
RandomDimmer _fixture_rand_dimmer;
Color _fixture_color;
Strobe _fixture_strobe;

void _fixture_writePWM(uint8_t led, uint16_t val) {
  uint16_t driver_i = led / 16;
  uint8_t  led_i    = led % 16;

  if (driver_i >= _fixture_driver_len) return;

  _fixture_driver[driver_i].setPin(led_i, val);
}

void FixtureClass::begin() {
  uint16_t i;
  for (i = 0; i < _fixture_driver_len; i++) {
    _fixture_driver[i].begin();
    _fixture_driver[i].setPWMFreq(1600);  // This is the maximum PWM frequency
  }
}

void FixtureClass::set(
  byte dimmer_v, byte rand_dimmer_v,
  byte color_r, byte color_g, byte color_b,
#ifdef RGBW
  byte color_w,
#endif
  byte strobe_v
) {
  _fixture_dimmer.set(dimmer_v);
  _fixture_rand_dimmer.set(rand_dimmer_v);
#ifdef RGBW
  _fixture_color.set(color_r, color_g, color_b, color_w);
#else
  _fixture_color.set(color_r, color_g, color_b);
#endif
  _fixture_strobe.set(strobe_v);
}

void FixtureClass::reset() {
  _fixture_dimmer.set(DIMMER_DEFL);
  _fixture_rand_dimmer.set(RAND_DIMMER_DEFL);
  _fixture_color.set(RED_DEFL, GREEN_DEFL, BLUE_DEFL);
  _fixture_strobe.set(STROBE_DEFL);
  // Effect.set(EFFECT_DEFL);
  // Speed.set(EFFECT_SPEED_DEFL);
}

void FixtureClass::update() {
  _fixture_strobe.update();
  // effect.update();

  // update lights
  float brightness_all = _fixture_strobe.strobe_lvl * _fixture_dimmer.dimmer_lvl;
  float brightness_single;
  uint8_t i;

  for (i = 0; i < NUM_LIGHTS; i++) {
    brightness_single = brightness_all * _fixture_rand_dimmer.rand_dimmer_lvls[i] /* * effect.effect_light_lvls[i].lvl */;

    _fixture_writePWM(i * 4 + 0, brightness_single * _fixture_color.red_lvl);
    _fixture_writePWM(i * 4 + 1, brightness_single * _fixture_color.gre_lvl);
    _fixture_writePWM(i * 4 + 2, brightness_single * _fixture_color.blu_lvl);
#ifdef RGBW
    _fixture_writePWM(i * 4 + 3, brightness_single * _fixture_color.wht_lvl);
#endif
  }
}

// Create a single class instance. Multiple class instances are not supported.
FixtureClass Fixture;
