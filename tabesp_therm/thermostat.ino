float tMin = -50;
float tMax = 150;

void setupThermostat() {

  sensors.begin();

  pinMode(aPin, OUTPUT);
  pinMode(bPin, OUTPUT);
  pinMode(cPin, OUTPUT);

  digitalWrite(aPin, aRelayState);
  digitalWrite(bPin, bRelayState);
  digitalWrite(cPin, cRelayState);
}

void updateRelay(char relayCode, uint8_t xPin, bool newState) {
  setRelay(xPin, newState);
}

void setRelay(uint8_t pin, bool state) {
  if (state) {
    digitalWrite(pin, HIGH);
  } else {
    digitalWrite(pin, LOW);
  }
}

void handleThermostat() {
  sensors.requestTemperatures();
  numberOfDevices = sensors.getDeviceCount();

  for (int i = 0; i < numberOfDevices; i++) {
    // Search the wire for address
    if (sensors.getAddress(tempDeviceAddress, i)) {
      float tempC = sensors.getTempC(tempDeviceAddress);
      char addrBuf[17];
      parseAddress(tempDeviceAddress, addrBuf);
      DEBUG("Temp: "); DEBUG(addrBuf); DEBUG(" C: "); DEBUG_LN(tempC);

      // update MQTT topic
      char topicBuf[50];
      strcpy(topicBuf, mqttTopic);
      strcat(topicBuf, "/temp");
      char tBuf[15];
      char msgBuf[200];
      dtostrf(tempC, 7, 3, tBuf);
      itoa(tempC, tBuf, 10);
      strcpy(msgBuf, "Temp: "); strcat(msgBuf, tBuf);
      mqtt.publish(topicBuf, 0, true, msgBuf);

      DEBUG("Compare "); DEBUG(aSensorAddr); DEBUG(" and "); DEBUG_LN(addrBuf);
      if (strstr(addrBuf, aSensorAddr)) {
        hadleRelay('A', aPin, aMode, aRelayState, aTTarget, aTRange, tempC);
      }
      if (strstr(addrBuf, bSensorAddr)) {
        hadleRelay('B', bPin, bMode, bRelayState, bTTarget, bTRange, tempC);
      }
    }
  }
}

void hadleRelay(char relayCode, uint8_t xPin, char xMode, bool xRelayState, int xTTarget, int xTRange, float currTemp ) {
  if (xMode == 'T' && !xRelayState) {
    updateRelay(relayCode, xPin, true);
  }
  if (xMode == 'F' && xRelayState) {
    updateRelay(relayCode, xPin, false);
  }
  if (xMode == 'A') {
    DEBUG("Im here");
    if ((currTemp < xTTarget - xTRange)) {
      updateRelay(relayCode, xPin, true);
    }
    if ((currTemp > xTTarget + xTRange)) {
      updateRelay(relayCode, xPin, false);
    }
//    if ((currTemp < xTTarget - xTRange) && !xRelayState) {
//      updateRelay(relayCode, xPin, true);
//    }
//    if ((currTemp > xTTarget + xTRange) && xRelayState) {
//      updateRelay(relayCode, xPin, false);
//    }
  }
}

void parseAddress(DeviceAddress deviceAddress, char* res) {
  char buf[3];
  strcpy(res, "");
  for (uint8_t i = 0; i < 8; i++) {
    if (deviceAddress[i] < 16) strcat(res, "0");
    itoa(deviceAddress[i], buf , HEX);
    strcat(res, buf);
  }
}

// function to print a device address
void printAddress(DeviceAddress deviceAddress) {
  for (uint8_t i = 0; i < 8; i++) {
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}
