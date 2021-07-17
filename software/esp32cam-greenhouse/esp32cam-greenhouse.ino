#include "esp_camera.h"
#include "Arduino.h"
#include "FS.h"                // SD Card ESP32
#include "SD_MMC.h"            // SD Card ESP32
#include "soc/soc.h"           // Disable brownour problems
#include "soc/rtc_cntl_reg.h"  // Disable brownour problems
#include "driver/rtc_io.h"
#include <EEPROM.h>            // read and write from flash memory
#include <WiFi.h>
#include "time.h"
#include <Wire.h>

#include "def.h"
#include "config.wifi.h"
#include "config.h"

// Sensors libraries (maybe it is bad idea to autodetect sensors)
#include "Adafruit_CCS811.h"
Adafruit_CCS811 ccs811;

#include<ADS1115_WE.h>
ADS1115_WE ads1115 = ADS1115_WE();

#include "ClosedCube_HDC1080.h"
ClosedCube_HDC1080 hdc1080;

#ifdef CAMERA_ENABLED
  camera_fb_t * fb = NULL;  
#endif

// This can't be implemented at the moment, see https://github.com/wollewald/INA219_WE/issues/6
//#include<INA219_WE.h>
//INA219_WE ina219;

struct tm timeinfo;
bool isStorageAvailable = false;

// CLI
Stream *cliSerial;

RTC_DATA_ATTR uint16_t bootCounter = 0;

// Flags
RTC_DATA_ATTR bool isTimeSynced = false;
bool isWiFiConnected = false;
bool isWaitingAllDone = true;
bool isFail = false;

// Sensors storage
uint8_t currentRecordIndex = 0;
uint8_t maxRecordsIndex = 0;
recordStructure sensors[255] = {};

// Timer
unsigned long startedAt;
unsigned long currentTime;
unsigned long previousTime;

void setup() {
  bootCounter++;  // TODO resync time after some boots, AND store bootCounter in eeprom after each time sync to have point on return on cold boot
  startedAt = millis();
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector

  Serial.begin(SERIAL_BAUD);
  cliSerial = &Serial;

  cliSerial->print("Boot No ");
  cliSerial->println(bootCounter);

  if (!isTimeSynced) {
    cliSerial->print("Time is not synced");
    setupWiFi();
  } else {
    getLocalTime(&timeinfo);
  }
  
  setupStorage();
  setupSleep();
  setupLog(SD_MMC);

  #ifdef CAMERA_ENABLED
    setupCamera(SD_MMC);
  #endif

  Wire.begin(I2C_SDA, I2C_SCL);
  autodetectSensors();
}

void loop() {
  //cliSerial->println("Loop start");
  while (isWaitingAllDone) {
    currentTime = millis();

    if (currentTime - previousTime >= SENSOR_DELAY) {
      previousTime = currentTime;
      askSensors(SENSORS_NORMAL);
    }

    // Don't wait too long
    if (currentTime - startedAt >= MAX_TIMER) {
      isFail = true;
    }

    if ((isAllSensorsDataReady() && isTimeSynced) || isFail) {
      askSensors(SENSORS_FORCE); // Final attempt for camera, storage usage and maybe other sensors
      logSave(SD_MMC);
      isWaitingAllDone = false;
    }
  }
  
  doSleep();
}
