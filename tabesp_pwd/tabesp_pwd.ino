#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include "src/dependencies/async-mqtt-client/AsyncMqttClient.h"

////// PWD + Buttons
#include <FastLED.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
//////

#define WLED_DEBUG
#ifdef WLED_DEBUG
#define DEBUG(x)  Serial.print (x)
#define DEBUG_LN(x) Serial.println (x)
#define DEBUG_PRINTF(x) Serial.printf (x)
#else
#define DEBUG(x)
#define DEBUG_LN(x)
#define DEBUG_PRINTF(x)
#endif

AsyncMqttClient mqtt;

//WiFi CONFIG (all these can be changed via web UI, no need to set them here)
char clientSSID[33] = "AdminNetwork";
char clientPass[65] = "basata2216";

char mqttServer[65] = "192.168.1.202";                     //both domains and IPs should work (no SSL)
uint16_t mqttPort = 1883;
char mqttUser[65] = "";                       //optional: username for MQTT auth
char mqttPass[33] = "";                       //optional: password for MQTT auth
char mqttTopic[65] = "esp/pwd/3";

#define VERSION "0.5"

// 12 13 14 - rgb
// 4 5
// 2 - WLED, on board led?
// 15 - could not start


////// PWD + Buttons
CRGB color = CRGB::Black;
bool colorState = false;

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
//////

//boot starts here
void setup() {
  Serial.begin(115200); // 9600 / 115200
  DEBUG_LN("Boot");

  setupConfig();
  setupPWD();
  setupWiFi();
  setupMqtt();

  DEBUG_LN("Boot DONE");
}

unsigned long debugTime = 0;

//main program loop
void loop() {

  yield();
}

void restart() {
  DEBUG_LN("Restart");
  ESP.restart();
}

void reset() {
  DEBUG_LN("Factory reset");
  clearEEPROM();
  restart();
}
