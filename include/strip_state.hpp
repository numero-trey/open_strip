#ifndef OPEN_STRIP_INCLUDE_STRIP_STATE_HPP_
#define OPEN_STRIP_INCLUDE_STRIP_STATE_HPP_

#include <Arduino.h>
#include <FastLED.h>
#include "config.hpp"

class StripState {
  struct StripColor { uint8_t x; uint8_t y; uint8_t z; };
  bool dim_dirty_, program_dirty_, first_frame_;
  uint16_t _scaleSpeed(uint16_t max, uint8_t input);

public:
  const uint8_t kAddressableHeads = 32;
  const uint8_t kPacketSize = kAddressableHeads * 3 + CONTROL_CHANNELS;
  uint8_t dim, program, speed, hold, position, fade, num_colors, opt_a, opt_b;
  struct StripColor color_map[32];
  uint8_t num_leds = LEDS_PER_STRIP;

  // Animation routine variables
  // To store their data between frames
  long workspace[8];
  uint16_t uworkspace[8];

  // Parse raw incomming data and update the strip state
  void parseData(uint8_t *in_data);

  // Initialize state vars for animation routines
  void initAnimationVars();

  // Is this the first frame of the current animation, resetting the flag.
  bool isFirstFrame();

  // Grab color by index
  CRGB getColor(uint8_t index);
  CRGB getColor(uint8_t idx, uint8_t colors);
  CRGB getIndexedColor(uint8_t idx);

  uint8_t numColors(uint8_t min);

  uint16_t scaledSpeed(uint16_t max) { return _scaleSpeed(max, speed); }
  uint16_t scaledSpeedInv(uint16_t max) { return _scaleSpeed(max, 255 - speed); }

}; // class StripPacket

#endif //ndef OPEN_STRIP_INCLUDE_STRIP_STATE_HPP_
