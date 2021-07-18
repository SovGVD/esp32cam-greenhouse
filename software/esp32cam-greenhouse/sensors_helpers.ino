float getDataForCCS811(uint8_t idx, uint8_t valueType)
{
  for (uint8_t i = 0; i < maxRecordsIndex; i++) {
    if (sensors[i].status.isDataReady && !sensors[i].status.isDataFailed
     && sensors[i].device.channel == sensors[idx].device.channel
     && sensors[i].device.type == SENSOR_HDC1080
     && sensors[i].value.valueType == valueType) {
      return sensors[i].value.value;
    }
  }

  return SENSOR_ERROR_VALUE;
}

bool isAllSensorsDataReady() {
  for (uint8_t idx = 0; idx < maxRecordsIndex; idx++) {
    if (!isSensorDataReady(idx)) {
      return false;
    }
  }

  return true;
}

bool isSensorDataReady(uint8_t idx)
{
  return sensors[idx].status.isDataReady;
}

void sensorSkip(uint8_t idx)
{
  sensors[idx].settingsState.skip++;
}

bool isSensorSkip(uint8_t idx)
{
  return sensors[idx].settings.skip > 0
      && sensors[idx].settings.skip >= sensors[idx].settingsState.skip;
}

void sensorRetry(uint8_t idx)
{
  sensors[idx].settingsState.retry++;
}

bool isSensorRetryFailed(uint8_t idx)
{
  if (sensors[idx].settings.retry == 0) {
    return false;
  }

  if (sensors[idx].settingsState.retry >= sensors[idx].settings.retry) {
    return true;
  }

  return false;
}

bool isSensorContinuousReading(uint8_t idx)
{
  return sensors[idx].settings.average == 0;
}

void setSensorValue(uint8_t idx, double value)
{
  // Skip sensor values
  if (isSensorSkip(idx)) {
    sensorSkip(idx);
    return;
  }

  if (isSensorContinuousReading(idx)) {
    if (!isSensorDataReady(idx)) {
      // First reading
      sensors[idx].value.value = value;
    } else if (sensors[idx].settings.storeMax && value > sensors[idx].value.value) {
      sensors[idx].value.value = value;
    } else if (!sensors[idx].settings.storeMax && value < sensors[idx].value.value) {
      sensors[idx].value.value = value;
    }

    sensors[idx].status.isDataReady = true;
    return;
  }


  // We don't need calculate average
  if (sensors[idx].settings.average == 1) {
    sensors[idx].value.value = value;
    sensors[idx].status.isDataReady = true;
    return;
  }

  sensors[idx].value.value += value;
  sensors[idx].settingsState.average++;

  if (sensors[idx].settings.average == sensors[idx].settingsState.average) {
    sensors[idx].value.value = sensors[idx].value.value / sensors[idx].settings.average;
    sensors[idx].status.isDataReady = true;
  }
}

bool skipSensorOnAskSensors(uint8_t idx)
{
  if (sensors[idx].settings.average == 0) {
    return false;
  }

  return sensors[idx].status.isDataReady;
}
