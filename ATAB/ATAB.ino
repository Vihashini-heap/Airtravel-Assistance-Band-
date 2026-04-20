#include <Arduino.h>
#include <WiFi.h>

#include "config.h"
#include "wifi_module.h"
#include "api.h"
#include "display.h"
#include "haptic.h"
#include "speaker.h"

// -------- STATE FLAGS --------
bool registered = false;
bool passengerShown = false;
bool wifiConnectedOnce = false;

// -------- NOTIFICATION --------
bool notifActive = false;
unsigned long notifStart = 0;
const unsigned long NOTIF_DURATION = 30000;

String lastNotif = "";

// -------- TIMING --------
unsigned long lastPoll = 0;
const unsigned long POLL_INTERVAL = 2000;

void setup()
{
    Serial.begin(115200);

    wifi_init();
    display_init();
    haptic_init();
    audio_init();

    Serial.println("SYSTEM START");

    display_wifi_connecting();
    Serial.println("Connecting to WiFi...");
    wifi_connect();
}

void loop()
{
    // =========================
    // WIFI PRIORITY (STRICT)
    // =========================
    if (WiFi.status() != WL_CONNECTED)
    {
        if (wifiConnectedOnce)
            Serial.println("WiFi LOST");

        registered = false;
        passengerShown = false;
        wifiConnectedOnce = false;
        notifActive = false;

        display_wifi_disconnected();

        wifi_maintain();
        delay(200);
        return;
    }

    int bars = get_wifi_strength();

    // =========================
    // WIFI CONNECTED (ONLY ONCE)
    // =========================
    if (!wifiConnectedOnce)
    {
        Serial.println("WiFi Connected");
        Serial.print("IP: ");
        Serial.println(WiFi.localIP());

        display_wifi_connected();
        delay(800);

        wifiConnectedOnce = true;
    }

    // =========================
    // SERVER CHECK
    // =========================
    if (millis() - lastPoll >= POLL_INTERVAL)
    {
        lastPoll = millis();

         if (WiFi.status() != WL_CONNECTED)
     {
         display_wifi_disconnected();
         return;
     }
        int status = api_check_connectivity();

        // re-check WiFi after API
        if (WiFi.status() != WL_CONNECTED)
        {
            display_wifi_disconnected();
            return;
        }

        if (status != SERVER_CONNECTED)
        {
            Serial.println("SERVER ERROR");
            display_server_error(bars);
            return;
        }

        // =========================
        // REGISTER
        // =========================
        if (!registered)
        {
            Serial.println("Registering device...");

            display_registering();
            delay(800);

            registered = api_register_device();

            if (registered)
            {
                Serial.println("REGISTER SUCCESS");

                Serial.println("----- PASSENGER INFO -----");
                Serial.print("Name: "); Serial.println(passenger_name);
                Serial.print("Flight: "); Serial.println(flight_number);
                Serial.print("Seat: "); Serial.println(seat_number);
                Serial.print("Gate: "); Serial.println(gate);
                Serial.println("--------------------------");
            }
            else
            {
                Serial.println("REGISTER FAILED");
                return;
            }

            passengerShown = false;
        }

        // =========================
        // PASSENGER DISPLAY
        // =========================
        if (!passengerShown)
        {
            Serial.println("Displaying Passenger Info");

            display_passenger(passenger_name, flight_number, seat_number, gate, bars, true);

            delay(2500);

            passengerShown = true;

            display_sleep();
            Serial.println("Idle Mode");
        }

        // =========================
        // FETCH NOTIFICATION
        // =========================
        if (!notifActive && api_get_notifications())
        {
            String currentNotif = notif_title + notif_text;

            if (currentNotif != lastNotif)
            {
                lastNotif = currentNotif;

                Serial.println("----- NEW NOTIFICATION -----");
                Serial.print("Title: "); Serial.println(notif_title);
                Serial.print("Text : "); Serial.println(notif_text);
                Serial.print("Priority: "); Serial.println(notif_priority);
                Serial.println("----------------------------");

                display_notification(notif_title, notif_text, bars, true);
                haptic_trigger(notif_pattern, notif_priority);
                audio_play(notif_audio_id);
                notifStart = millis();
                notifActive = true;
            }
        }
    }

    // =========================
    // NOTIFICATION TIMER
    // =========================
    if (notifActive && millis() - notifStart >= NOTIF_DURATION)
    {
        Serial.println("Notification End → Idle");

        notifActive = false;
        display_sleep();
    }
    audio_loop();
    delay(10);
}