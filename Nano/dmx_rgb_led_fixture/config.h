#define NUM_LIGHTS        6
#define RGB

#ifndef RGB
#define RGBW
#endif

// DEFAULTS

#define DIMMER_DEFL       255
#define RAND_DIMMER_DEFL  255

#ifdef RGB
#define RED_DEFL          255
#define GREEN_DEFL        255
#define BLUE_DEFL         255
#else
#ifdef RGBW
#define RED_DEFL          0
#define GREEN_DEFL        0
#define BLUE_DEFL         0
#define WHITE_DEFL        255
#endif
#endif

#define STROBE_DEFL       0
#define EFFECT_DEFL       0
#define EFFECT_SPEED_DEFL 120
