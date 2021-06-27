void setupWiFi()
{
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

// TODO wifi skip
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print(WiFi.localIP());
  Serial.println(" WiFi connected");
}
