// TODO what about another address?

void disableSensors()
{
  for (uint8_t idx = 0; idx < maxRecordsIndex; idx++) {
        switch(sensors[idx].device.type) {
      #ifdef CAMERA_ENABLED
        case SENSOR_ESP32CAM:
          disableSensor_ESP32CAM(idx);
          break;
      #endif

      case SENSOR_SD_MMC:
        disableSensor_SD_MMC(idx);
        break;

      case SENSOR_INA219:
        disableSensor_INA219(idx);
        break;

      case SENSOR_HDC1080:
        disableSensor_HDC1080(idx);
        break;

      case SENSOR_CCS811:
        disableSensor_CCS811(idx);
        break;

      case SENSOR_ADS1115:
        disableSensor_ADS1115(idx);
        break;
    }
  }
}

void disableSensor_INA219(uint8_t idx)
{
  ina219.setMeasureMode(POWER_DOWN);
}

void disableSensor_HDC1080(uint8_t idx)
{
  // IDK
}

void disableSensor_CCS811(uint8_t idx)
{
  
  ccs811.setDriveMode(CCS811_DRIVE_MODE_IDLE);
}

void disableSensor_ADS1115(uint8_t idx)
{
  ads1115.setMeasureMode(ADS1115_SINGLE);
}
