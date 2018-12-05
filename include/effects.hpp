#ifndef OPEN_STRIP_INCLUDE_EFFECTS_HPP_
#define OPEN_STRIP_INCLUDE_EFFECTS_HPP_

#include <Arduino.h>
#include <FastLED.h>
#include "strip_state.hpp"
#include "config.hpp"

namespace open_strip_effects {

void staticColors(StripState &strip, CRGB *leds);
void doFrame(StripState &strip, CRGB *leds);
void stripes(StripState &strip, CRGB *leds);
void strobe(StripState &strip, CRGB *leds);

}
#endif //ndef OPEN_STRIP_INCLUDE_EFFECTS_HPP_
