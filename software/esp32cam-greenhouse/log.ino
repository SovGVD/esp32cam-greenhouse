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
  if(!getLocalTime(&timeinfo)){
    // TODO failback if it does not work
  }

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
    if (sensors[idx].isDataStored) {
      continue;
    }

    file.print(&timeinfo, "%A, %B %d %Y %H:%M:%S");
    file.print(",");

    file.print(sensors[idx].channel, DEC);
    file.print(",");
    file.print(sensors[idx].type, DEC);
    file.print(",");
    file.print(sensors[idx].num, DEC);
    file.print(",");
    file.print(sensors[idx].address, DEC);
    file.print(",");
    file.print(sensors[idx].isDataFailed ? "FAIL" : "OK");
    file.print(",");
    file.print(sensors[idx].value);
    file.print(",");
    file.print(sensors[idx].valueType, DEC);
    file.println();

    sensors[idx].isDataStored = true;
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
