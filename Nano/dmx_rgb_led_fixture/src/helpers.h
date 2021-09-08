#ifndef __HELPERS_H_
#define __HELPERS_H_

#include <Arduino.h>

float lerp_f(float a, float b, float t);
float mod_f(float a, float m);
extern const uint16_t PROGMEM gamma[256];

#endif
