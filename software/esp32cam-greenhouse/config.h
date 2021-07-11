#define TIME_TO_SLEEP  10 /* Time ESP32 will go to sleep (in seconds) */

#define CAMERA_ENABLED

#define NTP_SERVER "pool.ntp.org"

#define SERIAL_BAUD 115200

// Don't change that pins to any other, most pins are in use with sd card or cam or psram
#define I2C_SDA 3 
#define I2C_SCL 13

#define MAX_TIMER 30000 // milliseconds, 1 second = 1000 milliseconds
#define SENSOR_DELAY 1000
