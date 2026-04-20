#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_DRV2605.h>
#include "config.h"

TwoWire I2C_HAPTIC = TwoWire(1);
Adafruit_DRV2605 drv;

void haptic_init()
{
    pinMode(DRV_EN, OUTPUT);
    digitalWrite(DRV_EN, HIGH);
    delay(20);

    I2C_HAPTIC.begin(HAPTIC_SDA, HAPTIC_SCL);
    delay(20);

    if (!drv.begin(&I2C_HAPTIC))
    {
        Serial.println("DRV2605 error");
        while (1);
    }

    drv.useERM();
    drv.selectLibrary(1);
    drv.setMode(DRV2605_MODE_INTTRIG);
}

void play_effect(uint8_t effect, int duration)
{
    drv.stop();
    delay(50);

    drv.setWaveform(0, effect);
    drv.setWaveform(1, 0);
    drv.go();

    delay(duration);

    drv.stop();
    delay(80);
}

void haptic_trigger(String pattern, String priority)
{
    drv.setMode(DRV2605_MODE_INTTRIG);

    if (pattern.length() > 0)
    {
        int start = 0;

        while (true)
        {
            int idx = pattern.indexOf('_', start);

            String token;

            if (idx == -1)
                token = pattern.substring(start);
            else
                token = pattern.substring(start, idx);

            token.trim();
            token.toLowerCase();

            if (token == "long" || token == "high")
                play_effect(47, 600);
            else if (token == "medium")
                play_effect(47, 450);
            else
                play_effect(47, 300);

            delay(150);

            if (idx == -1) break;

            start = idx + 1;
        }

        return;
    }

    priority.toLowerCase();

    if (priority == "high")
    {
        for (int i = 0; i < 3; i++)
        {
            play_effect(47, 600);
            delay(150);
        }
    }
    else if (priority == "medium")
    {
        for (int i = 0; i < 2; i++)
        {
            play_effect(47, 450);
            delay(150);
        }
    }
    else
    {
        play_effect(47, 300);
    }
}