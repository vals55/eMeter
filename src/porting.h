#ifndef PORTING_H_
#define PORTING_H_

#ifdef ESP8266
    #include <ESP8266WiFi.h>

    #define getChipId() ESP.getChipId()

#elif defined(ESP32)
    #include <WiFi.h>
    #include <esp_wifi.h>

    #define getChipId() (uint32_t) ESP.getEfuseMac()
    #endif

#endif