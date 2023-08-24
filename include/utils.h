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

extern uint16_t getCRC(const BoardConfig &conf);
extern bool isMQTT(const BoardConfig &conf);
extern bool isHA(const BoardConfig &conf);
extern bool isDHCP(const BoardConfig &conf);
extern String getDeviceName();
extern String getAppName();
extern String getMacAddressHex();

extern void blink_start();
extern void stop_led();

#endif