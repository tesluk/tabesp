void setupMqtt() {
  mqtt.onConnect(onMqttConnect);
  mqtt.onMessage(onMqttMessage);
  mqtt.setServer(mqttServer, mqttPort);
  mqtt.setKeepAlive(60);

  char topicBuf[50];
  strcpy(topicBuf, mqttTopic);
  strcat(topicBuf, "/LWT");
  mqtt.setWill(topicBuf, 1, true, "OFFLINE");

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
  subscribe("/color/+/set");
  subscribe("/relay/+/set");
  subscribe("/pin/+/set");

  subscribe("/gcolor/set");
  subscribe("/grelay/+/set");
  subscribe("/gpin/+/set");

  strcpy(topicBuf, mqttTopic);
  strcat(topicBuf, "/config");
  char configBuf[200];
  buildConfigBody(configBuf);
  mqtt.publish(topicBuf, 0, true, configBuf);

  strcpy(topicBuf, mqttTopic);
  strcat(topicBuf, "/info");
  char infoBuf[100];
  buildInfoBody(infoBuf);
  mqtt.publish(topicBuf, 0, true, infoBuf);

  strcpy(topicBuf, mqttTopic);
  strcat(topicBuf, "/LWT");
  mqtt.publish(topicBuf, 1, true, "ONLINE");
}

void subscribe(char* topic) {
  char topicBuf[50];
  strcpy(topicBuf, mqttTopic);
  strcat(topicBuf, topic);
  uint16_t packetIdSub = mqtt.subscribe(topicBuf, 1);
  DEBUG("Subscribed ");  DEBUG_LN(topicBuf);
}


void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
  DEBUG("  topic: ");  DEBUG_LN(topic);

  DEBUG_LN(properties.qos);

  char lPayload[7];
  memcpy(lPayload, payload, len);

  lPayload[len] = 0;

  if (len == 0) {
    if (strstr(topic, "/config/restart")) {
      restart();
    }
    if (strstr(topic, "/config/reset")) {
      reset();
    }
    return;
  }

  // PCA

  // /color/X/set   0-F (0-15)
  if (strstr(topic, "/color")) {
    int pinN = hexToInt(topic[strlen(mqttTopic) + 7]);
    DEBUG_LN(pinN);

    RGB colorTmp = colorFromHex(lPayload);
    setColorTarget(pinN, colorTmp);
    return;
  }

  // /relay/X/set   0-F (0-15)
  if (strstr(topic, "/relay")) {
    int pinN = hexToInt(topic[strlen(mqttTopic) + 7]);
    DEBUG_LN(pinN);

    bool val = true;
    if ((len == 1 && lPayload[0] == '0') || (len == 3 && lPayload[0] == 'O' && lPayload[1] == 'F' && lPayload[2] == 'F')) {
      val = false;
    }

    setRelayTarget(pinN, val);
    return;
  }

  // /pin/X/set   0-F (0-15)
  if (strstr(topic, "/pin")) {
    int pinN = hexToInt(topic[strlen(mqttTopic) + 5]);
    DEBUG_LN(pinN);

    uint16_t val = strtoul(lPayload, NULL, 10);
    setPWMTarget(pinN, val);
    return;
  }


  // GPIO

  // /gcolor/set
  if (strstr(topic, "/gcolor")) {
    RGB colorTmp = colorFromHex(lPayload);
    setGColorTarget(colorTmp);
    return;
  }

  // /grelay/X/set   0-2
  if (strstr(topic, "/grelay")) {
    int pinN = hexToInt(topic[strlen(mqttTopic) + 7]);
    DEBUG_LN(pinN);

    bool val = true;
    if ((len == 1 && lPayload[0] == '0') || (len == 3 && lPayload[0] == 'O' && lPayload[1] == 'F' && lPayload[2] == 'F')) {
      val = false;
    }

    setGRelayTarget(pinN, val);
    return;
  }

  // /gpin/X/set   0-2
  if (strstr(topic, "/gpin")) {
    int pinN = hexToInt(topic[strlen(mqttTopic) + 6]);
    DEBUG_LN(pinN);

    uint16_t val = strtoul(lPayload, NULL, 10);
    setGpioTarget(pinN, val);
    return;
  }

}
