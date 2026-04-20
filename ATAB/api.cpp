#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#include "config.h"
#include "api.h"

// -------- GLOBAL VARIABLES --------
String passenger_name = "";
String seat_number = "";
String flight_number = "";
String gate = "";
String device_token = "";

String notif_title = "";
String notif_text = "";
String notif_priority = "";
String notif_pattern = "";
String notif_audio_id = "";


// -------- CONNECTIVITY CHECK --------
int api_check_connectivity()
{
    if (WiFi.status() != WL_CONNECTED)
        return WIFI_DISCONNECTED;

    HTTPClient http;
    String url = String(BASE_URL) + "/connectivity_check";

    http.begin(url);
    http.setTimeout(1000);

    int code = http.GET();

    // WiFi dropped during request
    if (WiFi.status() != WL_CONNECTED)
    {
        http.end();
        return WIFI_DISCONNECTED;
    }

    if (code <= 0)
    {
        http.end();
        return SERVER_DISCONNECTED;
    }

    http.end();

    if (code == 200)
        return SERVER_CONNECTED;
    else
        return SERVER_DISCONNECTED;
}


// -------- REGISTER DEVICE --------
bool api_register_device()
{
    if (WiFi.status() != WL_CONNECTED)
        return false;

    HTTPClient http;
    String url = String(BASE_URL) + "/devices/register";

    http.begin(url);
    http.setTimeout(1500);
    http.addHeader("Content-Type", "application/json");

    String payload = "{\"device_id\":\"" + String(DEVICE_ID) + "\"}";

    int code = http.POST(payload);

    if (WiFi.status() != WL_CONNECTED)
    {
        http.end();
        return false;
    }

    if (code == 201)
    {
        String res = http.getString();

        DynamicJsonDocument doc(4096);
        deserializeJson(doc, res);

        passenger_name = doc["passenger"]["first_name"].as<String>();
        seat_number    = doc["passenger"]["seat_number"].as<String>();
        flight_number  = doc["flight"]["flight_number"].as<String>();
        gate           = doc["flight"]["gate"].as<String>();
        device_token   = doc["device_token"].as<String>();

        http.end();
        return true;
    }

    http.end();
    return false;
}


// -------- GET NOTIFICATIONS --------
bool api_get_notifications()
{
    if (WiFi.status() != WL_CONNECTED)
        return false;

    HTTPClient http;

    String url = String(BASE_URL) + "/devices/" + String(DEVICE_ID) + "/notifications";

    http.begin(url);
    http.setTimeout(1000);
    http.addHeader("Authorization", "Bearer " + device_token);

    int code = http.GET();

    if (WiFi.status() != WL_CONNECTED)
    {
        http.end();
        return false;
    }

    if (code <= 0)
    {
        http.end();
        return false;
    }

    if (code == 401)
    {
        api_register_device();
        http.end();
        return false;
    }

    if (code == 200)
    {
        String res = http.getString();

        DynamicJsonDocument doc(4096);
        deserializeJson(doc, res);

        JsonArray notifications = doc["notifications"];

        if (notifications.size() > 0)
        {
            JsonObject n = notifications[0];

            notif_title    = n["title"].as<String>();
            notif_text     = n["display_text"].as<String>();
            notif_priority = n["priority"].as<String>();

            //AUDIO ID EXTRACTION
        notif_audio_id = "";

    if (n.containsKey("audio") && n["audio"].containsKey("standard_message_id"))
       {
        notif_audio_id = n["audio"]["standard_message_id"].as<String>();
        Serial.print("Audio ID: ");
        Serial.println(notif_audio_id);
       }
    else
    {
        Serial.println("No audio in notification");
    }
            //Haptic
            notif_pattern = "";
            if (n["visual_alert"].containsKey("vibration_pattern"))
                notif_pattern = n["visual_alert"]["vibration_pattern"].as<String>();

            http.end();
            return true;
        }
    }

    http.end();
    return false;
}
