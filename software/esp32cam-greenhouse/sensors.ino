float sensorValueFloat = 0.0;
double sensorValueDouble = 0.0;
uint16_t sensorValueUInt16 = 0;

void autodetectSensors()
{
  i2cScan();
  initHumidity();
  //initAir(); // it makes initHumidity return wrong values
  initSoil();
}

void askSensors(bool forceAction)
{
  for (uint8_t idx = 0; idx < maxRecordsIndex; idx++) {
    // skip sensors with ready data
    if (sensors[idx].isDataReady) {
      continue;
    }

    switch(sensors[idx].type) {
      #ifdef CAMERA_ENABLED
        case SENSOR_ESP32CAM:
          cliSerial->println("SENSOR_ESP32CAM");
          askSensor_ESP32CAM(idx, forceAction);
          break;
      #endif

      case SENSOR_SD_MMC:
        cliSerial->println("SENSOR_SD_MMC");
        askSensor_SD_MMC(idx, forceAction);
        break;

      case SENSOR_HDC1080:
        cliSerial->println("SENSOR_HDC1080");
        askSensor_HDC1080(idx, forceAction);
        break;

      case SENSOR_CCS811:
        cliSerial->println("SENSOR_CCS811");
        askSensor_CCS811(idx, forceAction);
        break;

      case SENSOR_ADS1115:
        cliSerial->println("SENSOR_ADS1115");
        askSensor_ADS1115(idx, forceAction);
        break;
    }
  }
}

void askSensor_HDC1080(uint8_t idx, bool forceAction)
{
  if (sensors[idx].address != 0x0) {
    hdc1080.begin(sensors[idx].address);
  }

  switch(sensors[idx].valueType) {
    case VALUE_TYPE_HUMIDITY:
      if (sensors[idx].retry > SENSOR_RETRY) {
        askSensor_HDC1080_error_noData(idx);
        return;
      }

      sensorValueDouble = hdc1080.readHumidity();

      if (sensorValueDouble < 99.9) {
        cliSerial->print("Humidity: ");
        cliSerial->println(sensorValueDouble);
        sensors[idx].value = sensorValueDouble;
        sensors[idx].isDataReady = true;
      } else {
        sensors[idx].retry++;
      }
      break;

    case VALUE_TYPE_TEMPERATURE:
      sensorValueDouble = hdc1080.readTemperature();
      cliSerial->print("Temperature: ");
      cliSerial->println(sensorValueDouble);
      sensors[idx].value = sensorValueDouble;
      sensors[idx].isDataReady = true;
      break;

    default:
      askSensor_HDC1080_error_noData(idx);
      return;
      break;
  }
}

void askSensor_CCS811(uint8_t idx, bool forceAction)
{
  delay(100);
  // TODO what if another address?
  if(ccs811.available()) {
    if(!ccs811.readData()) {
      switch(sensors[idx].valueType) {
        case VALUE_TYPE_CO2:
          if (sensors[idx].retry > SENSOR_RETRY) {
            askSensor_CCS811_error_noData(idx);
            return;
          }

          sensorValueUInt16 = ccs811.geteCO2();
          if (sensorValueUInt16 != 400) {
            cliSerial->print("CO2: ");
            cliSerial->println(sensorValueUInt16);
            sensors[idx].value = sensorValueUInt16;
            sensors[idx].isDataReady = true;
          } else {
            sensors[idx].retry++;
          }

          break;
        case VALUE_TYPE_TVOC:
          if (sensors[idx].retry > SENSOR_RETRY) {
            askSensor_CCS811_error_noData(idx);
            return;
          }

          sensorValueUInt16 = ccs811.getTVOC();
          if (sensorValueUInt16 > 0) { 
            cliSerial->print("TVOC: ");
            cliSerial->println(sensorValueUInt16);
            sensors[idx].value = sensorValueUInt16;
            sensors[idx].isDataReady = true;
          } else {
            sensors[idx].retry++;
          }
          break;
        default:
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
  
  switch(sensors[idx].num) {
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
  cliSerial->print("Analog ");
  cliSerial->print(sensors[idx].num);
  cliSerial->print(": ");
  cliSerial->println(sensorValueFloat);

  sensors[idx].value = sensorValueFloat;
  sensors[idx].isDataReady = true;
}

bool isSensorsDataReady() {
  for (uint8_t idx = 0; idx < maxRecordsIndex; idx++) {
    if (!sensors[idx].isDataReady) {
      return false;
    }
  }

  return true;
}
