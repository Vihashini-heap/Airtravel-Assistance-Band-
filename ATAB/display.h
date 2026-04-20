#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>

void display_init();

// MAIN
void display_passenger(String name, String flight, String seat, String gate, int bars, bool connected);
void display_notification(String title, String text, int bars, bool connected);

// WIFI
void display_wifi_connecting();
void display_wifi_connected();
void display_wifi_disconnected();
void display_reconnecting();

// SERVER
void display_server_connecting();
void display_registering();
void display_server_error(int bars);
void display_server_retry();

// POWER
void display_sleep();

#endif