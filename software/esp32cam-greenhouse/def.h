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

typedef struct {
  uint8_t channel;    // For TCA, 0 - without TCA
  uint8_t type;       // sensor type, e.g. SENSOR_SD_MMC or SENSOR_ADS1115
  uint8_t address;    // non-default i2c sensor address
  uint8_t num;        // number of sensor, e.g. HDC1080 returning two values: temperature and humidity, in that case it will be shown as 2 sensors: num=0 and num=1
} deviceStructure;

typedef struct {
  double value;
  uint8_t valueType;
} valueStructure;

typedef struct {
  bool isDataStored;  // data save on SD card via `log`
  bool isDataReady;   // data ready to be saved
  bool isDataFailed;  // failed to fetch data
  bool isPrepareMode; // sensor in prepare mode (e.g. SENSOR_CCS811 required some heatup)
} statusStructure;

typedef struct {
  uint8_t retry;      // number or retrying if sensor is not available
  uint8_t skip;       // skip readings before start saving it (e.g. SENSOR_ESP32CAM required at least 3 frames to set Auto White Balance)
  uint8_t average;    // number or readings before average value, 0 - continuous reading, 1 - one reading only
  bool storeMax;      // for average = 0, true -> save maximal value, false -> save minimal value
} settingsStructure;

typedef struct {
  deviceStructure device;
  valueStructure value;
  statusStructure status;
  settingsStructure settings;
  settingsStructure settingsState;
} recordStructure;

// Sensors:
// Default  Expected  Device           Description
//    0x40            HDC1080          Humidity/Temperature
//    0x40      0x41  INA219           Power sensor
//    0x5A            CCS811           Air Quality
//    0x48            ADS1115/ADS1015  ADC (soil sensors)

#define SENSOR_ERROR_VALUE -99999.9

#define SENSORS_NORMAL false
#define SENSORS_FORCE  true

#define SENSORS_NUM     6

#define SENSOR_ESP32CAM 0
#define SENSOR_SD_MMC   1
#define SENSOR_INA219   2
#define SENSOR_HDC1080  3
#define SENSOR_CCS811   4
#define SENSOR_ADS1115  5

#define VALUE_TYPE_RAW           0
#define VALUE_TYPE_STORAGE_USED  1
#define VALUE_TYPE_STORAGE_TOTAL 2
#define VALUE_TYPE_POWER_V       3
#define VALUE_TYPE_POWER_A       4
#define VALUE_TYPE_JPEG          5
#define VALUE_TYPE_TEMPERATURE   6
#define VALUE_TYPE_HUMIDITY      7
#define VALUE_TYPE_ECO2          8
#define VALUE_TYPE_TVOC          9

// {retry, skip, average, storeMax}
settingsStructure defaultSettings[SENSORS_NUM] = {
  {0, 5, 1, true},  // SENSOR_ESP32CAM
  {0, 0, 0, true},  // SENSOR_SD_MMC
  {0, 0, 0, true},  // SENSOR_POWER
  {5, 0, 2, true},  // SENSOR_HDC1080
  {5, 0, 2, true},  // SENSOR_CCS811
  {2, 0, 3, true}   // SENSOR_ADS1115
};
