#pragma once
#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

typedef struct {
    String title;
    String text;
    String priority;
    String pattern;
    String audio_id;
} NotificationMessage;

extern QueueHandle_t qDisplay;
extern QueueHandle_t qHaptic;
extern QueueHandle_t qAudio;