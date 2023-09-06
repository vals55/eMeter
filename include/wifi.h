// Copyright 2023 vals. All rights reserved.
// SPDX-License-Identifier: MIT
#ifndef __WIFI_H
#define __WIFI_H

#include <ESP8266WiFi.h>
#include "config.h"

#define WIFI_CONNECT_ATTEMPTS 2

bool wifiConnect(BoardConfig &conf);
void wifiBegin(BoardConfig &conf);
void wifiSetMode(WiFiMode_t wifi_mode);
void wifiShutdown();
String wifiGetMode();

#endif