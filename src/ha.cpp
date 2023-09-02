#include "ha.h"
#include "utils.h"
#include "porting.h"

void publishEntity(PubSubClient &mqtt_client, String &topic, String &discovery_topic,
                              const SlaveData &data, String &device_id, String &device_mac,
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

    String uniqueId_prefix = get_device_name();

    update_channel_names(channel, entity_id, entity_name);

    if (extended)
    {
        device_name = get_device_name();
        device_model = FPSTR(MODEL_NAMES[data.model]);
        sw_version = String(F(FIRMWARE_VERSION)) + "." + data.version; // ESP_VERSION.ATTINY_VERSION
        hw_version = F(HARDWARE_VERSION);                              // в дальнейшем можно модифицировать для гибкого определения версии hw
        device_manufacturer = F(MANUFACTURER);
    }

    LOG_INFO(F("MQTT: DISCOVERY:  Sensor: ") << entity_name);

    if ((attrs_index != NONE) && (attrs_count != NONE))
    {
        json_attributes_topic = topic;
        json_attributes_template = get_attributes_template(entities, attrs_index, attrs_count, channel);
    }

    String payload = build_entity_discovery(topic.c_str(),
                                            entity_type.c_str(), entity_name.c_str(), entity_id.c_str(),
                                            state_class.c_str(), device_class.c_str(), unit_of_meas.c_str(),
                                            entity_category.c_str(), icon.c_str(),
                                            device_id.c_str(), device_mac.c_str(),
                                            true, device_name.c_str(), device_manufacturer.c_str(),
                                            device_model.c_str(), sw_version.c_str(), hw_version.c_str(),
                                            json_attributes_topic.c_str(), json_attributes_template.c_str());

    String entity_discovery_topic = String(discovery_topic) + "/" + entity_type + "/" + uniqueId_prefix + "/" + entity_id + "/config";
    publish(mqtt_client, entity_discovery_topic, payload);
}

void publishGeneralEntities(PubSubClient &mqtt_client, String &topic, String &discovery_topic, const SlaveData &data, String &device_id, String &device_mac) {
    // добавляем одиночные сенсоры из массива GENERAL_ENTITIES с индекса 0 ("Battery Voltage") до  7 ("RSSI")
    // всего 7 сенсоров без атрибутов
    bool extended = false;
    for (int i = 0; i < 5; i++)
    {
        extended = i == 0; // в первый сенсор дописываем всю информацию про устройство
        publishEntity(mqtt_client, topic, discovery_topic, data, device_id, device_mac, GENERAL_ENTITIES, i, extended);
    }
    // добавляем сенсор с атрибутами из массива GENERAL_ENTITIES
    // основной сенсор 5 ("Battery Voltage") атрибуты 6 (Voltage diff)
    publishEntity(mqtt_client, topic, discovery_topic, data, device_id, device_mac, GENERAL_ENTITIES, 5, false, 6, 1);
    // основной сенсор 8 ("RSSI") атрибуты 8,9,10 (mac router, mac, ip)
    publishEntity(mqtt_client, topic, discovery_topic, data, device_id, device_mac, GENERAL_ENTITIES, 7, false, 8, 3);
}

void publishHA(PubSubClient &mqtt_client, String &topic, String &discovery_topic, const SlaveData &data) {
  
  rlog_i("info", "MQTT: Publishing HA topic");
  unsigned long start_time = millis();

  String device_id = String(getChipId());
  String device_mac = getMacAddressHex();

  rlog_i("info", "MQTT: General entities");
  publishGeneralEntities(mqtt_client, topic, discovery_topic, data, device_id, device_mac);

  rlog_i("info", "MQTT: Channel entities");
  publish_discovery_channel_entities(mqtt_client, topic, discovery_topic, data, device_id, device_mac);

  rlog_i("info", "MQTT: Discovery topic published: %d ms", millis() - start_time);
}
