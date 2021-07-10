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
  sensors[idx].value = 0.0;
  sensors[idx].isDataReady  = true;
  sensors[idx].isDataFailed = true;
}
