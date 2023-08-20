#ifndef __PORTING_H
#define __PORTING_H

#ifdef ESP8266
    #include <ESP8266WiFi.h>

    #define getChipId() ESP.getChipId()

#elif defined(ESP32)
    #include <WiFi.h>
    #include <esp_wifi.h>

    #define getChipId() (uint32_t) ESP.getEfuseMac()
    #endif

#endif