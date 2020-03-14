#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include "src/dependencies/async-mqtt-client/AsyncMqttClient.h"

////// DS18b20 Thermostat + IR
#include <OneWire.h>
#include <DallasTemperature.h>
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

char mqttServer[65] = "192.168.1.200";                     //both domains and IPs should work (no SSL)
uint16_t mqttPort = 1883;
char mqttUser[65] = "";                       //optional: username for MQTT auth
char mqttPass[33] = "";                       //optional: password for MQTT auth
char mqttTopic[65] = "tabesp/therm/aaa";

#define VERSION "0.6"

// 12 13 14 - rgb
// 4 5
// 2 - WLED, on board led?
// 15 - could not start

////// Thermostat
uint8_t aPin = 12;
uint8_t bPin = 13;
uint8_t cPin = 14;

char aMode = 'A'; // On
char bMode = 'A'; // Off
char cMode = 'F'; // Auto

bool aRelayState = false;
bool bRelayState = false;
bool cRelayState = false;

int aTTarget = 25;
int bTTarget = 25;
int cTTarget = 25;

int aTRange = 1;
int bTRange = 1;
int cTRange = 1;

char aSensorAddr[33] = "be3b3b04005d";
char bSensorAddr[33] = "693a3b0400dd";
char cSensorAddr[33] = "";

#define ONE_WIRE_BUS 4 // D2
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
int numberOfDevices;
DeviceAddress tempDeviceAddress;

//boot starts here
void setup() {
  Serial.begin(115200); // 9600 - 115200
  DEBUG_LN("Boot");

  setupConfig();
  setupWiFi();
  setupMqtt();

  setupThermostat();
}

unsigned long loopTime = 0;
unsigned long checkPeriod = 9999;

void loop() {

  if (millis() - loopTime > checkPeriod) {

    handleThermostat();
    
    loopTime = millis();
  }

  //  for (uint16_t i = 0; i < 4096; i += 32) {
  //    if(i % 256 == 0){
  //      DEBUG(':');
  //    }
  //    for (uint8_t pwmnum = 0; pwmnum < 16; pwmnum++) {
  //      pwm.setPWM(pwmnum, 0, (i + (4096 / 16)*pwmnum) % 4096 );
  //    }
  //    yield();
  //  }


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
