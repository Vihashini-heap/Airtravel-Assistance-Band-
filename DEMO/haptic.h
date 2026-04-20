#ifndef HAPTIC_H
#define HAPTIC_H

#include <Arduino.h>
void haptic_init();
void haptic_trigger(String pattern, String priority);

#endif