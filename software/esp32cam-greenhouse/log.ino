void setupLog(fs::FS &fs)
{
  if (!isStorageAvailable) {
    return;
  }

  String path = "/" + String(LOG_FOLDER);
  if (!fs.exists(path)) {
    fs.mkdir(path);
  }
}

void logSave(fs::FS &fs)
{
  if (!isStorageAvailable) {
    return;
  }


  logSaveFolder(fs);

  String path = "/" + String(LOG_FOLDER) + "/" + getNameYM() + "/" + getNameYMD() + ".csv";

  File file = fs.open(path.c_str(), FILE_APPEND);
  if(!file){
    // TODO something
  }

  for (uint8_t idx = 0; idx < maxRecordsIndex; idx++) {
    if (sensors[idx].status.isDataStored) {
      continue;
    }

    file.print(&timeinfo, "%B %d %Y %H:%M:%S");
    file.print(",");
    file.print(bootCounter, DEC);
    file.print(",");
    file.print(sensors[idx].device.channel, DEC);
    file.print(",");
    file.print(sensors[idx].device.type, DEC);
    file.print(",");
    file.print(sensors[idx].device.num, DEC);
    file.print(",");
    file.print(sensors[idx].device.address, DEC);
    file.print(",");
    file.print(sensors[idx].status.isDataFailed ? "FAIL" : "OK");
    file.print(",");
    file.print(sensors[idx].value.value);
    file.print(",");
    file.print(sensors[idx].value.valueType, DEC);
    file.println();

    sensors[idx].status.isDataStored = true;
  }

  file.close();
}

void logSaveFolder(fs::FS &fs)
{
  if (!isStorageAvailable) {
    return;
  }

  String path = "/" + String(LOG_FOLDER) + "/" + getNameYM();
  if (!fs.exists(path)) {
    fs.mkdir(path);
  }
}
