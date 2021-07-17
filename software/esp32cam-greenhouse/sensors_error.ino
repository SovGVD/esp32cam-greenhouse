void askSensor_HDC1080_error_noData(uint8_t idx)
{
  askSensor_markAsFailed(idx);
}

void askSensor_CCS811_error_noData(uint8_t idx)
{
  askSensor_markAsFailed(idx);
}

void askSensor_ADS1115_error_noData(uint8_t idx)
{
  askSensor_markAsFailed(idx);
}

void askSensor_markAsFailed(uint8_t idx)
{
  // Save something if we already have some values
  if (sensors[idx].settings.average > 1 && sensors[idx].settingsState.average > 1) {
    sensors[idx].value.value = sensors[idx].value.value / sensors[idx].settingsState.average;
  } else {
    sensors[idx].value.value = 0.0;
  }

  sensors[idx].status.isDataReady  = true;
  sensors[idx].status.isDataFailed = true;
}
