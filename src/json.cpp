#include "json.h"
#include "data.h"
#include "setup.h"
#include "sync_time.h"
#include "porting.h"

void getJSONData(const BoardConfig &conf, const Measurements &data, const Offset &offset, const Calculations & calc, DynamicJsonDocument &json_data) {

  JsonObject root = json_data.to<JsonObject>();

  root[F("voltage0")] = data.voltage;
  root[F("current0")] = data.current;
  root[F("power0")] = data.power;
  root[F("frequency")] = data.frequency;
  root[F("energy0")] = data.energy;
  root[F("pf")] = data.pf;

  root[F("counter_t0")] = conf.counter_t0;
  root[F("counter_t1")] = conf.counter_t1;

  root[F("imp01")] = data.impulses1;
  root[F("imp02")] = data.impulses2;
  root[F("imp1")] = data.impulses1 + offset.impulses1;
  root[F("imp2")] = data.impulses2 + offset.impulses2;

  root[F("voltage1")] = calc.voltage;
  root[F("current1")] = calc.current1;
  root[F("power1")] = calc.power1;
  root[F("energy1")] = calc.energy1 + offset.energy1;
  root[F("voltage2")] = calc.voltage;
  root[F("current2")] = calc.current2;
  root[F("power2")] = calc.power2;
  root[F("energy2")] = calc.energy2 + offset.energy2;

  root[F("rssi")] = WiFi.RSSI();
  root[F("mac")] = WiFi.macAddress();
  root[F("ip")] = WiFi.localIP();

  root[F("ver")] = conf.version;
  root[F("ver_esp")] = FIRMWARE_VERSION;
  root[F("chip_id")] = getChipId();
  root[F("freemem")] = ESP.getFreeHeap();
  root[F("timestamp")] = getCurrentTime();

  root[F("mqtt_period")] = conf.mqtt_period;
  root[F("stat_period")] = conf.stat_period;
}  
