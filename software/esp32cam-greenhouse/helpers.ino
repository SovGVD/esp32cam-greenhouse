float getDataForCCS811(uint8_t idx, uint8_t valueType)
{
  for (uint8_t i = 0; i < maxRecordsIndex; i++) {
    if (sensors[i].isDataReady && !sensors[i].isDataFailed
     && sensors[i].channel == sensors[idx].channel 
     && sensors[i].type == SENSOR_HDC1080
     && sensors[i].valueType == valueType) {
      return sensors[i].value;
    }
  }

  return SENSOR_ERROR_VALUE;
}
