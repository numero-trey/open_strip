#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <math.h>

#include <FastLED.h>
#include <ArtnetWifi.h>

#include "main.hpp"
#include "config.hpp"
#include "strip_state.hpp"
#include "effects.hpp"

#include "wifi_config.inc"
// wifi_config.inc should contain the following:
//    const char* kWifiSSID     = "your-ssid";
//    const char* kWifiPassword = "your-password";

using namespace std;

ArtnetWifi artnetnode;

CRGB leds[NUM_STRIPS][LEDS_PER_STRIP];
StripState strips[NUM_STRIPS];

uint16_t received_packets;

void setup() {
  Serial.begin(250000);
  Serial.println();
  Serial.println("Connecting...");

  WiFi.begin(kWifiSSID, kWifiPassword);

  // Await connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
  }

  // Connected
  Serial.println();
  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());

  // Init led strips
  FastLED.addLeds<NEOPIXEL, LED_PIN_0>(leds[0], LEDS_PER_STRIP).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<NEOPIXEL, LED_PIN_1>(leds[1], LEDS_PER_STRIP).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<NEOPIXEL, LED_PIN_2>(leds[2], LEDS_PER_STRIP).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<NEOPIXEL, LED_PIN_3>(leds[3], LEDS_PER_STRIP).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<NEOPIXEL, LED_PIN_4>(leds[4], LEDS_PER_STRIP).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<NEOPIXEL, LED_PIN_5>(leds[5], LEDS_PER_STRIP).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<NEOPIXEL, LED_PIN_6>(leds[6], LEDS_PER_STRIP).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<NEOPIXEL, LED_PIN_7>(leds[7], LEDS_PER_STRIP).setCorrection(TypicalLEDStrip);

  // TODO: load/save strip config.
  strips[0].num_leds = 64;

  FastLED.show();

  artnetnode.begin();
  artnetnode.setArtDmxCallback(onDmxFrame);

}

void loop() {
  //Serial.print(".");
  artnetnode.read();

  EVERY_N_MILLIS(1000 / FRAME_RATE) {
    // Update enimations
    // for (size_t idx = 0; idx < NUM_STRIPS; idx++) {
    //   open_strip_effects::doFrame(strips[idx], leds[idx]);
    // }
    open_strip_effects::doFrame(strips[0], leds[0]);

    FastLED.show();
  }

  EVERY_N_SECONDS(2) {
    Serial.printf("\ntime:%lu,packets_sec:%d\n", millis() / 1000, received_packets / 2);
    received_packets = 0;
  }

} // void loop()

void onDmxFrame(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t* data) {
  if (universe == 0) {
    received_packets++;
    // if (length > LEDS_PER_STRIP * 3) length = LEDS_PER_STRIP * 3;
    // for (uint8_t i = 0; i < length / 3; i++) {
    //   leds[0][i] = CRGB(data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
    // }
    parseFrame(data, length, 0);
  }
}

void parseFrame(uint8_t *data, uint16_t length, uint8_t start_strip) {
  uint8_t packet_buffer[CHANNELS_PER_STRIP], sent_strips;

  sent_strips = 1; //ceil((float)length / CHANNELS_PER_STRIP);
  if (sent_strips > 4) sent_strips = 4;

  for (uint8_t idx = 0; idx < sent_strips; idx++) {
    // Figure out how much data we have to copy, then blit it into a zeroed buffer.
    uint16_t channels_to_copy = _min(CHANNELS_PER_STRIP, length - (idx * CHANNELS_PER_STRIP));
    //Serial.printf("\ni: %d, ch: %u", idx, channels_to_copy);
    memset(packet_buffer, 0, sizeof(packet_buffer));
    //Serial.print(" memset");
    memcpy(packet_buffer, data + idx * CHANNELS_PER_STRIP, channels_to_copy);
    //Serial.print(" cpy");
    strips[start_strip + idx].parseData(packet_buffer);
  }

}
