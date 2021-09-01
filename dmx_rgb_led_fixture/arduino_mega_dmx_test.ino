#define DMX_USE_PORT1

#include <Arduino.h>
#include <DMXSerial.h>
#include <SoftPWM.h>

unsigned int DMX_START = 16; // DMX channel 16 is the start channel. dimmer 16, rand dimer 17, red 18
#define DMX_LENGTH      8

#define DIMMER_CH       0
#define RAND_DIMMER_CH  1
#define RED_CH          2
#define GREEN_CH        3
#define BLUE_CH         4
#define STROBE_CH       5
#define EFFECT_CH       6
#define EFFECT_SPEED_CH 7

// defaults

#define DIMMER_DEFL       255
#define RAND_DIMMER_DEFL  255
#define RED_DEFL          255
#define GREEN_DEFL        255
#define BLUE_DEFL         255
#define STROBE_DEFL       0
#define EFFECT_DEFL       0
#define EFFECT_SPEED_DEFL 120

const unsigned int dip_pins[] = { 22, 23, 24, 25, 26, 27, 28, 29, 30 }; // MSB to LSB
const unsigned int num_dip_pins = sizeof(dip_pins) / sizeof(unsigned int);
const unsigned int led_pins[] = {
  2, 3, 4,
  5, 6, 7,
  8, 9, 10,
  11, 12, 13,
  44, 45, 46,
  // digital pins for softpwm
  32, 34, 36,
};
#define pwmWrite(pin, val) ((pin) <= 13 || ((pin) >= 44 && (pin) <= 46) ? analogWrite(pin, val) : SoftPWMSet(pin, val))
const unsigned int num_pins = sizeof(led_pins) / sizeof(unsigned int);
const unsigned int num_lights = num_pins / 3;

// global variables, helper managers

struct { // Dimmer
  float dimmer_lvl = 1.0f;

  void set(byte val) {
    dimmer_lvl = (float)val / 255;
  }
} Dimmer;

struct { // RandomDimmer
  byte          rand_dimmer_lvl                   = 255;
  float         rand_dimmer_lvls[num_lights]      = {1.0f};
  unsigned int  rand_dimmer_shuffled[num_lights];

  void init() {
    for (int i = 0; i < num_pins; i++) {
      rand_dimmer_shuffled[i] = i;
    }
  }

  void shuffle() {
    unsigned int t, j, i;
    for (i = 0; i < num_lights - 1; i++) {
      j = random(0, num_lights - i);

      t = rand_dimmer_shuffled[i];
      rand_dimmer_shuffled[i] = rand_dimmer_shuffled[j];
      rand_dimmer_shuffled[j] = t;
    }
  }

  void set(byte new_rand_dimmer_lvl) {
    if (new_rand_dimmer_lvl == rand_dimmer_lvl) {
      return;
    }

    rand_dimmer_lvl = new_rand_dimmer_lvl;

    if (rand_dimmer_lvl == 0 || rand_dimmer_lvl == 255) {
      shuffle();
    }

    float full_light = ((float) rand_dimmer_lvl / 255) * num_lights;
    float curr_light = floor(full_light);

    for (int i = 0; i < num_lights; i++) {
      if (curr_light > i) {
        rand_dimmer_lvls[rand_dimmer_shuffled[i]] = 1.0f;
      } else if (floor < i) {
        rand_dimmer_lvls[rand_dimmer_shuffled[i]] = 0.0f;
      } else {
        rand_dimmer_lvls[rand_dimmer_shuffled[i]] = full_light - curr_light;
      }
    }
  }
} RandomDimmer;

struct { // Color
  byte red_lvl = 255;
  byte gre_lvl = 255;
  byte blu_lvl = 255;

  void set(byte red, byte gre, byte blu) {
    red_lvl = red;
    gre_lvl = gre;
    blu_lvl = blu;
  }
} Color;

struct { // Strobe
  unsigned long strobe_on_ms        = 50;
  unsigned long strobe_off_ms       = 0;
  bool          strobe_lvl          = 1;
  unsigned long strobe_previous_ms  = 0;

  void set(byte new_strobe) {
    if (new_strobe == 0) {
      strobe_off_ms = 0;
      strobe_lvl = 1;
    }
    else {
      // new_strobe = 1 - 255
      strobe_off_ms = 953 - (new_strobe * 3.7); // >0 - 18 Hz --- 10ms - 950ms
    }
  }

  void update() {
    if (strobe_off_ms == 0) {
      return;
    }

    // check to see if it's time to change the state of the strobe
    unsigned long current_ms = millis();

    if ((strobe_lvl == 1) && (current_ms - strobe_previous_ms >= strobe_on_ms)) {
      strobe_lvl = 0;  // Turn it off
      strobe_previous_ms = current_ms;  // Remember the time
    }
    else if ((strobe_lvl == 0) && (current_ms - strobe_previous_ms >= strobe_off_ms)) {
      strobe_lvl = 1;  // turn it on
      strobe_previous_ms = current_ms;   // Remember the time
    }
  }
} Strobe;

struct {
  unsigned short bpm;
  unsigned long beat_start = 0;
  unsigned long beat_offset = 0;
  unsigned long beat_len;

  void set(byte new_speed) {
    if (new_speed == bpm) return;

    bpm = new_speed;
    beat_len = 60000L / bpm;
  }

  float getCurrBeat(unsigned int num_beats) {
    unsigned long full_len = num_beats * beat_len;
    unsigned long time_ago = (millis() - beat_start) % full_len;

    return (float)time_ago / beat_len;
  }

  unsigned long timeToNextFrame(unsigned int beat, unsigned long offset) {
    unsigned long full_len = beat * beat_len;
    unsigned long time_ago = (millis() - beat_start - (offset * beat_len)) % full_len;

    return full_len - time_ago;
  }
} Speed;

