#define TIME_TO_SLEEP  10 /* Time ESP32 will go to sleep (in seconds) */

#define CAMERA_ENABLED

#define NTP_SERVER "pool.ntp.org"

#define SERIAL_BAUD 115200

#define I2C_SDA 16 // don't 13 for i2c if SD card is not in 1-bit mode, try uart0 (e.g. 1)
#define I2C_SCL 13 // @see https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/sdio_slave.html

#define MAX_TIMER 30000 // milliseconds, 1 second = 1000 milliseconds
#define SENSOR_DELAY 1000
