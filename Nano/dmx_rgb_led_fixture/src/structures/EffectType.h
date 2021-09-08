#ifndef __EFFECTTYPE_H__
#define __EFFECTTYPE_H__

#include "Light.h"

typedef struct {
  void (*start)(Light *lights);
  void (*update)(Light *lights);
  void (*stop)(Light *lights);
} EffectType;

#endif
