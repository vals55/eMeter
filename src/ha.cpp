#include "ha.h"
#include "utils.h"
#include "porting.h"

void updateChannelName(int channel, String &entity_id, String &entity_name) {
  if (channel != NONE) {
    entity_id += channel;
    entity_name = String(FPSTR(CHANNEL_NAMES[channel])) + " " + entity_name;
  }
}

String buildEntity( const char *mqtt_topic,
                    const char *entity_type,
                    const char *entity_name,
                    const char *entity_id,
                    const char *state_class,
                    const char *device_class,
                    const char *unit_of_meas,
                    const char *entity_category,
                    const char *icon,
                    const char *device_id,
                    const char *device_mac,
                    bool enabled_by_default,
                    const char *device_name,
                    const char *device_manufacturer,
                    const char *device_model,
                    const char *sw_version,
                    const char *hw_version,
                    const char *json_attributes_topic,
                    const char *json_attributes_template) {
  DynamicJsonDocument json_doc(JSON_BUFFER);
  JsonObject entity = json_doc.to<JsonObject>();

  entity[F("name")] = entity_name;

  String uniqueId_prefix = getDeviceName();
  String unique_id = uniqueId_prefix + "-" + entity_id;
  entity[F("uniq_id")] = unique_id.c_str();
  entity[F("obj_id")] = unique_id.c_str();
  entity[F("stat_t")] = mqtt_topic;

  String value_template;
  value_template = String("{{ value_json.") + entity_id + String(" | is_defined }}");
  entity[F("val_tpl")] = value_template.c_str();

  if (state_class[0])
    entity[F("stat_cla")] = state_class;

  if (device_class[0])
    entity[F("dev_cla")] = device_class;

  if (unit_of_meas[0])
    entity[F("unit_of_meas")] = unit_of_meas;

  if (entity_category[0])
    entity[F("ent_cat")] = entity_category;

  if (icon[0])
    entity[F("ic")] = icon;

  if (enabled_by_default)
    entity[F("en")] = enabled_by_default;

  if (MQTT_FORCE_UPDATE)
    entity[F("force_update")] = true;

  StaticJsonDocument<JSON_SMALL_BUFFER> json_device_doc;
  JsonObject device = json_device_doc.to<JsonObject>();
  JsonArray identifiers = device.createNestedArray(F("identifiers"));

  identifiers[0] = device_id;
  identifiers[1] = device_mac;

  if (device_name[0])
    device[F("name")] = device_name;

  if (device_manufacturer[0])
    device[F("manufacturer")] = device_manufacturer;

  if (device_model[0])
    device[F("model")] = device_model;

  if (sw_version[0])
    device[F("sw_version")] = sw_version;

  if (hw_version[0])
    device[F("hw_version")] = hw_version;

  entity[F("device")] = device;

  if (json_attributes_topic[0] && json_attributes_template[0]) {
    entity[F("json_attributes_topic")] = json_attributes_topic;
    entity[F("json_attributes_template")] = json_attributes_template;
  }

  if (strcmp(entity_type, "number") == 0) {
    // https://www.home-assistant.io/integrations/number.mqtt
    String command_topic = String(mqtt_topic) + F("/") + entity_id + F("/set");
    entity[F("cmd_t")] = command_topic;
    entity[F("cmd_tpl")] = F("{{value | round(0) | int}}");
    entity[F("mode")] = F("box");
    entity[F("min")] = 1;
    entity[F("max")] = 65535;
    entity[F("step")] = 1;

    entity[F("optimistic")] = true;
    entity[F("retain")] = true;
    entity[F("qos")] = 1;
  } else if (strcmp(entity_type, "float") == 0) {
    // https://www.home-assistant.io/integrations/number.mqtt
    String command_topic = String(mqtt_topic) + F("/") + entity_id + F("/set");
    entity[F("cmd_t")] = command_topic;
    entity[F("cmd_tpl")] = F("{{value | round(2) | float}}");
    entity[F("mode")] = F("box");
    entity[F("min")] = 1;
    entity[F("max")] = 9999999.99;
    entity[F("step")] = 1;

    entity[F("optimistic")] = true;
    entity[F("retain")] = true;
    entity[F("qos")] = 1;
  }
  

  rlog_i("info", "MQTT: DISCOVERY SENSOR: JSON Mem usage: %d", json_doc.memoryUsage());
  rlog_i("info", "MQTT: DISCOVERY SENSOR: JSON size: %d", measureJson(json_doc));

  String payload;
  serializeJson(entity, payload);

  return payload;
}

String getAttributesTemplate(const char *const attrs[][MQTT_PARAM_COUNT], int index, int count, int channel) {
  String json_attributes_template = "";

  DynamicJsonDocument json_doc(JSON_BUFFER);
  JsonObject json_attributes = json_doc.to<JsonObject>();
  String attribute_name;
  String attribute_id;
  String attribute_template;

  for (int i = 0; i < count; i++) {
    attribute_name = FPSTR(attrs[index + i][1]);
    attribute_id = FPSTR(attrs[index + i][2]);

    updateChannelName(channel, attribute_id, attribute_name);
    attribute_template = String(F("{{ value_json.")) + attribute_id + F(" | is_defined }}");
    json_attributes[attribute_name] = attribute_template;
  }

  serializeJson(json_attributes, json_attributes_template);
  return json_attributes_template;
}

