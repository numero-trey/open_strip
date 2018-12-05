#include <Arduino.h>
#include <FastLED.h>
#include "strip_state.hpp"
#include "effects.hpp"

void StripState::parseData(uint8_t *in_data) {
  // Serial.printf("in_speed:%d,old_speed:%d\n",in_data[1], speed);
  dim        = in_data[0];
  program    = in_data[1] >> 1;
  speed      = in_data[2];
  hold       = in_data[3];
  position   = in_data[4];
  fade       = in_data[5];
  num_colors = in_data[6];
  opt_a      = in_data[7];
  opt_b      = in_data[8];

  for (size_t idx = 0; idx < 32; idx++) {
    color_map[idx].x = in_data[idx * 3 + CONTROL_CHANNELS];
    color_map[idx].y = in_data[idx * 3 + CONTROL_CHANNELS + 1];
    color_map[idx].z = in_data[idx * 3 + CONTROL_CHANNELS + 2];
  }

  // If the program has changed, reset the animation vars
  // if (in_data[1] >> 1 != program && in_data[1] >> 1 < ANIMATIONS_COUNT) {
  //   program = in_data[1] >> 1;
  //   program_dirty_ = true;
  //   initAnimationVars();
  // }
} // StripState::parseData

void StripState::initAnimationVars() {
  memset(workspace, 0, sizeof(workspace) * sizeof(workspace[0]));
  memset(uworkspace, 0, sizeof(uworkspace) * sizeof(uworkspace[0]));
  first_frame_ = true;
} // initAnimationVars()

bool StripState::isFirstFrame() {
  if (first_frame_) {
    first_frame_ = false;
    return true;
  }
  return false;
} // isFirstFrame()

CRGB StripState::getColor(uint8_t idx) {
  uint8_t calc_index = idx * _max(num_colors,1) / 255;
  return CHSV(color_map[calc_index].x, color_map[calc_index].y, color_map[calc_index].z);
} // getColor()

CRGB StripState::getColor(uint8_t idx, uint8_t colors) {
  uint8_t calc_index = idx * colors / 255;
  return CHSV(color_map[calc_index].x, color_map[calc_index].y, color_map[calc_index].z);
} // getColor()

CRGB StripState::getIndexedColor(uint8_t idx) {
  return CHSV(color_map[idx].x, color_map[idx].y, color_map[idx].z);
} // getColor()

uint8_t StripState::numColors(uint8_t min) {
  return _min(_max(num_colors, min), 32);
}
