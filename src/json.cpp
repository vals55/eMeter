// Copyright 2023 vals. All rights reserved.
// SPDX-License-Identifier: MIT
#include "json.h"
#include "data.h"
#include "setup.h"
#include "sync_time.h"
#include "porting.h"

void getJSONData(const Data &data, DynamicJsonDocument &json_data) {

  JsonObject root = json_data.to<JsonObject>();

  root[F("voltage")]   = data.data.voltage;
  root[F("current")]   = data.data.current;
  root[F("power")]     = data.data.power;
  root[F("frequency")] = data.data.frequency;
  root[F("energy")]    = data.data.energy + data.offset.energy0;
  root[F("pf")]        = data.data.pf;

  // root[F("counter_t1")] = data.conf.counter_t1;
  // root[F("counter_t2")] = data.conf.counter_t2;

  root[F("imp1")]  = data.data.impulses1;
  root[F("imp2")]  = data.data.impulses2;
  root[F("imp01")] = data.data.impulses1 + data.offset.impulses1;
  root[F("imp02")] = data.data.impulses2 + data.offset.impulses2;

  root[F("voltage01")] = data.calc.voltage;
  root[F("current01")] = data.calc.current1;
  root[F("power01")]   = data.calc.power1;
  root[F("energy01")]  = data.calc.energy1 + data.offset.energy1;
  root[F("voltage02")] = data.calc.voltage;
  root[F("current02")] = data.calc.current2;
  root[F("power02")]   = data.calc.power2;
  root[F("energy02")]  = data.calc.energy2 + data.offset.energy2;
  root[F("constant")]  = data.conf.coeff;

  root[F("rssi")] = WiFi.RSSI();
  root[F("mac")]  = WiFi.macAddress();
  root[F("ip")]   = WiFi.localIP();

  root[F("ver")]       = data.conf.version;
  root[F("ver_esp")]   = FIRMWARE_VERSION;
  root[F("chip_id")]   = getChipId();
  root[F("freemem")]   = ESP.getFreeHeap();
  // root[F("timestamp")] = getCurrentTime();
  root[F("timestamp")] = getLocalTime();

  root[F("mqtt_period")] = data.conf.mqtt_period;
  root[F("stat_period")] = data.conf.stat_period;
}  
