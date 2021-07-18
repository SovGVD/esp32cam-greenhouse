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

bool isDeviceDataReady(uint8_t idx)
{
  for (uint8_t i = 0; i < maxRecordsIndex; i++) {
    if (!sensors[i].status.isDataReady
     && sensors[i].device.channel == sensors[idx].device.channel
     && sensors[i].device.type == sensors[idx].device.type
     && sensors[i].device.address == sensors[idx].device.address) {
      return false;
    }
  }

  return true;
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

bool isSensorInSkip(uint8_t idx)
{
  return sensors[idx].settings.skip > 0
      && sensors[idx].settings.skip >= sensors[idx].settingsState.skip;
}

void sensorRetry(uint8_t idx)
{
  sensors[idx].settingsState.retry++;
}

bool isSensorInRetry(uint8_t idx)
{
  return sensors[idx].settings.retry > 0
      && sensors[idx].settings.retry >= sensors[idx].settingsState.retry;
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
  if (isSensorInSkip(idx)) {
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

/**
 * Skip sensor reading if:
 *  - not continuous
 *  - data ready
 */
bool skipSensorOnAskSensors(uint8_t idx)
{
  if (sensors[idx].settings.average == 0) {
    return false;
  }

  return sensors[idx].status.isDataReady;
}


void printSensor(uint8_t idx)
{
  cliSerial->print("[");
  cliSerial->print(idx);
  cliSerial->print("]");
  cliSerial->print(" device: {channel: ");
  cliSerial->print(sensors[idx].device.channel);
  cliSerial->print(", type: ");
  cliSerial->print(printDevice_type(sensors[idx].device.type));
  cliSerial->print(", address: ");
  cliSerial->print(sensors[idx].device.address);
  cliSerial->print(", num: ");
  cliSerial->print(sensors[idx].device.num);
  cliSerial->print("}, value: {value: ");
  cliSerial->print(sensors[idx].value.value);
  cliSerial->print(", valueType: ");
  cliSerial->print(printDevice_valueType(sensors[idx].value.valueType));
  cliSerial->print("}, status: { isDataStored: ");
  cliSerial->print(printDevice_bool(sensors[idx].status.isDataStored));
  cliSerial->print(", isDataReady: ");
  cliSerial->print(printDevice_bool(sensors[idx].status.isDataReady));
  cliSerial->print(", isDataFailed: ");
  cliSerial->print(printDevice_bool(sensors[idx].status.isDataFailed));
  cliSerial->print(", isPrepareMode: ");
  cliSerial->print(printDevice_bool(sensors[idx].status.isPrepareMode));
  cliSerial->print("}, settings: {retry: ");
  cliSerial->print(sensors[idx].settings.retry);
  cliSerial->print(", skip: ");
  cliSerial->print(sensors[idx].settings.skip);
  cliSerial->print(", average: ");
  cliSerial->print(sensors[idx].settings.average);
  cliSerial->print(", storeMax: ");
  cliSerial->print(printDevice_bool(sensors[idx].settings.storeMax));
  cliSerial->print("}, settingsState: {retry: ");
  cliSerial->print(sensors[idx].settingsState.retry);
  cliSerial->print(", skip: ");
  cliSerial->print(sensors[idx].settingsState.skip);
  cliSerial->print(", average: ");
  cliSerial->print(sensors[idx].settingsState.average);
  cliSerial->print(", storeMax: ");
  cliSerial->print(printDevice_bool(sensors[idx].settingsState.storeMax));
  cliSerial->print("}, [");

  cliSerial->print(isSensorInSkip(idx) ? " skip " : "  do  ");
  cliSerial->print(isSensorDataReady(idx) ? " DA " : " -- ");
  cliSerial->print(sensors[idx].status.isDataFailed ? " DataFAIL " : "  DataOK  ");
  cliSerial->print(sensors[idx].settings.retry > 0 ? (isSensorRetryFailed(idx) ? " RF ": " RP ") : " RN "); // Retry: Fail, Progress, No (no retry)

  cliSerial->println("]");

}

String printDevice_bool(bool v)
{
  return v ? "true" : "false";
}

String printDevice_type(uint8_t t)
{
  switch(t){
    case SENSOR_ESP32CAM:
      return "ESP32CAM";
    case SENSOR_SD_MMC:
      return "SD_MMC";
    case SENSOR_INA219:
      return "INA219";
    case SENSOR_HDC1080:
      return "HDC1080";
    case SENSOR_CCS811:
      return "CCS811";
    case SENSOR_ADS1115:
      return "ADS1115";
    default:
      return "UNKNOWN_DEVICE " + String(t);
  }
}

String printDevice_valueType(uint8_t t)
{
  switch(t) {
    case VALUE_TYPE_RAW:
      return "RAW";
    case VALUE_TYPE_STORAGE_USED:
      return "STORAGE_USED";
    case VALUE_TYPE_STORAGE_TOTAL:
      return "STORAGE_TOTAL";
    case VALUE_TYPE_POWER_V:
      return "POWER_V";
    case VALUE_TYPE_POWER_A:
      return "POWER_A";
    case VALUE_TYPE_JPEG:
      return "JPEG";
    case VALUE_TYPE_TEMPERATURE:
      return "TEMPERATURE";
    case VALUE_TYPE_HUMIDITY:
      return "HUMIDITY";
    case VALUE_TYPE_ECO2:
      return "eCO2";
    case VALUE_TYPE_TVOC:
      return "TVOC";
    default:
      return "UNKNOWN_VALUE_TYPE " + String(t);

  }
}
