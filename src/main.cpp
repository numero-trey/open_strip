#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <math.h>
#include "BluetoothSerial.h"

#include <FastLED.h>
#include <ArtnetWifi.h>
#include <PacketSerial.h>
#include <MQTT.h>

#include "main.hpp"
#include "config.hpp"
#include "strip_state.hpp"
#include "effects.hpp"

// TODO: Rename to secrets.inc
#include "wifi_config.inc"
// wifi_config.inc should contain the following:
//    const char* kWifiSSID     = "your-ssid";
//    const char* kWifiPassword = "your-password";

using namespace std;

ArtnetWifi artnetnode;
PacketSerial myPacketSerial;
WiFiClient net_client;
MQTTClient mqtt_client(256);

// BluetoothSerial SerialBT;

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
  // SerialBT.begin("OpenStrip");
  // Serial.println("Bluetooth started...");

  // myPacketSerial.setStream(&SerialBT);
  // myPacketSerial.setPacketHandler(&onSerialFrame);

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
  strips[1].num_leds = 20;

  FastLED.show();

  artnetnode.begin();
  artnetnode.setArtDmxCallback(onDmxFrame);

  mqtt_client.begin(kMqttServer, net_client);
  mqtt_client.onMessageAdvanced(onMqttFrame);
  reconnectMqtt();

}

void loop() {
  //Serial.print(".");
  artnetnode.read();
  // myPacketSerial.update();

  EVERY_N_MILLIS(1000 / FRAME_RATE) {
    // Update enimations
    for (size_t idx = 0; idx < NUM_STRIPS; idx++) {
      open_strip_effects::doFrame(strips[idx], leds[idx]);
    }
    // open_strip_effects::doFrame(strips[0], leds[0]);

    FastLED.show();
  }

  EVERY_N_SECONDS(2) {
    Serial.printf("\ntime:%lu,packets_sec:%d\n", millis() / 1000, received_packets / 2);
    received_packets = 0;
  }

  mqtt_client.loop();

  EVERY_N_MILLIS(100) {
    if (!mqtt_client.connected()) {
      reconnectMqtt();
    }
  }

} // void loop()

void onDmxFrame(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t* data) {
  if (universe == 0) {
    // if (length > LEDS_PER_STRIP * 3) length = LEDS_PER_STRIP * 3;
    // for (uint8_t i = 0; i < length / 3; i++) {
    //   leds[0][i] = CRGB(data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
    // }
    parseFrame(data, length, 0);
  }
}

void onSerialFrame(const uint8_t* buffer, size_t size) {
  Serial.printf("rcv:%d\n", size);
  uint8_t parse_buff[512];
  memcpy(parse_buff, buffer, _min(512, size));
  parseFrame(parse_buff, size, 0);
}

void onMqttFrame(MQTTClient *client, char topic[], char payload[], int payload_length) {
  Serial.printf("MQTT topic: \"%s\"\n", topic);
  uint8_t parse_buff[256];
  memcpy(parse_buff, payload, _min(256, payload_length));
  parseFrame(parse_buff, _min(256, payload_length), 0);
}

void parseFrame(uint8_t *data, uint16_t length, uint8_t start_strip) {
  uint8_t packet_buffer[CHANNELS_PER_STRIP], sent_strips;

  received_packets++;

  sent_strips = ceil((float)length / CHANNELS_PER_STRIP);
  if (sent_strips > 4) sent_strips = 4;

  //Serial.printf("len: %lu, send: %d\n", length, sent_strips);

  for (uint8_t idx = 0; idx < sent_strips; idx++) {
    //Serial.printf("proc strip: %d\n", idx);
    // Figure out how much data we have to copy, then blit it into a zeroed buffer.
    uint16_t channels_to_copy = _min(CHANNELS_PER_STRIP, length - (idx * CHANNELS_PER_STRIP));
    // Serial.printf("\ni: %d, ch: %u", idx, channels_to_copy);
    memset(packet_buffer, 0, sizeof(packet_buffer));
    //Serial.print(" memset");
    memcpy(packet_buffer, data + idx * CHANNELS_PER_STRIP, channels_to_copy);
    //Serial.print(" cpy");
    strips[start_strip + idx].parseData(packet_buffer);
  }
}

void reconnectMqtt() {
  char client_id[24];
  char sub_topic[42];
  // Loop until we're reconnected
  while (!mqtt_client.connected()) {
    // Create a random client ID
    sprintf(client_id, "OpenStrip-%08X", (uint32_t)ESP.getEfuseMac());
    sprintf(sub_topic, "openStrip/dev/%s", client_id);
    // Attempt to connect
    Serial.printf("Attempting MQTT connection as %s...", client_id);
    if (mqtt_client.connect(client_id)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      mqtt_client.publish("openStrip/Announce", client_id);
      // ... and resubscribe
      Serial.printf("Subscribing to \"%s\"\n", sub_topic);
      mqtt_client.subscribe(sub_topic);
      // mqtt_client.subscribe("/openStrip/#");
    } else {
      Serial.print("failed, rc=");
      // Serial.print(mqtt_client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