void publishEntity(PubSubClient &mqtt_client, String &topic, String &discovery_topic,
                              const Data &data, String &device_id, String &device_mac,
                              const char *const entities[][MQTT_PARAM_COUNT],
                              int entity_indx,
                              bool extended = false,
                              int attrs_index = NONE,
                              int attrs_count = NONE,
                              int channel = NONE) {

  String device_name = "";
  String device_model = "";
  String sw_version = "";
  String hw_version = "";
  String device_manufacturer = "";
  String entity_type = FPSTR(entities[entity_indx][0]);
  String entity_name = FPSTR(entities[entity_indx][1]);
  String entity_id = FPSTR(entities[entity_indx][2]);
  String state_class = FPSTR(entities[entity_indx][3]);
  String device_class = FPSTR(entities[entity_indx][4]);
  String unit_of_meas = FPSTR(entities[entity_indx][5]);
  String entity_category = FPSTR(entities[entity_indx][6]);
  String icon = FPSTR(entities[entity_indx][7]);
  String json_attributes_topic = "";
  String json_attributes_template = "";

  String uniqueId_prefix = getDeviceName();
  updateChannelName(channel, entity_id, entity_name);

  if (extended) {
    device_name = getDeviceName();
    device_model = FPSTR(MODEL_NAMES[MODEL]);
    sw_version = String(F(FIRMWARE_VERSION));
    hw_version = F(HARDWARE_VERSION); 
    device_manufacturer = F(BRAND_NAME);
  }
  
  rlog_i("info", "MQTT: DISCOVERY:  Sensor: %s", entity_name);
  
  if ((attrs_index != NONE) && (attrs_count != NONE)) {
    json_attributes_topic = topic;
    json_attributes_template = getAttributesTemplate(entities, attrs_index, attrs_count, channel);
  }

  String payload = buildEntity( topic.c_str(),
                                entity_type.c_str(), entity_name.c_str(), entity_id.c_str(),
                                state_class.c_str(), device_class.c_str(), unit_of_meas.c_str(),
                                entity_category.c_str(), icon.c_str(),
                                device_id.c_str(), device_mac.c_str(),
                                true, device_name.c_str(), device_manufacturer.c_str(),
                                device_model.c_str(), sw_version.c_str(), hw_version.c_str(),
                                json_attributes_topic.c_str(), json_attributes_template.c_str());

  String entity_discovery_topic = String(discovery_topic) + "/" + entity_type + "/" + uniqueId_prefix + "/" + entity_id + "/config";
  publish(mqtt_client, entity_discovery_topic, payload, DEFAULT_PUBLISH_MODE);
}

void publishGeneralEntities(PubSubClient &mqtt_client, String &topic, String &discovery_topic, const Data &data, String &device_id, String &device_mac) {
  // добавляем одиночные сенсоры из массива GENERAL_ENTITIES с индекса 0 ("Voltage") до 9 ("RSSI")
  // всего 10 сенсоров без атрибутов
  bool extended = false;
  for (int i = 0; i < 9; i++) {
    extended = i == 0; // в первый сенсор дописываем всю информацию про устройство
    publishEntity(mqtt_client, topic, discovery_topic, data, device_id, device_mac, GENERAL_ENTITIES, i, extended);
  }
  // основной сенсор 10 ("RSSI") атрибуты 11,12,13 (ip, mac, chip)
  publishEntity(mqtt_client, topic, discovery_topic, data, device_id, device_mac, GENERAL_ENTITIES, 10, false, 11, 3);
}

void publishChannelEntities(PubSubClient &mqtt_client, String &topic, String &discovery_topic, const Data &data, String &device_id, String &device_mac) {
  String channel_name = "";
  for (int channel = 0; channel < CHANNEL_COUNT; channel++) {
    // один сенсор из массива CHANNEL_ENTITIES с индексом 0 ("total") будет основным
    // остальные его атрибутами с 1 по индекс 3
    publishEntity(mqtt_client, topic, discovery_topic, data, device_id, device_mac, CHANNEL_ENTITIES, 0, false, 1, 3, channel);
  }
}

void publishHA(PubSubClient &mqtt_client, String &topic, String &discovery_topic, const Data &data) {
  
  rlog_i("info", "MQTT: Publishing HA topic");
  unsigned long start_time = millis();

  String device_id = String(getChipId());
  String device_mac = getMacAddressHex();

  rlog_i("info", "MQTT: General entities");
  publishGeneralEntities(mqtt_client, topic, discovery_topic, data, device_id, device_mac);

  rlog_i("info", "MQTT: Channel entities");
  publishChannelEntities(mqtt_client, topic, discovery_topic, data, device_id, device_mac);

  rlog_i("info", "MQTT: Discovery topic published: %d ms", millis() - start_time);
}
