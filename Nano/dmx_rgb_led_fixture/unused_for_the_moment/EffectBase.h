// effects declarations

struct EffectClass { // Effect
  Light effect_light_lvls[NUM_LIGHTS];
  int   effect_type                   = 0;
  byte  effect_speed                  = 120;

  void set(byte new_effect_val);
  void update();
} Effect;

struct EffectBaseClass {
  void blank() {
    for (int i = 0; i < NUM_LIGHTS; i++) {
      Effect.effect_light_lvls[i].set(0.0f);
    }
  }
  virtual void start(){};
  virtual void update(){};
  virtual void stop(){};
};

struct : EffectBaseClass { // EffectAllOn
  void start() override {
    for (int i = 0; i < NUM_LIGHTS; i++) {
      Effect.effect_light_lvls[i].set(1.0f);
    }
  }
} EffectAllOn;

struct : EffectBaseClass { // EffectFadeOutRandom
  unsigned int rand_dimmer_shuffled[NUM_LIGHTS];
  float last_beat = 0.0f;

  void shuffle() {
    unsigned int t, j, i;
    for (i = 0; i < NUM_LIGHTS - 1; i++) {
      j = random(0, NUM_LIGHTS - i);

      t = rand_dimmer_shuffled[i];
      rand_dimmer_shuffled[i] = rand_dimmer_shuffled[j];
      rand_dimmer_shuffled[j] = t;
    }
  }

  void start() override {
    for (int i = 0; i < num_pins; i++) {
      rand_dimmer_shuffled[i] = i;
    }
    shuffle();
    blank();
  }
  void update() override {
    float beat = Speed.getCurrBeat(NUM_LIGHTS);
    float beat_mod = mod_f(beat, 1.0f);
    unsigned int index = floor(beat);

    // a new beat has begun
    if (beat_mod < last_beat) {
      Effect.effect_light_lvls[index].set(1.0f);
      Effect.effect_light_lvls[index].fade(Speed.beat_len * 1.5, 1.0f);
    }

    last_beat = beat_mod;
  }
} EffectFadeOutRandom;

struct : EffectBaseClass { // EffectFadeOutRandom
  void update() override {
    float phase = 1.0f - Speed.getCurrBeat(1);
    float lvl = phase * 1.5;
    // float lvl_cb = lvl * lvl * lvl;

    for (int i = 0; i < NUM_LIGHTS; i++) {
      Effect.effect_light_lvls[i].set(lvl);
    }
  }
} EffectFadeOutToBeat;

EffectBaseClass *effect_list[] = {
    &EffectAllOn,
    &EffectFadeOutToBeat,
};
#define effect_num (sizeof(effect_list) / sizeof(EffectBaseClass *))

void EffectClass::set(byte new_effect_val) {
  int new_effect_type = map(new_effect_val, 0, 256, 0, effect_num + 1);

  if (new_effect_type != effect_type) {
    effect_list[effect_type]->stop();
    effect_type = new_effect_type;
    effect_list[effect_type]->start();
  }
}

void EffectClass::update(){
  for (int i = 0; i < NUM_LIGHTS; i++) {
    effect_light_lvls[i].update();
  }
}