struct AnimationObject {
  unsigned long id;
  struct AnimationObject *next;
  void update() {
    
  }
};

struct {
  AnimationObject *list;
  unsigned long inc = 0;

  void add() {
    struct AnimationObject *item = malloc(sizeof(struct AnimationObject));
    item->id = inc++;
  }

  void update() {
    AnimationObject *curr = list;
    while (curr != nullptr) {
      curr->update();
      curr = curr->next;
    }
  }
} Animation;

// effects declarations

float lerp_f (float a, float b, float t) {
  return a + (b - a) * t;
}

float mod_f (float a, float m) {
  while (a >= m) {
    a -= m;
  }
  return a;
}

void readDipSwitch() {
  DMX_START = 0;
  for (int i = num_dip_pins - 1; i >= 0; i++) {
    DMX_START <<= 1;
    DMX_START += digitalRead(dip_pins[i]);
  }
}

struct LightClass {
  float lvl = 1.0f;
  unsigned long duration_ms, start_ms;
  float from_lvl, to_lvl;
  bool active_fade = false;

  void fade(unsigned long new_duration_ms, float new_to_lvl) {
    if (new_to_lvl == lvl) {
      active_fade = false;
      return;
    }

    from_lvl = lvl;
    to_lvl = new_to_lvl;
    start_ms = millis();
    duration_ms = new_duration_ms;
    active_fade = true;
  }

  void update() {
    if (!active_fade) {
      return;
    }

    float time = (float)(millis() - start_ms) / duration_ms;
    lvl = lerp_f(from_lvl, to_lvl, time);
  }

  void set(float new_lvl) {
    lvl = new_lvl;
    active_fade = false;
  }
};

struct EffectClass { // Effect
  LightClass effect_light_lvls[num_lights];
  int   effect_type                   = 0;
  byte  effect_speed                  = 120;

  void set(byte new_effect_val);
  void update();
} Effect;

struct EffectBaseClass {
  void blank() {
    for (int i = 0; i < num_lights; i++) {
      Effect.effect_light_lvls[i].set(0.0f);
    }
  }
  virtual void start(){};
  virtual void update(){};
  virtual void stop(){};
};

struct : EffectBaseClass { // EffectAllOn
  void start() override {
    for (int i = 0; i < num_lights; i++) {
      Effect.effect_light_lvls[i].set(1.0f);
    }
  }
} EffectAllOn;

struct : EffectBaseClass { // EffectFadeOutRandom
  unsigned int rand_dimmer_shuffled[num_lights];
  float last_beat = 0.0f;

  void shuffle() {
    unsigned int t, j, i;
    for (i = 0; i < num_lights - 1; i++) {
      j = random(0, num_lights - i);

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
    float beat = Speed.getCurrBeat(num_lights);
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

    for (int i = 0; i < num_lights; i++) {
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
  for (int i = 0; i < num_lights; i++) {
    effect_light_lvls[i].update();
  }
}

// programme life-cycle

void setDefaults() {
  Dimmer.set(DIMMER_DEFL);
  RandomDimmer.set(RAND_DIMMER_DEFL);
  Strobe.set(STROBE_DEFL);
  Color.set(RED_DEFL, GREEN_DEFL, BLUE_DEFL);
  Effect.set(EFFECT_DEFL);
  Speed.set(EFFECT_SPEED_DEFL);
}

void setup() {
  SoftPWMBegin(SOFTPWM_NORMAL);
 
  DMXSerial.init(DMXProbe, 2);                  // initialize DMX receiver in manual mode  // configures RS485 for receive mode
  DMXSerial.maxChannel(DMX_START + DMX_LENGTH - 1); // limit the number of DMX channels to read in

  for (int i = 0; i < num_pins; i++) {
    pinMode(led_pins[i], OUTPUT);
  }
  for (int i = 0; i < num_dip_pins; i++) {
    pinMode(dip_pins[i], INPUT);
  }

  RandomDimmer.init();

  setDefaults();
}

void loop() {
  // wait 16ms for an incomming DMX packet, then continue updating => at least 60 Hz refresh rate of animations
  if (DMXSerial.receive(16)) {

    readDipSwitch();

    // read the values and feed them to the managers
    Dimmer.set(DMXSerial.read(DMX_START + DIMMER_CH));
    RandomDimmer.set(DMXSerial.read(DMX_START + RAND_DIMMER_CH));
    Strobe.set(DMXSerial.read(DMX_START + STROBE_CH));
    Color.set(DMXSerial.read(DMX_START + RED_CH), DMXSerial.read(DMX_START + GREEN_CH), DMXSerial.read(DMX_START + BLUE_CH));
    Effect.set(DMXSerial.read(DMX_START + EFFECT_CH));
    Speed.set(DMXSerial.read(DMX_START + EFFECT_SPEED_CH));
  }

  else if (DMXSerial.noDataSince() > 2000) {
    setDefaults();
  }

  Strobe.update();
  Effect.update();

  // update lights
  float brightness_all = Strobe.strobe_lvl * Dimmer.dimmer_lvl,
        brightness_single;

  for (int i = 0; i < num_lights; i++) {
    brightness_single = brightness_all * RandomDimmer.rand_dimmer_lvls[i] * Effect.effect_light_lvls[i].lvl;

    pwmWrite(led_pins[i * 3 + 0], brightness_single * Color.red_lvl);
    pwmWrite(led_pins[i * 3 + 1], brightness_single * Color.gre_lvl);
    pwmWrite(led_pins[i * 3 + 2], brightness_single * Color.blu_lvl);
  }
}

// Fin.
