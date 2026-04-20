#ifndef API_H
#define API_H

#define WIFI_DISCONNECTED   -1
#define SERVER_DISCONNECTED  0
#define SERVER_CONNECTED     1

int api_check_connectivity();
bool api_register_device();
bool api_get_notifications();

// passenger
extern String passenger_name;
extern String seat_number;
extern String flight_number;
extern String origin;
extern String destination;
extern String gate;
extern String device_token;

// notification
extern String notif_title;
extern String notif_text;
extern String notif_priority;
extern String notif_pattern;
extern String notif_audio_id;

#endif