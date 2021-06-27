void setupLog(fs::FS &fs)
{
  if (!isStorageAvailable) {
    return;
  }

  String path = "/logs";
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

  String path = "/logs/" + String(timeinfo.tm_year + 1900) + "-" + String(timeinfo.tm_mon + 1) + "/" 
    + String(timeinfo.tm_year + 1900) + "-" + String(timeinfo.tm_mon + 1) + "-" + String(timeinfo.tm_mday) + ".csv";

  File file = fs.open(path.c_str(), FILE_APPEND);
  if(!file){
    // TODO something
  }

  file.print(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  file.print(",");

  file.println();

  file.close();
}

void logSaveFolder(fs::FS &fs)
{
  if (!isStorageAvailable) {
    return;
  }

  String path = "/logs/" + String(timeinfo.tm_year + 1900) + "-" + String(timeinfo.tm_mon + 1);
  if (!fs.exists(path)) {
    fs.mkdir(path);
  }
  
}
