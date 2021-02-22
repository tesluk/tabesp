void setupWiFi() {
  WiFi.begin(clientSSID, clientPass);
  DEBUG_LN("Connecting to WiFi .. "); 
  DEBUG(clientSSID); DEBUG("  "); DEBUG_LN(clientPass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(2000);
    DEBUG(" .");
  }
  DEBUG_LN(" Connected to the WiFi");
}

uint8_t attemptsCount = 0;

void handleWiFi() {
  DEBUG("Checking WiFi status ");
  DEBUG_LN(WiFi.status());

  if (WiFi.status() == WL_CONNECTED) {
    attemptsCount = 0;
  } else {
    attemptsCount = attemptsCount + 1;
    DEBUG("Not connected. Attempt "); DEBUG_LN(attemptsCount);
  }

  if (attemptsCount > 25) {
    attemptsCount = 0;
    DEBUG_LN("Reconnecting to Wifi");
    setupWiFi();
  }

  // Restart if not connected
}
