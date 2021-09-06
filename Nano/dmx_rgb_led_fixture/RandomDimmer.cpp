#include "RandomDimmer.h"

RandomDimmer::RandomDimmer() {
  for (int i = 0; i < NUM_LIGHTS; i++) {
    rand_dimmer_shuffled[i] = i;
  }
}

void RandomDimmer::set(byte new_rand_dimmer_lvl) {
  if (new_rand_dimmer_lvl == rand_dimmer_lvl) {
    return;
  }

  rand_dimmer_lvl = new_rand_dimmer_lvl;

  if (rand_dimmer_lvl == 0 || rand_dimmer_lvl == 255) {
    shuffle();
  }

  float full_light = ((float) rand_dimmer_lvl / 255) * NUM_LIGHTS;
  float curr_light = floor(full_light);

  for (int i = 0; i < NUM_LIGHTS; i++) {
    if (curr_light > i) {
      rand_dimmer_lvls[rand_dimmer_shuffled[i]] = 1.0f;
    } else if (curr_light < i) {
      rand_dimmer_lvls[rand_dimmer_shuffled[i]] = 0.0f;
    } else {
      rand_dimmer_lvls[rand_dimmer_shuffled[i]] = full_light - curr_light;
    }
  }
}

void RandomDimmer::shuffle() {
  unsigned int t, j, i;
  for (i = 0; i < NUM_LIGHTS - 1; i++) {
    j = random(0, NUM_LIGHTS - i);

    t = rand_dimmer_shuffled[i];
    rand_dimmer_shuffled[i] = rand_dimmer_shuffled[j];
    rand_dimmer_shuffled[j] = t;
  }
}
