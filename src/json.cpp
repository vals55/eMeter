// Copyright 2023 vals. All rights reserved.
// SPDX-License-Identifier: MIT
#include "json.h"
#include "data.h"
#include "setup.h"
#include "sync_time.h"
#include "porting.h"

extern uint8_t needOTA;
extern String ver;
extern uint32_t start;

const char* msg[]  = {
  "Обновления не найдены.", 
  "Обновление не требуется.",
  "Есть новое обновление v. ",
  "Установка обновления...",
  "Обновление загружено. Перезагрузка.",
  "Ошибка обновления."
};

char heap[10];
uint32_t bytes;
float kBytes;
char uptime[20];

void getJSONData(const Data &data, DynamicJsonDocument &json_data) {

  JsonObject root = json_data.to<JsonObject>();

  root[F("voltage")]   = data.data.voltage;
  root[F("current")]   = data.data.current;
  root[F("power")]     = data.data.power;
  root[F("frequency")] = data.data.frequency;
  root[F("energy")]    = data.data.energy;  // + data.offset.energy0;
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

void getJSONState(const Data &data, DynamicJsonDocument &json_state) {

  JsonObject root = json_state.to<JsonObject>();

  root[F("inner-voltage")]   = String(data.data.voltage, 1);
  root[F("inner-current")]   = String(data.data.current, 1);
  root[F("inner-power")]     = String(data.data.power, 1);
  root[F("inner-frequency")] = String(data.data.frequency, 1);
  root[F("inner-energy0")]   = String(data.data.energy, 1);
  root[F("inner-pf")]        = String(data.data.pf);
  root[F("inner-energy1")]   = String(data.calc.energy1+data.offset.energy1, 1);
  root[F("inner-energy2")]   = String(data.calc.energy2+data.offset.energy2, 1);

  root[F("inner-maxvoltage")] = String(data.ext.maxvoltage, 1);
  root[F("inner-maxcurrent")] = String(data.ext.maxcurrent, 1);
  root[F("inner-maxpower")]   = String(data.ext.maxpower, 1);
  root[F("inner-maxfreq")]    = String(data.ext.maxfreq, 1);
  root[F("inner-maxpf")]      = String(data.ext.maxpf);
  root[F("inner-minvoltage")] = String(data.ext.minvoltage, 1);
  root[F("inner-mincurrent")] = String(data.ext.mincurrent, 1);
  root[F("inner-minpower")]   = String(data.ext.minpower, 1);
  root[F("inner-minfreq")]    = String(data.ext.minfreq, 1);
  root[F("inner-minpf")]      = String(data.ext.minpf);
  root[F("inner-rssi")]       = WiFi.RSSI();
  
  bytes = ESP.getFreeHeap();
  kBytes = bytes / 1000.0f;
  sprintf(heap, "%.03f", kBytes);
  root[F("inner-heap")]       = heap;

  root[F("inner-freq")]       = ESP.getCpuFreqMHz();
  root[F("inner-firmware")]   = FIRMWARE_VERSION;
  
  getUpTime(start, uptime);
  root[F("inner-uptime")]     = uptime;
  
  root[F("inner-msg")]        = msg[needOTA];
  root[F("inner-ver")]        = needOTA == OTA_UPDATE_READY ? ver : "";
  root[F("style-loader")]     = needOTA > 2 ? "display:inline-block" : "display:none";
  root[F("oncli-btn-upd")]    = needOTA > 1 ? "upd();" : "history.back();";
  root[F("inner-btn-upd")]    = needOTA > 1 ? "Обновить" : "Назад";
}  
