// Copyright 2023 vals. All rights reserved.
// SPDX-License-Identifier: MIT
#include "mqtt.h"

void publish_chunked(PubSubClient &mqtt_client, String &topic, String &payload, unsigned int chunk_size) {
  rlog_i("info", "MQTT: Publish free memory: %d payload len: %d topic: %s", ESP.getFreeHeap(), payload.length(), MQTT_DEFAULT_TOPIC_PREFIX);

  unsigned int len = payload.length();
  const uint8_t *buf = (const uint8_t *)payload.c_str();

  if (mqtt_client.beginPublish(topic.c_str(), len, true)) {
    while (len > 0) {
      if (len >= chunk_size) {
        mqtt_client.write(buf, chunk_size);
        buf += chunk_size;
        len -= chunk_size;
        rlog_i("info", "MQTT: Publish: Sended chunk size: %d", chunk_size);
      } else {
        mqtt_client.write(buf, len);
        rlog_i("info", "MQTT: Publish: Sended chunk size: %d", len);
        break;
      }
    }
    if (mqtt_client.endPublish()) {
      rlog_i("info", "MQTT: CHUNK Published succesfully");
    } else {
      rlog_i("info", "MQTT: CHUNK Publish failed");
    }
  } else {
    rlog_i("info", "MQTT: Client not connected.");
  }
}

void publish_big(PubSubClient &mqtt_client, String &topic, String &payload) {
  rlog_i("info", "MQTT: Publish free memory: %d payload len: %d topic: %s", ESP.getFreeHeap(), payload.length(), MQTT_DEFAULT_TOPIC_PREFIX);

  unsigned int len = payload.length();
  if (mqtt_client.beginPublish(topic.c_str(), len, true)) {
    if (mqtt_client.print(payload.c_str()) == len) {
      rlog_i("info", "MQTT: BIG Published succesfully");
    } else {
      rlog_i("info", "MQTT: BIG Publish failed");
    }
    mqtt_client.endPublish();
  } else {
    rlog_i("info", "MQTT: Client not connected.");
  }
}

void publish_simple(PubSubClient &mqtt_client, String &topic, String &payload) {
  rlog_i("info", "MQTT: Publish free memory: %d payload len: %d topic: %s", ESP.getFreeHeap(), payload.length(), MQTT_DEFAULT_TOPIC_PREFIX);

  if (mqtt_client.connected()) {
    if (mqtt_client.publish(topic.c_str(), payload.c_str(), true)) {
      rlog_i("info", "MQTT: SIMPLE Published succesfully");
    } else {
      rlog_i("info", "MQTT: SIMPLE Publish failed");
    }
  } else {
    rlog_i("info", "MQTT: Client not connected.");
  }
}

void publish(PubSubClient &mqtt_client, String &topic, String &payload, int mode) {
  switch (mode) {
    case PUBLISH_MODE_SIMPLE:
      publish_simple(mqtt_client, topic, payload);
      break;
    case PUBLISH_MODE_CHUNKED:
      publish_chunked(mqtt_client, topic, payload, MQTT_CHUNK_SIZE);
      break;
    case PUBLISH_MODE_BIG:
    default:
      publish_big(mqtt_client, topic, payload);
  }
}

void publishToSingleTopic(PubSubClient &mqtt_client, String &topic, DynamicJsonDocument &json_data) {
  String payload = "";
  serializeJson(json_data, payload);
  publish(mqtt_client, topic, payload, DEFAULT_PUBLISH_MODE);
}

void publishToMultipleTopics(PubSubClient &mqtt_client, String &topic, DynamicJsonDocument &json_data) {
  JsonObject root = json_data.as<JsonObject>();
  for (JsonPair p : root) {
    String sensor_topic = topic + "/" + p.key().c_str();
    String sensor_value = p.value().as<String>();
    publish(mqtt_client, sensor_topic, sensor_value, DEFAULT_PUBLISH_MODE);
  }
}

void publishData(PubSubClient &mqtt_client, String &topic, DynamicJsonDocument &json_data, bool auto_discovery) {
  unsigned long start = millis();

  if (auto_discovery) {
    rlog_i("info", "MQTT: Publish data to single topic");
    publishToSingleTopic(mqtt_client, topic, json_data);
  } else {
    rlog_i("info", "MQTT: Publish data to multiple topic");
    publishToMultipleTopics(mqtt_client, topic, json_data);
  }
  rlog_i("info", "MQTT: Publish data finished for %d millis", millis() - start);
}

void publishStorage(PubSubClient &mqtt_client, String &topic, float energy1, float energy2) {
  unsigned long start = millis();
  String storage_topic = topic + F(STORAGE_T0);
  String storage_value = String(energy1);
  publish(mqtt_client, storage_topic, storage_value, PUBLISH_MODE_SIMPLE);
  storage_topic = topic + F(STORAGE_T1);
  storage_value = String(energy2);
  publish(mqtt_client, storage_topic, storage_value, PUBLISH_MODE_SIMPLE);

  rlog_i("info", "MQTT: Publish storage data finished for %d millis", millis() - start);
}
