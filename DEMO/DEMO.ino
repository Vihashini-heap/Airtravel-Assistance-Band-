#include <Arduino.h>
#include <WiFi.h>

#include "config.h"
#include "wifi_module.h"
#include "api.h"
#include "display.h"
#include "haptic.h"
#include "speaker.h"
#include "rtos_common.h"

// -------- QUEUES --------
QueueHandle_t qDisplay;
QueueHandle_t qHaptic;
QueueHandle_t qAudio;

// -------- STATE --------
bool registered = false;
String lastNotif = "";

// -------- WIFI TASK --------
void wifi_task(void *pv)
{
    wifi_init();

    display_wifi_connecting();
    wifi_connect();

    while (1)
    {
        if (!is_wifi_connected())
        {
            display_wifi_disconnected();
        }

        wifi_maintain();
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

// -------- API TASK --------
void api_task(void *pv)
{
    while (1)
    {
        if (!is_wifi_connected())
        {
            registered = false;
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            continue;
        }

        int status = api_check_connectivity();

        if (status != SERVER_CONNECTED)
        {
            display_server_error(get_wifi_strength());
            vTaskDelay(2000 / portTICK_PERIOD_MS);
            continue;
        }

        // REGISTER
        if (!registered)
        {
            display_registering();
            registered = api_register_device();

            vTaskDelay(1000 / portTICK_PERIOD_MS);
            continue;
        }

        // PASSENGER SCREEN (DEFAULT)
        display_passenger(passenger_name, flight_number, seat_number, gate, get_wifi_strength(), true);

        // NOTIFICATION
        if (api_get_notifications())
        {
            String current = notif_title + notif_text;

            if (current != lastNotif)
            {
                lastNotif = current;

                NotificationMessage msg;
                msg.title    = notif_title;
                msg.text     = notif_text;
                msg.priority = notif_priority;
                msg.pattern  = notif_pattern;
                msg.audio_id = notif_audio_id;

                xQueueSend(qDisplay, &msg, 0);
                xQueueSend(qHaptic,  &msg, 0);
                xQueueSend(qAudio,   &msg, 0);

                Serial.println("NEW NOTIFICATION");
            }
        }

        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}

// -------- DISPLAY TASK --------
void display_task(void *pv)
{
    display_init();

    NotificationMessage msg;

    while (1)
    {
        if (xQueueReceive(qDisplay, &msg, 3000))
        {
            display_notification(msg.title, msg.text, get_wifi_strength(), true);
        }
        else
        {
            // fallback → passenger screen
            display_passenger(passenger_name, flight_number, seat_number, gate, get_wifi_strength(), true);
        }
    }
}

// -------- HAPTIC TASK --------
void haptic_task(void *pv)
{
    haptic_init();

    NotificationMessage msg;

    while (1)
    {
        if (xQueueReceive(qHaptic, &msg, portMAX_DELAY))
        {
            haptic_trigger(msg.pattern, msg.priority);
        }
    }
}

// -------- AUDIO TASK --------
void audio_task(void *pv)
{
    audio_init();

    NotificationMessage msg;

    while (1)
    {
        if (xQueueReceive(qAudio, &msg, portMAX_DELAY))
        {
            audio_play(msg.audio_id);
        }

        audio_loop();  // IMPORTANT
    }
}

void setup()
{
    Serial.begin(115200);

    qDisplay = xQueueCreate(5, sizeof(NotificationMessage));
    qHaptic  = xQueueCreate(5, sizeof(NotificationMessage));
    qAudio   = xQueueCreate(5, sizeof(NotificationMessage));

    xTaskCreate(wifi_task,   "WiFi",   4096, NULL, 2, NULL);
    xTaskCreate(api_task,    "API",    6144, NULL, 2, NULL);
    xTaskCreate(display_task,"Display",4096, NULL, 1, NULL);
    xTaskCreate(haptic_task, "Haptic", 4096, NULL, 1, NULL);
    xTaskCreate(audio_task,  "Audio",  6144, NULL, 2, NULL);
}

void loop() {}