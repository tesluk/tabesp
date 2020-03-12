void setupWiFi(){
  WiFi.begin(clientSSID, clientPass);
  DEBUG("Connecting to WiFi .. "); DEBUG(clientSSID);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    DEBUG(" .");
  }
  DEBUG_LN(" Connected to the WiFi");
}
