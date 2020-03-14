#define EEPSIZE 2560
#define EEPVER 1

void setupConfig() {
  EEPROM.begin(EEPSIZE);

  clearEEPROM();

  int eepVer = EEPROM.read(234); // config version
  DEBUG("Config version: "); DEBUG_LN(eepVer);

  if (EEPROM.read(233) != 233) //set no first boot flag
  {
    clearEEPROM();
    EEPROM.write(233, 233);

    DEBUG_LN("First boot. Save config");
    saveClientSSID();
    saveClientPass();
    saveMqttServer();
    saveMqttUser();
    saveMqttPass();
    saveMqttTopic();
//    saveTestMsg();
  }
  // Upgrade config here
  if (eepVer < 1) {
    // save ver 1 here
  }
  EEPROM.write(377, EEPVER); // save new config version
  commit();

  DEBUG_LN("Load config");

  readClientSSID();
  readClientPass();
  readMqttServer();
  readMqttUser();
  readMqttPass();
  readMqttTopic();
//  readTestMsg();
}

void saveClientSSID() {
  writeStringToEEPROM(  0, clientSSID, 32);
}
void readClientSSID() {
  readStringFromEEPROM(  0, clientSSID, 32);
}

void saveClientPass() {
  writeStringToEEPROM( 32, clientPass, 32);
}
void readClientPass() {
  readStringFromEEPROM( 32, clientPass, 32);
}

void saveMqttServer() {
  writeStringToEEPROM( 64, mqttServer, 64);
}
void readMqttServer() {
  readStringFromEEPROM( 64, mqttServer, 64);
}

void saveMqttUser() {
  writeStringToEEPROM(128,   mqttUser, 64);
}
void readMqttUser() {
  readStringFromEEPROM(128,   mqttUser, 64);
}

void saveMqttPass() {
  writeStringToEEPROM(192,   mqttPass, 32); // 224
}
void readMqttPass() {
  readStringFromEEPROM(192,   mqttPass, 32); // 224
}

void saveMqttTopic() {
  writeStringToEEPROM(512,  mqttTopic, 64); // 576
}
void readMqttTopic() {
  readStringFromEEPROM(512,  mqttTopic, 64); // 576
}

// REMOVE me
//void saveTestMsg() {
//  writeStringToEEPROM( 300, testMsg, 100);
//  commit();
//}
//void readTestMsg() {
//  readStringFromEEPROM( 300, testMsg, 100);
//}

void commit()
{
  if (!EEPROM.commit()) {
    DEBUG_LN("ERROR: Could not commit eeprom");
  }
}

void clearEEPROM()
{
  for (int i = 0; i < EEPSIZE; i++)
  {
    EEPROM.write(i, 0);
  }
  commit();
}

void writeStringToEEPROM(uint16_t pos, char* str, uint16_t len)
{
  for (int i = 0; i < len; ++i)
  {
    EEPROM.write(pos + i, str[i]);
    if (str[i] == 0) return;
  }
}

void readStringFromEEPROM(uint16_t pos, char* str, uint16_t len)
{
  for (int i = 0; i < len; ++i)
  {
    str[i] = EEPROM.read(pos + i);
    if (str[i] == 0) return;
  }
  str[len] = 0; //make sure every string is properly terminated. str must be at least len +1 big.
}
