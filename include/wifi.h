#ifndef __WIFI_H
#define __WIFI_H

#include <ESP8266WiFi.h>
#include "config.h"

#define WIFI_CONNECT_ATTEMPTS 2

extern bool wifiConnect(Config &conf);
extern void wifiBegin(Config &conf);
extern void wifiSetMode(WiFiMode_t wifi_mode);
extern void wifiShutdown();
extern String wifiGetMode();

#endif