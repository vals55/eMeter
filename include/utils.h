// Copyright 2023 vals. All rights reserved.
// SPDX-License-Identifier: MIT
#ifndef __UTILS_H
#define __UTILS_H

#include "config.h"

#define MAC_STR "%02X:%02X:%02X:%02X:%02X:%02X"
#define MAC_STR_HEX "%02X%02X%02X%02X%02X%02X"
#define PROTO_HTTPS "https"
#define PROTO_HTTP "http"

inline void strncpy0(char *dest, const char *src, const size_t len) {
    strncpy(dest, src, len - 1);
    dest[len - 1] = 0;
}

uint16_t getCRC(const BoardConfig &conf);
bool isMQTT(const BoardConfig &conf);
bool isStat(const BoardConfig &conf);
bool isHA(const BoardConfig &conf);
bool isDHCP(const BoardConfig &conf);
String getDeviceName();
String getAppName();
String getMacAddressHex();
String getProtocol(const String &url);
void removeSlash(String &str);

#endif