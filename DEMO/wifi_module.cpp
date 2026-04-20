#include <Arduino.h>
#include <WiFi.h>
#include "config.h"
#include "wifi_module.h"

void wifi_init()
{
    WiFi.mode(WIFI_STA);
    WiFi.setAutoReconnect(true);
    WiFi.persistent(false);
    WiFi.setSleep(false);
}

void wifi_connect()
{
    Serial.println("Connecting to WiFi...");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    unsigned long start = millis();

    while (WiFi.status() != WL_CONNECTED && millis() - start < 8000)
    {
        delay(500);
        Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.println();
        Serial.println("WiFi Connected");
        Serial.println(WiFi.localIP());
    }
    else
    {
        Serial.println();
        Serial.println("WiFi failed, will retry...");
    }
}

bool is_wifi_connected()
{
    return WiFi.status() == WL_CONNECTED;
}

int get_wifi_strength()
{
    if (!is_wifi_connected()) return 0;

    int rssi = WiFi.RSSI();

    if (rssi >= -50) return 4;
    else if (rssi >= -60) return 3;
    else if (rssi >= -70) return 2;
    else if (rssi >= -80) return 1;
    else return 0;
}

void wifi_maintain()
{
    static unsigned long lastAttempt = 0;

    // Already connected → do nothing
    if (WiFi.status() == WL_CONNECTED)
        return;

    // Retry every 3 seconds
    if (millis() - lastAttempt < 3000)
        return;

    lastAttempt = millis();

    Serial.println("Reconnecting WiFi...");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}