#include <Arduino.h>
#include <FastLED.h>
#include <math.h>

#include "effects.hpp"
#include "strip_state.hpp"
#include "config.hpp"

namespace open_strip_effects {
// Update strip with current animation
void doFrame(StripState &strip, CRGB *leds) {
  switch (strip.program) {
    case 1:
      stripes(strip, leds);
      break;
    default:
      staticColors(strip, leds);
      break;
  }
}

// Fill strip w/static color
void staticColors(StripState &strip, CRGB *leds) {
  // strip.uworkspace[0] == frame counter
  // strip.uworkspace[1] == color index
  // Serial.printf("speed:%d", strip.speed);
  static uint16_t kMaxTimeFrames = 5 * FRAME_RATE;
  uint16_t color_time = strip.scaledSpeedInv(kMaxTimeFrames);
  uint16_t fade_time = scale16by8(color_time, strip.fade);
  int8_t prev_color_idx;
  CRGB buffer_color;

  if (strip.speed != 0 && strip.uworkspace[0] >= color_time) {
    strip.uworkspace[0] = 0; // Reset timer
    strip.uworkspace[1]++;   // Advance current color
    if (strip.uworkspace[1] >= strip.numColors(1)) strip.uworkspace[1] = 0; // Clamp color to max
  }
  // Get next color
  prev_color_idx = strip.uworkspace[1] - 1;
  if (prev_color_idx < 0) prev_color_idx = strip.numColors(1) - 1; // Clamp color to max

  // Blend colors
  if (strip.uworkspace[0] >= fade_time) {
    buffer_color = strip.getIndexedColor(strip.uworkspace[1]);
  } else {
    buffer_color = blend(strip.getIndexedColor(prev_color_idx), strip.getIndexedColor(strip.uworkspace[1]), 255 * strip.uworkspace[0] / fade_time);
  }

  fill_solid(
    leds, strip.num_leds,
    buffer_color
  );

  // Advance timer
  strip.uworkspace[0]++;
}


void stripes(StripState &strip, CRGB *leds) {
  // animated_offset == strip.workspace[0]
  // Stripe width ranges from strip.num_leds to 1
  uint8_t stripe_width = scale8(strip.num_leds - 1, 255 - strip.opt_a) + 1;
  long calc_offset = strip.position - 127 + strip.workspace[0] / 400;
  // Shift calculated offset until it is positive
  while (calc_offset < 0) calc_offset += stripe_width * strip.numColors(2);
  //Serial.printf("sw:%d,ofs:%d\n", stripe_width, calc_offset);
  //Serial.printf("p:%d,w:%d\n", strip.position, stripe_width);
  for (size_t idx = 0; idx < strip.num_leds; idx++) {
    uint16_t effective_led = idx + calc_offset;
    uint8_t color_idx = (effective_led / stripe_width) % strip.numColors(2);
    //Serial.printf("i:%d,e:%d,c:%d|", idx, effective_led, color_idx);
    leds[idx] = strip.getIndexedColor(color_idx);
  }

  // Scale up values 64..127 to 64..a lot
  int8_t calc_speed = strip.speed - 128;
    //Serial.printf("c:%d,", calc_speed);
  if (calc_speed > 64) {
    // Scale positive values
    //Serial.printf("sh:%d\n", 400 * (calc_speed - 64) / 63 + 64);
    strip.workspace[0] += 400 * (calc_speed - 64) / 63 + 64;
  } else if (calc_speed < -64) {
    // Scale negative values
    //Serial.printf("sl:%d\n", 400 * (abs(calc_speed) - 64) / 63) + 64);
    strip.workspace[0] -= 400 * (abs(calc_speed) - 64) / 63 + 64;
  } else {
    // Unscaled values
    //Serial.printf("s:%d\n", calc_speed);
    strip.workspace[0] += calc_speed;
  }


  strip.workspace[0] %= stripe_width * strip.numColors(2) * 400;
  //Serial.printf("a:%ld,w:%d\n", strip.workspace[0], stripe_width * strip.numColors(2));
  //Serial.print("\n");
}


} // namespace open_strip_effects
