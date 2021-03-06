#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <EEPROM.h>

#include "src/dependencies/async-mqtt-client/AsyncMqttClient.h"

#include "structure.h"

////// PWD + Buttons
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
//MQTTClient mqtt;

//WiFi CONFIG (all these can be changed via web UI, no need to set them here)
char clientSSID[33] = "ssid";
char clientPass[65] = "pass";

char mqttServer[65] = "192.168.50.210";                     //both domains and IPs should work (no SSL)
uint16_t mqttPort = 1883;
char mqttUser[65] = "";                       //optional: username for MQTT auth
char mqttPass[33] = "";                       //optional: password for MQTT auth
char mqttTopic[65] = "esp/pwd/5";

#define VERSION "0.8.2"


////// PWD + Buttons
//CRGB color = CRGB::Black;

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

unsigned long loopTime = 0;
unsigned long checkPeriod = 9999;

unsigned long stepTime = 0;
unsigned long stepPeriod = 10;

void loop() {

  if (millis() - loopTime > checkPeriod) {
    handleWiFi();
    handleMqtt();
    loopTime = millis();
  }

  if (millis() - stepTime > stepPeriod) {
    stepTime = millis();
    handlePWD();
    handleGpio();
  }
  
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
