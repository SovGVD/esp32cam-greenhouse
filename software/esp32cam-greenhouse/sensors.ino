float sensorValueFloat = 0.0;
double sensorValueDouble = 0.0;
uint16_t sensorValueUInt16 = 0;

void autodetectSensors()
{
  i2cScan();
  initPower();
  initHumidity();
  initAir();
  initSoil();
}

void askSensors(bool forceAction)
{
  for (uint8_t idx = 0; idx < maxRecordsIndex; idx++) {

    // skip sensors with ready data
    if (skipSensorOnAskSensors(idx)) {
      continue;
    }

    switch(sensors[idx].device.type) {
      #ifdef CAMERA_ENABLED
        case SENSOR_ESP32CAM:
          askSensor_ESP32CAM(idx, forceAction);
          break;
      #endif

      case SENSOR_SD_MMC:
        askSensor_SD_MMC(idx, forceAction);
        break;

      case SENSOR_INA219:
        askSensor_INA219(idx, forceAction);
        break;

      case SENSOR_HDC1080:
        askSensor_HDC1080(idx, forceAction);
        break;

      case SENSOR_CCS811:
        askSensor_CCS811(idx, forceAction);
        break;

      case SENSOR_ADS1115:
        askSensor_ADS1115(idx, forceAction);
        break;
    }
  }
}

void askSensor_INA219(uint8_t idx, bool forceAction)
{
  // TODO what if another address?

  if (!ina219.getOverflow()) {
    switch(sensors[idx].value.valueType) {
      case VALUE_TYPE_POWER_V:
        setSensorValue(idx, ina219.getBusVoltage_V() + ina219.getShuntVoltage_mV() / 1000);
        break;

      case VALUE_TYPE_POWER_A:
        setSensorValue(idx, ina219.getCurrent_mA() / 1000);
        break;

      default:
        askSensor_INA219_error_noData(idx);
        return;
    }
  } else {
    sensorRetry(idx);
  }
}

void askSensor_HDC1080(uint8_t idx, bool forceAction)
{
  if (sensors[idx].device.address != 0x0) {
    hdc1080.begin(sensors[idx].device.address);
  }

  switch(sensors[idx].value.valueType) {
    case VALUE_TYPE_HUMIDITY:
      if (isSensorRetryFailed(idx)) {
        askSensor_HDC1080_error_noData(idx);
        return;
      }

      sensorValueDouble = hdc1080.readHumidity();

      if (sensorValueDouble < 99.9) {
        setSensorValue(idx, sensorValueDouble);
      } else {
        sensorRetry(idx);
      }
      break;

    case VALUE_TYPE_TEMPERATURE:
      sensorValueDouble = hdc1080.readTemperature();
      if (sensorValueDouble != 125.0) {
        setSensorValue(idx, sensorValueDouble);
      } else {
        sensorRetry(idx);
      }
      break;

    default:
      askSensor_HDC1080_error_noData(idx);
      return;
  }
}

void askSensor_CCS811(uint8_t idx, bool forceAction)
{
  // CCS811 a little bit complicated and I don't think it is working correct
  float temperatureCCS811 = getDataForCCS811(idx, VALUE_TYPE_TEMPERATURE);
  float humidityCCS811    = getDataForCCS811(idx, VALUE_TYPE_HUMIDITY);

  if (temperatureCCS811 == SENSOR_ERROR_VALUE || humidityCCS811 == SENSOR_ERROR_VALUE) {
    return;
  }

  if (isDeviceDataReady(idx)) {
    ccs811.setDriveMode(CCS811_DRIVE_MODE_IDLE);
    sensors[idx].status.isPrepareMode = false;
    return;
  }

  // TODO what if another address?

  if(ccs811.available()) {
    if (!sensors[idx].status.isPrepareMode) {
      ccs811.setEnvironmentalData(humidityCCS811, temperatureCCS811);
      sensors[idx].status.isPrepareMode = true;
      ccs811.setDriveMode(CCS811_DRIVE_MODE_1SEC);
      delay(1000);
    }

    delay(200);
    if(!ccs811.readData()) {
      switch(sensors[idx].value.valueType) {
        case VALUE_TYPE_ECO2:
          if (isSensorRetryFailed(idx)) {
            askSensor_CCS811_error_noData(idx);
            return;
          }

          sensorValueUInt16 = ccs811.geteCO2();
          if (sensorValueUInt16 != 400 && sensorValueUInt16 != 0) {
            setSensorValue(idx, sensorValueUInt16);
          } else {
            sensorRetry(idx);
          }

          break;

        case VALUE_TYPE_TVOC:
          if (isSensorRetryFailed(idx)) {
            askSensor_CCS811_error_noData(idx);
            return;
          }

          sensorValueUInt16 = ccs811.getTVOC();
          if (sensorValueUInt16 > 0) { 
            setSensorValue(idx, sensorValueUInt16);
          } else {
            sensorRetry(idx);
          }
          break;

        default:
          ccs811.setDriveMode(CCS811_DRIVE_MODE_IDLE);
          sensors[idx].status.isPrepareMode = false;
          askSensor_CCS811_error_noData(idx);
          return;
          break;
      }
    }
  }  
}

void askSensor_ADS1115(uint8_t idx, bool forceAction)
{
  // TODO  what if another address?

  ADS1115_MUX channel = ADS1115_COMP_0_GND;
  
  switch(sensors[idx].device.num) {
    case 0:
      channel = ADS1115_COMP_0_GND;
      break;
    case 1:
      channel = ADS1115_COMP_1_GND;
      break;
    case 2:
      channel = ADS1115_COMP_2_GND;
      break;
    case 3:
      channel = ADS1115_COMP_3_GND;
      break;
    default:
      askSensor_ADS1115_error_noData(idx);
      return;
      break;
  }
  
  ads1115.setCompareChannels(channel);
  delay(10);
  sensorValueFloat = ads1115.getResult_mV();

  setSensorValue(idx, sensorValueFloat);
}
