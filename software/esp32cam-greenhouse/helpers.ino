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
  if (sensors[idx].settings.skip > 0) {
    sensors[idx].settingsState.skip++;

    if (sensors[idx].settings.skip >= sensors[idx].settingsState.skip) {
      return true;
    }
  }

  return false;
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

void setSensorValue(uint8_t idx, double value)
{
  // Skip sensor values
  if (isSensorSkip(idx)) {
    return;
  }

  if (sensors[idx].settings.average == 0) {
    if (!sensors[idx].status.isDataReady) {
      sensors[idx].value.value = value;
    } else if (sensors[idx].settings.storeMax && value > sensors[idx].value.value) {
      sensors[idx].value.value = value;
    } else if (!sensors[idx].settings.storeMax && value < sensors[idx].value.value) {
      sensors[idx].value.value = value;
    }

    sensors[idx].status.isDataReady = true;
    return;
  }

  sensors[idx].value.value += value;

  // We don't need calculate average
  if (sensors[idx].settings.average == 1) {
    sensors[idx].status.isDataReady = true;
    return;
  }

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
