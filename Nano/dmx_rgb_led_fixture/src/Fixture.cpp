#include <Arduino.h>
#include <Adafruit_PWMServoDriver.h>

#include "../config.h"
#include "helpers.h"
#include "Fixture.h"

#include "managers/Dimmer.h"
#include "managers/RandomDimmer.h"
#include "managers/Color.h"
#include "managers/Strobe.h"
#include "managers/Effect.h"

Adafruit_PWMServoDriver Fixture__driver[] = {
  Adafruit_PWMServoDriver(0x40),
  Adafruit_PWMServoDriver(0x41),
};
uint8_t Fixture__driver_len = sizeof(Fixture__driver) / sizeof(Adafruit_PWMServoDriver);

Dimmer Fixture__dimmer;
RandomDimmer Fixture__rand_dimmer;
Color Fixture__color;
Strobe Fixture__strobe;
Effects Fixture__effect;

void Fixture__writePWM(uint8_t led, byte val) {
  uint16_t driver_i = led / 16;
  uint8_t  led_i    = led % 16;

  if (driver_i >= Fixture__driver_len) return;

  Fixture__driver[driver_i].setPin(led_i, gamma[val]);
}

// FixtureClass

void FixtureClass::begin() {
  uint16_t i;
  for (i = 0; i < Fixture__driver_len; i++) {
    Fixture__driver[i].begin();
    Fixture__driver[i].setPWMFreq(1600);  // This is the maximum PWM frequency
  }
}

void FixtureClass::set(
  byte dimmer_v, byte rand_dimmer_v,
  byte color_r, byte color_g, byte color_b,
  byte strobe_v,
  byte effect_v
) {
  Fixture__dimmer.set(dimmer_v);
  Fixture__rand_dimmer.set(rand_dimmer_v);
  Fixture__color.set(color_r, color_g, color_b);
  Fixture__strobe.set(strobe_v);
}

void FixtureClass::reset() {
  Fixture__dimmer.set(DIMMER_DEFL);
  Fixture__rand_dimmer.set(RAND_DIMMER_DEFL);
  Fixture__color.set(RED_DEFL, GREEN_DEFL, BLUE_DEFL);
  Fixture__strobe.set(STROBE_DEFL);
  // Effects.set(EFFECT_DEFL);
  // Speed.set(EFFECT_SPEED_DEFL);
}

// #define RED_CORR 1.0f
// #define GRE_CORR 0.9f
// #define BLU_CORR 0.7f
#define RED_CORR 1.0f
#define GRE_CORR 1.0f
#define BLU_CORR 0.8f

void FixtureClass::update() {
  Fixture__strobe.update();
  // Fixture__effect.update();

  // update lights
  float brightness_all = Fixture__strobe.strobe_lvl * Fixture__dimmer.dimmer_lvl;
  float brightness_single;
  uint8_t i;

  for (i = 0; i < NUM_LIGHTS; i++) {
    brightness_single = brightness_all * Fixture__rand_dimmer.rand_dimmer_lvls[i] /* * effect.effect_light_lvls[i].lvl */;

    Fixture__writePWM(i * 4 + 0, brightness_single * RED_CORR * Fixture__color.red_lvl);
    Fixture__writePWM(i * 4 + 1, brightness_single * GRE_CORR * Fixture__color.gre_lvl);
    Fixture__writePWM(i * 4 + 2, brightness_single * BLU_CORR * Fixture__color.blu_lvl);
  }
}

// Create a single class instance. Multiple class instances are not supported.
FixtureClass Fixture;
