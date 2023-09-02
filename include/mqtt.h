#ifndef __MQTT_H
#define __MQTT_H

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "rlog.h"
#include "config.h"
#include "data.h"
#include "utils.h"

#define MQTT_MAX_TRIES 5
#define MQTT_CONNECT_DELAY 100
#define MQTT_ALL_TOPICS "/#"
#define MQTT_QOS 1
#define MQTT_RECONNECT_GAP 5000

#define MQTT_CHUNK_SIZE 128
#define PUBLISH_MODE_BIG 0
#define PUBLISH_MODE_CHUNKED 1
#define PUBLISH_MODE_SIMPLE 2
#define DEFAULT_PUBLISH_MODE PUBLISH_MODE_BIG

#define STORAGE_T0 "/energy_t0"
#define STORAGE_T1 "/energy_t1"

void publish(PubSubClient &mqtt_client, String &topic, String &payload, int mode);
void publishData(PubSubClient &mqtt_client, String &topic, DynamicJsonDocument &json_data, bool auto_discovery);
void publishStorage(PubSubClient &mqtt_client, String &topic, float energy1, float energy2);

#endif
