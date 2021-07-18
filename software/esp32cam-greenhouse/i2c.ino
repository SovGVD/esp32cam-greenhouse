void i2cScan()
{
  cliSerial->print("I2C SCAN: [");
  for (byte i = 8; i < 120; i++)
  {
    Wire.beginTransmission (i);        // Begin I2C transmission Address (i)
    if (Wire.endTransmission () == 0)  // Receive 0 = success (ACK response) 
    {
      cliSerial->print(i, DEC);
      cliSerial->print(" (0x");
      cliSerial->print(i, HEX);
      cliSerial->print("), ");
    }
  }
  cliSerial->println("]");
}

void initPower()
{
  if (ina219.init()) {
    ina219.setADCMode(SAMPLE_MODE_16);
    ina219.setMeasureMode(CONTINUOUS);
    ina219.setPGain(PG_320);
    ina219.setBusRange(BRNG_16);

    appendSensor(0, SENSOR_INA219, 0x0, VALUE_TYPE_POWER_V);
    appendSensor(0, SENSOR_INA219, 0x0, VALUE_TYPE_POWER_A);
    cliSerial->println("INA219 power sensor detected");
  } else {
    cliSerial->println("INA219 - sensor is not available");
  }
}

void initHumidity()
{
  hdc1080.begin(0x40);
  if (hdc1080.readDeviceId()) {
    appendSensor(0, SENSOR_HDC1080, 0x0, VALUE_TYPE_TEMPERATURE); // Temperature
    appendSensor(0, SENSOR_HDC1080, 0x0, VALUE_TYPE_HUMIDITY);    // Humidity, set address to 0x0 to use previusly requested sensor as it is just one sensor
    cliSerial->println("HDC1080 Humidity and Temperature sensor detected");
  } else {
    cliSerial->println("HDC1080 - sensor is not available");
  }
}


void initAir()
{
  if (bootCounter % 5) {
    return;
  }

  // my css811 on the same board with hda1080 and looks like it is heating it so temperature and humidity return wrong data =(
  if (ccs811.begin()) {
    while(!ccs811.available()) {   // TODO not great, try to move it into main loop
      delay(10);
    }
    ccs811.setDriveMode(CCS811_DRIVE_MODE_IDLE);

    appendSensor(0, SENSOR_CCS811, 0x0, VALUE_TYPE_ECO2);
    appendSensor(0, SENSOR_CCS811, 0x0, VALUE_TYPE_TVOC);
    cliSerial->println("CCS811 air quality sensor detected");
  } else {
    cliSerial->println("CCS811 - sensor is not available");
  }
}

void initSoil()
{
  //cliSerial->println("Try ADC1115");
  if (ads1115.init()) {
    // Setup sensor
    ads1115.setVoltageRange_mV(ADS1115_RANGE_2048);
    ads1115.setCompareChannels(ADS1115_COMP_0_GND);
    ads1115.setAlertPinMode(ADS1115_DISABLE_ALERT);
    ads1115.setConvRate(ADS1115_128_SPS);
    ads1115.setMeasureMode(ADS1115_CONTINUOUS);

    // 4 channels
    appendSensor(0, SENSOR_ADS1115, 0x0, VALUE_TYPE_RAW);
    appendSensor(0, SENSOR_ADS1115, 0x0, VALUE_TYPE_RAW);
    appendSensor(0, SENSOR_ADS1115, 0x0, VALUE_TYPE_RAW);
    appendSensor(0, SENSOR_ADS1115, 0x0, VALUE_TYPE_RAW);
    cliSerial->println("ADC1115 based soil moisture sensor detected");
  } else {
    cliSerial->println("ADC1115 - sensor is not available");
  }
}

void appendSensor(uint8_t sensorChannel, uint8_t sensorType, uint8_t sensorAddress, uint8_t sensorValueType)
{
  if (currentRecordIndex == 255) {
    // TODO some error here, but probably we will never have more than 255 sensors per device
    return;
  }
  uint8_t sensorNum = 0;

  for (uint8_t idx = 0; idx < maxRecordsIndex; idx++)
  {
    // increment sensor number
    if (sensors[idx].device.channel == sensorChannel && sensors[idx].device.type == sensorType && sensors[idx].device.address == sensorAddress) {
      sensorNum = sensors[idx].device.num + 1;
    }
  }

  sensors[currentRecordIndex] = {
      {
        sensorChannel,
        sensorType,
        sensorAddress,
        sensorNum
      },
      {
        0.0,
        sensorValueType,
      },
      {
        false,
        false,
        false,
        false
      },
      defaultSettings[sensorType],                    // Settings
      {0, 0, 0, defaultSettings[sensorType].storeMax} // Settings state
    };

  cliSerial->print("INIT: ");
  printSensor(currentRecordIndex);

  currentRecordIndex++;
  maxRecordsIndex = currentRecordIndex;
}
