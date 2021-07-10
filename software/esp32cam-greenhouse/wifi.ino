void WiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info)
{
  isWiFiConnected = true;
  cliSerial->print(WiFi.localIP());
  cliSerial->println(" WiFi connected");
  setupTime();
}

void setupWiFi()
{
  cliSerial->println("Start WiFi");
  WiFi.onEvent(WiFiStationConnected, SYSTEM_EVENT_STA_CONNECTED);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  cliSerial->println("Wait WiFi connection...");
  isWiFiConnected = false;
}
