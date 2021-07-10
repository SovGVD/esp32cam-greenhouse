String getNameYM()
{
  return String(timeinfo.tm_year + 1900) + "-" + String(timeinfo.tm_mon + 1);
}

String getNameYMD()
{
  return String(timeinfo.tm_year + 1900) + "-" + String(timeinfo.tm_mon + 1) + "-" + String(timeinfo.tm_mday);
}

String getNameYMDHIS()
{
  return String(timeinfo.tm_year + 1900) + "-" + String(timeinfo.tm_mon + 1) + "-" + String(timeinfo.tm_mday) + "-" 
    + String(timeinfo.tm_hour) + "-" + String(timeinfo.tm_min) + "-" + String(timeinfo.tm_sec);
}
