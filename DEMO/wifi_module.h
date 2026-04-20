#ifndef WIFI_MODULE_H
#define WIFI_MODULE_H

void wifi_init();
void wifi_connect();
void wifi_maintain();
bool is_wifi_connected();
int get_wifi_strength();

#endif