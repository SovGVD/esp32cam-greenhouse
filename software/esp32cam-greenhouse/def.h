// define the number of bytes you want to access
#define EEPROM_SIZE 1

// Pin definition for CAMERA_MODEL_AI_THINKER
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */

#define SENSOR_ID_MAX 4

#define LOG_FOLDER "logs"
#define PIC_FOLDER "pics"

// Sensors:
// 0x40 HDC1080          Humidity/Temperature
// 0x5A CCS811           Air Quality
// 0x48 ADS1115/ADS1015  ADC (soil sensors)

#define CAM_WB_ADJUST_FRAMES 4
#define SENSOR_RETRY 5

#define SENSORS_NORMAL false
#define SENSORS_FORCE  true

#define SENSOR_ESP32CAM 0
#define SENSOR_SD_MMC   1
#define SENSOR_POWER    2 // TODO
#define SENSOR_HDC1080  3
#define SENSOR_CCS811   4
#define SENSOR_ADS1115  5

#define VALUE_TYPE_RAW           0
#define VALUE_TYPE_STORAGE_USED  1
#define VALUE_TYPE_STORAGE_TOTAL 2
#define VALUE_TYPE_POWER_V       3  // TODO
#define VALUE_TYPE_POWER_A       4  // TODO
#define VALUE_TYPE_JPEG          5
#define VALUE_TYPE_TEMPERATURE   6
#define VALUE_TYPE_HUMIDITY      7
#define VALUE_TYPE_CO2           8
#define VALUE_TYPE_TVOC          9

typedef struct {
  uint8_t channel;  // For TCA, 0 - without TCA
  uint8_t type;
  uint8_t address;
  uint8_t num;
  uint8_t valueType;
  double value;
  bool isDataStored;
  bool isDataReady;
  bool isDataFailed;
  uint8_t retry;
} recordStructure;
