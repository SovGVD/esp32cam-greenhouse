void setupStorage()
{
  if(!SD_MMC.begin("/sdcard", true)){
    Serial.println("SD Card Mount Failed");
    return;
  }
    
  uint8_t cardType = SD_MMC.cardType();
  if(cardType == CARD_NONE){
    Serial.println("No SD Card attached");
    isStorageAvailable = false;
    return;
  }

  isStorageAvailable = true;

  appendSensor(0, SENSOR_SD_MMC, 0x0, VALUE_TYPE_STORAGE_USED);
  appendSensor(0, SENSOR_SD_MMC, 0x0, VALUE_TYPE_STORAGE_TOTAL);
}

void askSensor_SD_MMC(uint8_t idx, bool forceAction)
{
  if (!isStorageAvailable) {
    askSensor_SD_MMC_noData(idx);
    return;
  }

  // This will return size BEFORE image save, so does not looks correct
  switch(sensors[idx].valueType) {
    case VALUE_TYPE_STORAGE_USED:
      cliSerial->print("SD_MMC used: ");
      cliSerial->println(SD_MMC.usedBytes());
      sensors[idx].value = SD_MMC.usedBytes() / (1024 * 1024);
      sensors[idx].isDataReady = true;
      break;
    case VALUE_TYPE_STORAGE_TOTAL:
      cliSerial->print("SD_MMC total: ");
      cliSerial->println(SD_MMC.totalBytes());
      cliSerial->print("SD_MMC size: ");
      cliSerial->println(SD_MMC.cardSize());
      sensors[idx].value = SD_MMC.totalBytes() / (1024 * 1024);
      sensors[idx].isDataReady = true;
      break;
    default:
      askSensor_SD_MMC_noData(idx);
      break;
  }
}

void askSensor_SD_MMC_noData(uint8_t idx)
{
  askSensor_markAsFailed(idx);
}
