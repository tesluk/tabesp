void setupMqtt() {

  DEBUG("Connecting to mqtt on "); DEBUG_LN(mqttServer);

  mqtt.onConnect(onMqttConnect);
  mqtt.onMessage(onMqttMessage);
  mqtt.setServer(mqttServer, mqttPort);
  mqtt.connect();

}

void onMqttConnect(bool sessionPresent) {
  DEBUG_LN("Connected to MQTT.");
  DEBUG("Session present: "); DEBUG_LN(sessionPresent);

  char topicBuf[50];
  strcpy(topicBuf, mqttTopic);
  strcat(topicBuf, "/color/set");
  uint16_t packetIdSub = mqtt.subscribe(topicBuf, 2);
  DEBUG("Subscribed ");  DEBUG_LN(packetIdSub);

//  strcpy(topicBuf, mqttTopic);
//  strcat(topicBuf, "/config/msg");
//  mqtt.publish(topicBuf, 0, true, testMsg);

  strcpy(topicBuf, mqttTopic);
  strcat(topicBuf, "/config/msg/get");
  packetIdSub = mqtt.subscribe(topicBuf, 2);

  strcpy(topicBuf, mqttTopic);
  strcat(topicBuf, "/config/msg/set");
  packetIdSub = mqtt.subscribe(topicBuf, 2);

  strcpy(topicBuf, mqttTopic);
  strcat(topicBuf, "/relay/set");
  packetIdSub = mqtt.subscribe(topicBuf, 2);

  strcpy(topicBuf, mqttTopic);
  strcat(topicBuf, "/info");
  mqtt.publish(topicBuf, 0, true, VERSION);
}

void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
  DEBUG_LN("Publish received.");
  DEBUG("  topic: ");  DEBUG_LN(topic);
  DEBUG("  qos: ");  DEBUG(properties.qos); DEBUG("  dup: ");  DEBUG(properties.dup); DEBUG("  retain: ");  DEBUG_LN(properties.retain);
  DEBUG("  len: "); DEBUG(len); DEBUG("  index: ");  DEBUG(index);  DEBUG("  total: ");  DEBUG_LN(total);

  payload[len] = 0;
  DEBUG_LN(payload);

  // RGB LED
  if (strstr(topic, "/color/set")) {
//    CRGB colorTmp = colorFromHex(payload);
//    color = colorTmp;
//    setColor(color);
  }

  // RELAY
  if (strstr(topic, "/relay/set")) {
    if (strstr(payload, "on") || strstr(payload, "ON") || strstr(payload, "1")) {
      aRelayState = true;
    } else {
      aRelayState = false;
    }
    setRelay(aPin, aRelayState);
  }

  // CONFIG
  char topicBuf[50];
//  if (strstr(topic, "/config/msg/get")) {
//    strcpy(topicBuf, mqttTopic);
//    strcat(topicBuf, "/config/msg");
//    mqtt.publish(topicBuf, 0, true, testMsg);
//  }

  if (strstr(topic, "/config/restart")) {
    restart();
  }

  if (strstr(topic, "/config/reset")) {
    reset();
  }

}
