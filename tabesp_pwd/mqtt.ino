void setupMqtt() {

  DEBUG("Connecting to mqtt on "); DEBUG_LN(mqttServer);

  mqtt.onConnect(onMqttConnect);
  mqtt.onMessage(onMqttMessage);
  mqtt.setServer(mqttServer, mqttPort);
  mqtt.setKeepAlive(60);
  mqtt.connect();
}

void handleMqtt(){
  if(mqtt.connected()) return;

  DEBUG_LN("Reconnecting to MQTT");
  setupMqtt();
}

void onMqttConnect(bool sessionPresent) {
  DEBUG_LN("Connected to MQTT.");
  DEBUG("Session present: "); DEBUG_LN(sessionPresent);

  char topicBuf[50];
  strcpy(topicBuf, mqttTopic);
  strcat(topicBuf, "/color/+/set");
  uint16_t packetIdSub = mqtt.subscribe(topicBuf, 2);
  DEBUG("Subscribed ");  DEBUG_LN(packetIdSub);

  strcpy(topicBuf, mqttTopic);
  strcat(topicBuf, "/pin/+/set");
  packetIdSub = mqtt.subscribe(topicBuf, 2);

//  strcpy(topicBuf, mqttTopic);
//  strcat(topicBuf, "/config/#");
//  packetIdSub = mqtt.subscribe(topicBuf, 2);

  strcpy(topicBuf, mqttTopic);
  strcat(topicBuf, "/config");
  char configBuf[200];
  buildConfigBody(configBuf);
  mqtt.publish(topicBuf, 0, true, configBuf);

  strcpy(topicBuf, mqttTopic);
  strcat(topicBuf, "/info");
  mqtt.publish(topicBuf, 0, true, VERSION);
}

void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
  DEBUG_LN("Publish received.");
  DEBUG("  topic: ");  DEBUG_LN(topic);
  DEBUG("  qos: ");  DEBUG(properties.qos); DEBUG("  dup: ");  DEBUG(properties.dup); DEBUG("  retain: ");  DEBUG_LN(properties.retain);
  DEBUG("  len: "); DEBUG(len); DEBUG("  index: ");  DEBUG(index);  DEBUG("  total: ");  DEBUG_LN(total);


  if (len == 0) {
    if (strstr(topic, "/config/restart")) {
      restart();
    }
    if (strstr(topic, "/config/reset")) {
      reset();
    }
    return;
  }

  payload[len] = 0;
  DEBUG_LN(payload);
  // PIN // /pin/X/set   0-F (0-15)
  if (strstr(topic, "/pin") && strstr(topic, "/set")) {
    char* tmp = topic + strlen(mqttTopic) + 5;
    //    char* num = strtok(tmp, "/") + 4;
    tmp[1] = 0;
    DEBUG_LN(tmp);
    int pinN = strtol(tmp, NULL, 16);
    uint16_t val = strtoul(payload, NULL, 10);

    setPWM(pinN, val);
  }

  // RGB LED // /color/X/set
  if (strstr(topic, "/color") && strstr(topic, "/set")) {
    char* tmp = topic + strlen(mqttTopic) + 7;
    tmp[1] = 0;
    DEBUG_LN(tmp);
    int pinN = strtol(tmp, NULL, 16);

    CRGB colorTmp = colorFromHex(payload);
    setColor(pinN, colorTmp);
  }

  // CONFIG
  //  if (strstr(topic, "/config/get")) {
  //    strcpy(topicBuf, mqttTopic);
  //    strcat(topicBuf, "/config/msg");
  //    mqtt.publish(topicBuf, 0, true, testMsg);
  //  }



}
