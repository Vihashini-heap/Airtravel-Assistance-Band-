#ifndef AUDIO_MODULE_H
#define AUDIO_MODULE_H

#include <Arduino.h>

void audio_init();
void audio_play(String message_id);
void audio_loop();
#endif