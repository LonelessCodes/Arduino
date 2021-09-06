#include "helpers.h"

float lerp_f(float a, float b, float t) {
  return a + (b - a) * t;
}

float mod_f(float a, float m) {
  while (a >= m) {
    a -= m;
  }
  return a;
}
