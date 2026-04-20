#include <Arduino.h>
#include <Audio.h>
#include "config.h"
#include "speaker.h"

Audio audio;

void audio_init()
{
    audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
    audio.setVolume(150);
    audio.forceMono(true);
}

void audio_play(String message_id)
{
    if (message_id == "" || message_id == "null") return;

    String url = String(BASE_URL) + "/devices/" + String(DEVICE_ID) + "/audio/" + message_id;
    Serial.println("Playing audio");
    Serial.println(url);

    audio.connecttohost(url.c_str());
}

void audio_loop()
{
    audio.loop();
}