#include "json.h"
#include "data.h"
#include "setup.h"
#include "sync_time.h"
#include "porting.h"

void getJSONData(const Data &data, DynamicJsonDocument &json_data) {

  JsonObject root = json_data.to<JsonObject>();

  root[F("voltage0")] = data.data.voltage;
  root[F("current0")] = data.data.current;
  root[F("power0")] = data.data.power;
  root[F("frequency")] = data.data.frequency;
  root[F("energy0")] = data.data.energy;
  root[F("pf")] = data.data.pf;

  root[F("counter_t0")] = data.conf.counter_t0;
  root[F("counter_t1")] = data.conf.counter_t1;

  root[F("imp01")] = data.data.impulses1;
  root[F("imp02")] = data.data.impulses2;
  root[F("imp1")] = data.data.impulses1 + data.offset.impulses1;
  root[F("imp2")] = data.data.impulses2 + data.offset.impulses2;

  root[F("voltage1")] = data.calc.voltage;
  root[F("current1")] = data.calc.current1;
  root[F("power1")] = data.calc.power1;
  root[F("energy1")] = data.calc.energy1 + data.offset.energy1;
  root[F("voltage2")] = data.calc.voltage;
  root[F("current2")] = data.calc.current2;
  root[F("power2")] = data.calc.power2;
  root[F("energy2")] = data.calc.energy2 + data.offset.energy2;

  root[F("rssi")] = WiFi.RSSI();
  root[F("mac")] = WiFi.macAddress();
  root[F("ip")] = WiFi.localIP();

  root[F("ver")] = data.conf.version;
  root[F("ver_esp")] = FIRMWARE_VERSION;
  root[F("chip_id")] = getChipId();
  root[F("freemem")] = ESP.getFreeHeap();
  root[F("timestamp")] = getCurrentTime();

  root[F("mqtt_period")] = data.conf.mqtt_period;
  root[F("stat_period")] = data.conf.stat_period;
}  
