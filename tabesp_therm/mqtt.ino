void setupMqtt() {

  DEBUG("Connecting to mqtt on "); DEBUG_LN(mqttServer);

  mqtt.onConnect(onMqttConnect);
  mqtt.onMessage(onMqttMessage);
  mqtt.setServer(mqttServer, mqttPort);
  mqtt.setKeepAlive(60);
  mqtt.connect();
}

void handleMqtt() {
  if (mqtt.connected()) return;

  DEBUG_LN("Reconnecting to MQTT");
  setupMqtt();
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
}

void mqttPostSensor(char* id, float value) {
  char topicBuf[50];
  strcpy(topicBuf, mqttTopic);
  strcat(topicBuf, "/temp/");
  strcat(topicBuf, id);

  char tBuf[15];
  dtostrf(value, 2, 2, tBuf);
  mqtt.publish(topicBuf, 0, true, tBuf);
}
