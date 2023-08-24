#ifndef __WIFI_H
#define __WIFI_H

#include <ESP8266WiFi.h>
#include "config.h"

#define WIFI_CONNECT_ATTEMPTS 2

extern bool wifiConnect(BoardConfig &conf);
extern void wifiBegin(BoardConfig &conf);
extern void wifiSetMode(WiFiMode_t wifi_mode);
extern void wifiShutdown();
extern String wifiGetMode();

#endif