void setupSleep()
{
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
}

void doSleep()
{
  cliSerial->println("Going to sleep now");
  esp_deep_sleep_start();
}
