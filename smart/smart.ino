#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "Audio.h"

// 🎧 I2S pins
#define I2S_BCLK  16
#define I2S_LRC   17
#define I2S_DOUT  18

// 📶 WiFi
const char* ssid = "OPPO";
const char* password = "Vihashini@18";

// 🌐 Server
String device_id = "ATAB-000000000006";
String server = "http://10.163.11.104:3000";

Audio audio;

void setup()
{
    Serial.begin(115200);

    // 🔌 WiFi connect
    WiFi.begin(ssid, password);
    Serial.print("Connecting WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\n✅ WiFi Connected");

    // 🎧 Audio setup
    audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
    audio.setVolume(16);
    audio.forceMono(true);

    delay(1000);

    // 🔥 GET NOTIFICATIONS
    String notif_url = server + "/v1/devices/" + device_id + "/notifications";

    HTTPClient http;
    http.begin(notif_url);

    int code = http.GET();

    if (code == 200)
    {
        String payload = http.getString();

        Serial.println("\n====== FULL JSON ======");
        Serial.println(payload);
        Serial.println("======================\n");

        // 🔥 Parse JSON
        DynamicJsonDocument doc(4096);
        DeserializationError error = deserializeJson(doc, payload);

        if (error) {
            Serial.print("❌ JSON Parse Error: ");
            Serial.println(error.c_str());
            return;
        }

        // 🔥 LOOP THROUGH NOTIFICATIONS
        JsonArray notifications = doc["notifications"];

        String message_id = "";

        for (JsonObject notif : notifications)
        {
            if (notif.containsKey("audio") && notif["audio"].containsKey("standard_message_id"))
            {
                message_id = notif["audio"]["standard_message_id"].as<String>();
                Serial.print("Found Message ID: ");
                Serial.println(message_id);
                break; // play first valid
            }
        }

        // ❌ If no audio found
        if (message_id == "" || message_id == "null") {
            Serial.println("⚠️ No audio notification found");
            return;
        }

        // 🔥 PLAY AUDIO
        String audio_url = server + "/v1/devices/" + device_id + "/audio/" + message_id;

        Serial.println("🔊 Playing MP3...");
        Serial.println(audio_url);

        audio.connecttohost(audio_url.c_str());
    }
    else
    {
        Serial.print("❌ Notification API failed: ");
        Serial.println(code);
    }

    http.end();
}

void loop()
{
    audio.loop();
}