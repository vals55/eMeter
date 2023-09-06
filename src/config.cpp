// Copyright 2023 vals. All rights reserved.
// SPDX-License-Identifier: MIT
#include <ESP8266WiFi.h>
#include <IPAddress.h>
#include <EEPROM.h>

#include "config.h"
#include "rlog.h"
#include "utils.h"
#include "porting.h"

void storeConfig(const BoardConfig &conf) {
    uint16_t crc = getCRC(conf);
    EEPROM.begin(sizeof(conf) + sizeof(crc));
    EEPROM.put(0, conf);
    EEPROM.put(sizeof(conf), crc);

    if (!EEPROM.commit()) {
        rlog_i("info", "Config store FAILED");
    } 
    else {
        rlog_i("info", "Config store OK crc=%x", crc);
    }
    EEPROM.end();
}

bool loadConfig(BoardConfig &conf) {
    rlog_i("info", "Loading Config...");
    
    uint16_t crc = 0;
    BoardConfig tmp_conf = {};
    EEPROM.begin(sizeof(tmp_conf) + sizeof(crc)); //  4 до 4096 байт. с адреса 0x7b000.
    EEPROM.get(0, tmp_conf);
    EEPROM.get(sizeof(tmp_conf), crc);
    EEPROM.end();

    uint16_t calculated_crc = getCRC(tmp_conf);
    if (crc == calculated_crc) {
        conf = tmp_conf;
        rlog_i("info", "Configuration CRC ok");

        conf.mqtt_host[MQTT_HOST_LEN - 1] = 0;
        conf.mqtt_login[MQTT_LOGIN_LEN - 1] = 0;
        conf.mqtt_password[MQTT_PASSWORD_LEN - 1] = 0;
        conf.mqtt_topic[MQTT_TOPIC_LEN - 1] = 0;
        conf.mqtt_discovery_topic[MQTT_TOPIC_LEN - 1] = 0;
        conf.ntp_server[NTP_HOST_LEN - 1] = 0;

        rlog_i("info", "--- MQTT ---- ");
        rlog_i("info", "host=%s  port=%d", conf.mqtt_host, conf.mqtt_port);
        rlog_i("info", "login=%s  pass=%s", conf.mqtt_login, conf.mqtt_password);
        rlog_i("info", "auto discovery=%d", conf.mqtt_auto_discovery);
        rlog_i("info", "discovery topic=%s", conf.mqtt_discovery_topic);

        rlog_i("info", "--- Network ---- ");
        if (conf.ip) {
            rlog_i("info", "DHCP turn off");
            rlog_i("info", "static_ip=%s", IPAddress(conf.ip).toString());
            rlog_i("info", "gateway=%s", IPAddress(conf.gateway).toString());
            rlog_i("info", "mask=%s", IPAddress(conf.mask).toString());
        }
        else {
            rlog_i("info", "DHCP is on");
        }

        rlog_i("info", "ntp_server=%s", conf.ntp_server);
        rlog_i("info", "tz=%d", conf.tz);

        rlog_i("info", "--- WIFI ---- ");
        rlog_i("info", "ssid=%s", conf.ssid);
        rlog_i("info", "password=%s", conf.password);
        rlog_i("info", "wifi_channel=%d", conf.wifi_channel);

        return true;
    }
    else {
        // Конфигурация не была сохранена в EEPROM, инициализируем с нуля

        rlog_i("info", "ESP config CRC failed. Maybe first run. Init configuration.");
        rlog_i("info", "Saved crc=%x  calculated=%x", crc, calculated_crc);

        conf.version = VERSION;
        rlog_i("info", "cfg version=%d", conf.version);

        String default_topic = String(MQTT_DEFAULT_TOPIC_PREFIX) + "/" + String(getChipId()) + "/";
        strncpy0(conf.mqtt_topic, default_topic.c_str(), default_topic.length() + 1);
        conf.mqtt_port = MQTT_DEFAULT_PORT;

        conf.mqtt_auto_discovery = MQTT_AUTO_DISCOVERY;
        String discovery_topic(HA_DISCOVERY_TOPIC);
        strncpy0(conf.mqtt_discovery_topic, discovery_topic.c_str(), discovery_topic.length() + 1);

        strncpy0(conf.ntp_server, DEFAULT_NTP_SERVER, sizeof(DEFAULT_NTP_SERVER));
        conf.tz = DEFAULT_TZ;
        
        conf.ip = 0;
        IPAddress network_gateway;
        network_gateway.fromString(DEFAULT_GATEWAY);
        conf.gateway = network_gateway;
        IPAddress network_mask;
        network_mask.fromString(DEFAULT_MASK);
        conf.mask = network_mask;
        conf.wifi_channel = 0;
        conf.wifi_phy_mode = 0;
        conf.wifi_bssid[0] = 0;
        conf.coeff = DEFAULT_COEFF;
// Можно задать константы при компиляции, чтобы eMeter сразу заработал

#ifdef MQTT_HOST
#pragma message(VAR_NAME_VALUE(MQTT_HOST))
        String mqtt_host = VALUE(MQTT_HOST);
        strncpy0(sett.mqtt_host, mqtt_host.c_str(), HOST_LEN);
        rlog_i("info", "default mqtt_host=%s", mqtt_host);
#endif

#ifdef MQTT_LOGIN
#pragma message(VAR_NAME_VALUE(MQTT_LOGIN))
        String mqtt_login = VALUE(MQTT_LOGIN);
        strncpy0(sett.mqtt_login, mqtt_login.c_str(), MQTT_LOGIN_LEN);
        rlog_i("info", "default mqtt_login=%s", mqtt_login);
#endif

#ifdef MQTT_PASSWORD
#pragma message(VAR_NAME_VALUE(MQTT_PASSWORD))
        String mqtt_password = VALUE(MQTT_PASSWORD);
        strncpy0(sett.mqtt_password, mqtt_password.c_str(), MQTT_PASSWORD_LEN);
        rlog_i("info", "default mqtt_password=%s", mqtt_password);
#endif
        return false;
    }
}

