// Copyright 2023 vals. All rights reserved.
// SPDX-License-Identifier: MIT
#ifndef __HA_H
#define __HA_H

#include "resources.h"
#include "mqtt.h"
#include "config.h"
#include "json.h"

#define MQTT_FORCE_UPDATE true

void publishHA(PubSubClient &mqtt_client, String &topic, String &discovery_topic);

#endif
