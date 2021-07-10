void autodetectSensors()
{
  i2cScan();
  initHumidity();
  //initAir();
  initSoil();
}

void askSensors(bool forceAction)
{
  cliSerial->println("Ask sensors");
  for (uint8_t idx = 0; idx < maxRecordsIndex; idx++) {
    cliSerial->println(idx, DEC);
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
      sensors[idx].value = hdc1080.readHumidity();
      sensors[idx].isDataReady = true;
      break;

    case VALUE_TYPE_TEMPERATURE:
      sensors[idx].value = hdc1080.readTemperature();
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
  // TODO what if another address?
  if(ccs811.available()) {
    if(!ccs811.readData()) {
      switch(sensors[idx].valueType) {
        case VALUE_TYPE_CO2:
          sensors[idx].value = ccs811.geteCO2();
          sensors[idx].isDataReady = true;
          break;
        case VALUE_TYPE_TVOC:
          sensors[idx].value = ccs811.getTVOC();
          sensors[idx].isDataReady = true;
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
  sensors[idx].value = ads1115.getResult_V();
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
