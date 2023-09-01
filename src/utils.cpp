#include <Arduino.h>
#include <Ticker.h>
#include <ESP8266WiFi.h>
#include "utils.h"
#include "config.h"
#include "rlog.h"
#include "wifi.h"
#include "porting.h"

uint16_t getCRC(const BoardConfig &conf) {
	uint8_t *buf = (uint8_t *)&conf;
	uint16_t crc = 0xffff, poly = 0xa001;
	uint16_t i = 0;
	uint16_t len = sizeof(conf) - 2;

	for (i = 0; i < len; i++) {
		crc ^= buf[i];
		for (uint8_t j = 0; j < 8; j++) {
			if (crc & 0x01)
			{
				crc >>= 1;
				crc ^= poly;
			}
			else
				crc >>= 1;
		}
	}
	rlog_i("info", "get_checksum crc=%x", crc);
	return crc;
}

bool isMQTT(const BoardConfig &conf) {
#ifndef MQTT_DISABLED
	return conf.mqtt_host[0];
#else
	return false;
#endif
}

bool isStat(const BoardConfig &conf) {
#ifndef STAT_DISABLED
	return conf.stat_host[0];
#else
	return false;
#endif
}

bool isHA(const BoardConfig &conf) {
#ifndef MQTT_DISABLED
	return isMQTT(conf) && conf.mqtt_auto_discovery;
#else
	return false;
#endif
}

bool isDHCP(const BoardConfig &conf) {
	return conf.ip != 0;
}

String getDeviceName() {
	String deviceName = String(BRAND_NAME) + "-" + getChipId();
	return deviceName;
}

String getAppName() {
	return getDeviceName() + "-" + String(FIRMWARE_VERSION);
}

String getMacAddressHex() {
	uint8_t baseMac[6];
	char baseMacChr[13] = {0};
	WiFi.macAddress(baseMac);
	sprintf(baseMacChr, MAC_STR_HEX, baseMac[0], baseMac[1], baseMac[2], baseMac[3], baseMac[4], baseMac[5]);
	return String(baseMacChr);
}

String getProtocol(const String &url) {
	String proto = PROTO_HTTP;
	int index = url.indexOf(':');
	if (index > 0) {
		proto = url.substring(0, index);
		proto.toLowerCase();
	}
	return proto;
}

void removeSlash(String &str) {
	if (str.endsWith(F("/"))) {
		str.remove(str.length() - 1);
	}
}
