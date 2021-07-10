void setupTime()
{
  configTime(0, 0, NTP_SERVER);
  if(getLocalTime(&timeinfo)){
    isTimeSynced = true;
  }
}
