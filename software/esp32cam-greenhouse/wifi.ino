void WiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info)
{
  cliSerial->println("WiFi connected");
}

void WiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info)
{
  cliSerial->print("WiFi IP: ");
  cliSerial->println(WiFi.localIP());
  isWiFiConnected = true;
  setupTime();
}


void setupWiFi()
{
  cliSerial->println("Start WiFi");

  WiFi.onEvent(WiFiStationConnected, SYSTEM_EVENT_STA_CONNECTED);
  WiFi.onEvent(WiFiGotIP, SYSTEM_EVENT_STA_GOT_IP);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  cliSerial->println("Wait WiFi connection...");
  isWiFiConnected = false;
}
