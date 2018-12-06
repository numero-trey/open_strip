#ifndef OPEN_STRIP_INCLUDE_MAIN_HPP_
#define OPEN_STRIP_INCLUDE_MAIN_HPP_

#include <Arduino.h>

void onDmxFrame(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t* data);
void doSerial();
void parseFrame(uint8_t* data, uint16_t length, uint8_t start_strip);
void onSerialFrame(const uint8_t* buffer, size_t size);

#endif //ndef OPEN_STRIP_INCLUDE_MAIN_HPP_
