#ifndef CONFIG_H
#define CONFIG_H

// -------- WIFI --------
#define WIFI_SSID "OPPO"
#define WIFI_PASSWORD "Vihashini@18"

// -------- SERVER --------
#define BASE_URL "http://10.163.11.104:3000/v1"
#define DEVICE_ID "ATAB-000000000001"

// -------- OLED (I2C) --------
#define OLED_SDA 4
#define OLED_SCL 5

// -------- HAPTIC (I2C) --------
#define HAPTIC_SDA 8
#define HAPTIC_SCL 9
#define DRV_EN 7

// -------- AUDIO (I2S) --------
#define I2S_BCLK  38
#define I2S_LRC   39
#define I2S_DOUT  40
#define AMP_SD 41
#endif