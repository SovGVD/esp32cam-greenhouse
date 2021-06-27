

/**
 * TODO:
 *  - working with path based on time is not optimal!!!!
 */

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

#include "def.h"
#include "config.wifi.h"
#include "config.h"

#ifdef CAMERA_ENABLED
  camera_fb_t * fb = NULL;  
#endif

struct tm timeinfo;
bool isStorageAvailable = false;
RTC_DATA_ATTR bool isTimeSynced = false;


void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector
 
  Serial.begin(115200);
  setupStorage();
  #ifdef CAMERA_ENABLED
    setupCamera(SD_MMC);
  #endif
  setupSleep();
  setupLog(SD_MMC);

  if (!isTimeSynced) {
    setupWiFi();
    setupTime();
  }
}

void loop() {
  logSave(SD_MMC);
  #ifdef CAMERA_ENABLED
    takePhoto(SD_MMC);
  #endif
  
  Serial.println("Going to sleep now");
  esp_deep_sleep_start();
  Serial.println("This will never be printed");
  
}
