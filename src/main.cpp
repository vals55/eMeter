// Copyright 2023 vals. All rights reserved.
// SPDX-License-Identifier: MIT
#include <Arduino.h>
#include <WiFiManager.h>
#include <rlog.h>
#include "sync_time.h"
#include <PZEM004Tv30.h>
#include <SoftwareSerial.h>

#include "utils.h"
#include "config.h"
#include "data.h"
#include "wifi.h"
#include "setup.h"
#include "calc.h"
#include "json.h"
#include "http.h"
#include "mqtt.h"
#include "ha.h"
#include "buffer.h"

#if defined(ESP32)
    #error "Software Serial is not supported on the ESP32"
#endif

/* Use software serial for the PZEM
 * Pin 12 Rx (Connects to the Tx pin on the PZEM)
 * Pin 13 Tx (Connects to the Rx pin on the PZEM)
*/
#if !defined(PZEM_RX_PIN) && !defined(PZEM_TX_PIN)
#define PZEM_RX_PIN 12
#define PZEM_TX_PIN 13
#endif

// #define OTA_DISABLE
#ifndef OTA_DISABLE
  #include <WiFiUdp.h>
  #include <ArduinoOTA.h>
#endif

// #define WEB_DISABLE
#ifndef WEB_DISABLE
  #include "web.h"
#endif

#ifndef ESP8266
  #define ESP8266
#endif
#define BUTTON 15
#define SETUP_LED 16
#define CNT1_PIN 5
#define CNT2_PIN 4

#define BTN_HOLD_SETUP 5000
#define BTN_CLICK 200

// #define UNDEFINED 0
// #define JUST_SUBSCRIBED 1
// #define SUBSCRIBED

void getData();
bool recalcTariff1(float energy);
bool recalcTariff2(float energy);

// BoardConfig conf;
// Measurements data;
// Extra ext;
// Offset offset;
// Calculations calc;

Data data;

WiFiClient espClient;
PubSubClient mqttClient(espClient);

volatile uint32_t imp1;
volatile uint32_t imp2;

#ifndef WEB_DISABLE
uint8_t needOTA = OTA_UPDATE_THE_SAME;
bool webActive = false;
String ver;
#endif

SoftwareSerial pzemSWSerial(PZEM_RX_PIN, PZEM_TX_PIN);
PZEM004Tv30 pzem(pzemSWSerial);

DynamicJsonDocument json_data(JSON_BUFFER);

// volatile uint32_t debounce1 = 0;
IRAM_ATTR void count1() {
  // if (millis() - debounce1 >= 400 && !digitalRead(CNT1_PIN)) {
  //   debounce1 = millis();
    imp1++;
  // }
}

// volatile uint32_t debounce2 = 0;
IRAM_ATTR void count2() {
  // if (millis() - debounce2 >= 400 && !digitalRead(CNT2_PIN)) {
  //   debounce2 = millis();
    imp2++;
  // }
}

time_t last_call;
uint32_t last_imp1;
uint32_t last_imp2;
uint8_t setup_state;    // init in reconnect()
uint32_t start = 0;
EEPROMBuff<BoardConfig> storage(8);   //4096 / (508+2) = 8 4К должно хватить на нечастые перезаписи

bool updateConfig(String &topic, String &payload) {
  bool updated = false;
  float energy = 0;
  long constant = 0;
  int period = 0;
  if (setup_state) {
    setup_state--;
  }

  rlog_i("info", "MQTT CALLBACK: setup_state = %d", setup_state);
  if (topic.endsWith(F("/set")) && !setup_state) {
    int endslash = topic.lastIndexOf('/');
    int prevslash = topic.lastIndexOf('/', endslash - 1);
    String param = topic.substring(prevslash + 1, endslash);

    rlog_i("info", "MQTT CALLBACK: Parameter %s", param.c_str());
  
    if (param.equals(F("energy1"))) {
      energy = payload.toFloat();
      if (energy > 0) {
        updated = recalcTariff1(energy);
        rlog_i("info", "MQTT CALLBACK: new value of T1 energy: %f",  energy);
      }
    }
    if (param.equals(F("energy2"))) {
      energy = payload.toFloat();
      if (energy > 0) {
        updated = recalcTariff2(energy);
        rlog_i("info", "MQTT CALLBACK: new value of T2 energy: %f",  energy);
      }
    }
    if (param.equals(F("mqtt_period"))) {
      period = payload.toInt();
      if (period > 0) {
        if (period != data.conf.mqtt_period) {
          data.conf.mqtt_period = period;
          updated = true;
          rlog_i("info", "MQTT CALLBACK: new value of mqtt_period: %d",  period);
        }
      }
    }
    if (param.equals(F("stat_period"))) {
      period = payload.toInt();
      if (period > 0) {
        if (period != data.conf.stat_period) {
          data.conf.stat_period = period;
          updated = true;
          rlog_i("info", "MQTT CALLBACK: new value of stat_period: %d",  period);
        }
      }
    }
    if (param.equals(F("energy_offset"))) {
      energy = payload.toFloat();
      if (energy != data.offset.energy0) {
        data.offset.energy0 = energy;
        updated = true;
        rlog_i("info", "MQTT CALLBACK: new value of energy offset: %d",  energy);
      }
    }
    if (param.equals(F("constant"))) {
      constant = payload.toInt();
      if (constant != (int)data.conf.coeff) {
        if (constant == 0) {
          data.conf.coeff = DEFAULT_COEFF;
          updated = true;
        } else {
          data.conf.coeff = constant;
          updated = true;
        }
        rlog_i("info", "MQTT CALLBACK: new value of constant: %d",  constant);
      }
    }
// after re-subscribe
  } else if (topic.endsWith(F(STORAGE_T1))) {
    energy = payload.toFloat();
    if (energy > 0 && setup_state) {
      rlog_i("info", "MQTT CALLBACK: get STORAGE value and SET to T1 energy: %f",  energy);
      recalcTariff1(energy);
    }
  } else if (topic.endsWith(F(STORAGE_T2))) {
    energy = payload.toFloat();
    if (energy > 0 && setup_state) {
      rlog_i("info", "MQTT CALLBACK: get STORAGE value and SET to T2 energy: %f",  energy);
      recalcTariff2(energy);
     }
  } else if (topic.endsWith(F(STORAGE_T0))) {
    energy = payload.toFloat();
//    if (isnan(energy) && setup_state) {
    if (setup_state) {
      data.offset.energy0 = energy;
//      updated = true;
      rlog_i("info", "MQTT CALLBACK: get STORAGE value and SET to T0 energy: %f",  energy);
    }
  } else if (topic.endsWith(F(STORAGE_CONSTANT))) {
    constant = payload.toInt();
    if (isnan(constant) && setup_state) {
        if (constant == 0) {
          data.conf.coeff = DEFAULT_COEFF;
          updated = true;
        } else {
          data.conf.coeff = constant;
          updated = true;
        }
      rlog_i("info", "MQTT CALLBACK: get STORAGE value and SET to constant: %f",  constant);
    }
  }
 
  return updated;
}

void callback(char* topic, byte* payload, unsigned int length) {
  String mTopic = topic;
  String mPayload;
  mPayload.reserve(length);

  rlog_i("info", "MQTT CALLBACK: Message arrived to: %s (len=%d)", mTopic.c_str(), length);
  for (unsigned int i = 0; i < length; i++) {
    mPayload += (char)payload[i];
  }
  updateConfig(mTopic, mPayload);
  rlog_i("info", "MQTT CALLBACK: Message payload: %s", mPayload.c_str());
}

uint32_t progressDelay = 0;

bool reconnect() {
  if (!isMQTT(data.conf)) {
    return false;
  }
  String client_id = getDeviceName();
  String topic = data.conf.mqtt_topic;
  removeSlash(topic);
  String subscribe_topic = topic + F(MQTT_ALL_TOPICS);
  int attempts = MQTT_MAX_TRIES;
  const char *login = data.conf.mqtt_login[0] ? data.conf.mqtt_login : NULL;
  const char *pass = data.conf.mqtt_password[0] ? data.conf.mqtt_password : NULL;

  rlog_i("info", "MQTT Connecting...");
  while (!mqttClient.connected() && attempts--) {
    rlog_i("info", "MQTT Attempt #%d from %d", MQTT_MAX_TRIES - attempts, MQTT_MAX_TRIES);
    if (mqttClient.connect(client_id.c_str(), login, pass)) {
      progressDelay = 0;
      rlog_i("info", "MQTT Connected. progress delay = %d", progressDelay);
      mqttClient.subscribe(subscribe_topic.c_str(), MQTT_QOS);
      rlog_i("info", "MQTT subscribed to: %s", subscribe_topic.c_str());
      setup_state = 9;
      return true;
    } else {
      rlog_i("info", "MQTT client connect failed with state %d", mqttClient.state());
      progressDelay += MQTT_PROGRESS_DELAY;
      delay(MQTT_CONNECT_DELAY);
    }
  }
  return mqttClient.connected();
}

bool recalcTariff1(float energy) {
  if (data.offset.energy1 != energy) {
    rlog_i("info", "RECALC 1: Old Offset.energy1: %f SET new %f", data.offset.energy1, energy);
    data.offset.energy1 = energy;
    data.offset.impulses1 = energy * data.conf.coeff;
    data.data.impulses1 = 0;
    data.calc.energy1 = 0;
    imp1 = 0;

    if (json_data.containsKey("energy01")) {
        json_data[F("energy01")] = data.offset.energy1;
    }
    if (json_data.containsKey("imp01")) {
        json_data[F("imp01")] = data.offset.impulses1;
    }
    if (json_data.containsKey("imp1")) {
        json_data[F("imp1")] = data.data.impulses1;
    }
    return true;
  }
  return false;
}

bool recalcTariff2(float energy) {
  if (data.offset.energy2 != energy) {
    rlog_i("info", "RECALC 2: Old Offset.energy2: %f SET new %f", data.offset.energy2, energy);
    data.offset.energy2 = energy;
    data.offset.impulses2 = energy * data.conf.coeff;
    data.data.impulses2 = 0;
    data.calc.energy2 = 0;
    imp2 = 0;

    if (json_data.containsKey("energy02")) {
        json_data[F("energy02")] = data.offset.energy2;
    }
    if (json_data.containsKey("imp02")) {
        json_data[F("imp02")] = data.offset.impulses2;
    }
    if (json_data.containsKey("imp2")) {
        json_data[F("imp2")] = data.data.impulses2;
    }
    return true;
  }
  return false;
}

void flashLED() {
  digitalWrite(SETUP_LED, HIGH);
  delay(5);
  digitalWrite(SETUP_LED, LOW);
}

void setupBoard() {
  
  digitalWrite(SETUP_LED, HIGH);
#ifndef WEB_DISABLE
    if(webActive) {
      webActive = stopWeb();
    }
#endif
  WiFi.persistent(false);
  WiFi.disconnect();
  WiFi.setSleepMode(WIFI_NONE_SLEEP);

  wifiSetMode(WIFI_AP_STA);
  delay(1000);

  startAP(data.conf);

  wifiShutdown();

  digitalWrite(SETUP_LED, LOW);
  
  rlog_i("info", "Restart ESP");
  ESP.restart();
}

void getData() {
  
  // String curr_time = getCurrentTime();
  
  float voltage = pzem.voltage();
  float current = pzem.current();
  float power = pzem.power();
  float energy = pzem.energy();
  float frequency = pzem.frequency();
  float pf = pzem.pf();

  if (power > 9999.9f) {
    pzem.resetEnergy();
  }

#define NOT_ROUND_DATA
#ifdef ROUND_DATA
  data.data.voltage = isnan(voltage) ? 0.0 : round(voltage * 10)/10;
  data.data.current = isnan(current) ? 0.0 : round(current * 10)/10;
  data.data.power = isnan(power) ? 0.0 : round(power * 10)/10;
  data.data.energy = isnan(energy) ? 0.0 : round(energy * 100)/100;
  data.data.frequency = isnan(frequency) ? 0.0 : round(frequency * 10)/10;
  data.data.pf = (pf == 0.0f || isnan(pf)) ? 1.0 : round(pf * 100)/100;
#else
  data.data.voltage = isnan(voltage) ? 0.0 : voltage;
  data.data.current = isnan(current) ? 0.0 : current;
  data.data.power = isnan(power) ? 0.0 : power;
  data.data.energy = isnan(energy) ? 0.0 : energy + data.offset.energy0;
  data.data.frequency = isnan(frequency) ? 0.0 : frequency;
  data.data.pf = (pf == 0.0f || isnan(pf)) ? 1.0 : pf;
#endif
  // rlog_i("measurment", "Address: %04x", pzem.readAddress());
  // rlog_i("measurment", "Voltage: %f", data.data.voltage);
  // rlog_i("measurment", "Current: %f", data.data.current);
  // rlog_i("measurment", "Power: %f", data.data.power);
  // rlog_i("measurment", "Energy: %f Offset: %f", data.data.energy, data.offset.energy0);
  // rlog_i("measurment", "Freq: %f", data.data.frequency);
  // rlog_i("measurment", "pf: %f", data.data.pf);

  calcExtraData(data.data, data.ext);
  
  float coeff = (float)data.conf.coeff;
  if (!coeff) {
    coeff = 3200.0f;
  }

  data.calc.energy1 = (float)imp1 / coeff;
  data.calc.energy2 = (float)imp2  / coeff;

  time_t now = time(nullptr);
  long period = now - last_call;
  if (period < 60) {
    return;
  }
  last_call = now;

  data.calc.power1 = (float)(imp1 - last_imp1) * 1000.0f * period / coeff;
  last_imp1 = imp1;
  data.calc.power2 = (float)(imp2 - last_imp2) * 1000.0f * period / coeff;
  last_imp2 = imp2;
  voltage = isnan(voltage) ? 220.0f : voltage;
  data.calc.voltage = voltage;
  data.calc.current1 = data.calc.power1 / voltage / data.data.pf;
  data.calc.current2 = data.calc.power2 / voltage / data.data.pf;

  rlog_i("measurment", "imp1: %d", imp1);
  rlog_i("measurment", "imp2: %d", imp2);
  rlog_i("measurment", "voltage: %f", voltage);
  rlog_i("measurment", "pf: %f", data.data.pf);
  rlog_i("measurment", "energy: %f", data.data.energy);
  rlog_i("measurment", "energy offset: %f", data.offset.energy0);
  rlog_i("measurment", "calc energy1: %f", data.calc.energy1);
  rlog_i("measurment", "calc energy2: %f", data.calc.energy2);
  rlog_i("measurment", "calc power1: %f", data.calc.power1);
  rlog_i("measurment", "calc power2: %f", data.calc.power2);
  rlog_i("measurment", "calc current1: %f", data.calc.current1);
  rlog_i("measurment", "calc current2: %f", data.calc.current2);

}

#ifndef WEB_DISABLE

WiFiClient http;

uint8_t isFirmwareReady() {
  
  String ret;
  int ind_md5;

  if(http.connect(OTA_SERVER, OTA_PORT)) {
    String request("mac=");
    request += WiFi.macAddress();
    http.printf("GET " OTA_REQ "?%s HTTP/1.1\r\n", request.c_str());
    http.println("Host: " OTA_SERVER);
    http.println("User-agent: ESP8266/1.0");
    http.println("Connection: close");
    http.println();
    ret = http.readString();
    ind_md5 = ret.indexOf("md5:");
    ver = ret.substring(ret.indexOf("firmware:") + 9, ind_md5);
    ret = ret.substring(ind_md5 + 4);
    http.flush();
    http.stop();
  }
  
  rlog_i("info", "OTA ver=%s md5=%s", ver, ret.c_str());
  if(ret == "-1") {
    rlog_i("info", "OTA not found");
    return NO_UPDATES;
  } else if (ret == ESP.getSketchMD5()) {
    rlog_i("info", "OTA not need md5 the same!");
    return OTA_UPDATE_THE_SAME;
  } else if (ver.toFloat() > String(FIRMWARE_VERSION).toFloat()) {
    rlog_i("info", "OTA need ver=%s", ver);
    return OTA_UPDATE_READY;
  } else {
    rlog_i("info", "OTA no updates");
    return NO_UPDATES;
  }
  
  rlog_i("info", "firmware=%s vs sketch=%s", ret.c_str(), ESP.getSketchMD5().c_str());
  return true;
}
#endif  

bool success = false;

void setup() {
  
  pinMode(SETUP_LED, OUTPUT);
  digitalWrite(SETUP_LED, LOW);

  pinMode (CNT1_PIN, INPUT_PULLUP);
  attachInterrupt(CNT1_PIN, count1, RISING);
  pinMode (CNT2_PIN, INPUT_PULLUP);
  attachInterrupt(CNT2_PIN, count2, RISING);

  Serial.begin(115200);
  Serial.println();
  rlog_i("info", "Boot ok");
  rlog_i("info", "BoardConfig size: %d", sizeof(BoardConfig));
  
  success = loadConfig(data.conf);
  rlog_i("info", "loadConfig = %d", success);
  if (!success) {
    rlog_i("info", "Setup board entering");
    setupBoard();
  }
 
  success = wifiConnect(data.conf);
  rlog_i("info", "WiFi connect = %d", success);

  if (!success) {
    rlog_i("info", "Setup board entering");
    setupBoard();
  }

#ifdef WIFI_DEBUG_INFO
  rlog_i("info", "WiFi debug info enabled");
  char heap[10];
  uint32_t bytes = ESP.getFreeHeap();
  float KiB = bytes / 1024.0f;
  sprintf(heap, "%.03f", KiB);
  rlog_i("info", "ESP free heap = %s KiB", heap);
  
  // wm.setDebugOutput(true);
  // wm.debugPlatformInfo();

  //reset settings - for testing
  // wm.resetSettings();
  // wm.erase();
#endif

  if(success && isNTP(data.conf)) {
    success = syncTime(data.conf);
    rlog_i("info", "sync_ntp_time = %d", success);
    if (success) {
      start = time(nullptr);
    }
  }
  
  if (data.offset.energy1 == 0) {
    recalcTariff1(data.conf.counter_t1);
  }
  if (data.offset.energy2 == 0) {
    recalcTariff2(data.conf.counter_t2);
  }

  if(isMQTT(data.conf) && (WiFi.status() == WL_CONNECTED)) {
    rlog_i("info", "MQTT begin");
    espClient.setTimeout(MQTT_SOCKET_TIMEOUT * 1000);
    mqttClient.setBufferSize(MQTT_MAX_PACKET_SIZE);
    mqttClient.setServer(data.conf.mqtt_host, data.conf.mqtt_port);
    mqttClient.setSocketTimeout(MQTT_SOCKET_TIMEOUT);
    mqttClient.setCallback(callback);
  }

  #ifndef OTA_DISABLE
    ArduinoOTA.begin();
  #endif
  
  #ifndef WEB_DISABLE
    needOTA = isFirmwareReady();
    webActive = startWeb();
  #endif
}

bool flag = false;
uint32_t btnTimer = 0;
uint32_t mqttTimer = 0;
uint32_t storageTimer = 0;
uint32_t statisticTimer = 0;
uint32_t measurementTimer = 0;
uint32_t stateTimer = 0;
uint32_t otaTimer = 0;
uint32_t secTimer = 0;
uint32_t lastReconnectAttempt = 0;

void loop() {
//  bool success = false;

  #ifndef OTA_DISABLE
  ArduinoOTA.handle();
  #endif
  
  if((WiFi.status() == WL_CONNECTED)) {
  #ifndef WEB_DISABLE
    handleWeb();
  #endif

    if(start == 0) {
      if (WiFi.getMode() == WIFI_STA && isNTP(data.conf)) {
        success = syncTime(data.conf);
        rlog_i("info", "LOOP sync_ntp_time = %d mode=%d", success, WiFi.getMode());
        if (success) {
          start = time(nullptr);
        }
      }
    }

  #define MQTT_ENABLE 1
  
    if (MQTT_ENABLE && isMQTT(data.conf) && !mqttClient.connected()) {
      long now = millis();
      if (now - lastReconnectAttempt > progressDelay + MQTT_RECONNECT_GAP) {
        rlog_i("info loop >>>>>", "MQTT reconnect GAP. progress delay = %d", progressDelay);
        lastReconnectAttempt = now;
        if (reconnect()) {
          lastReconnectAttempt = 0;
        }
      }
    } else {
      mqttClient.loop();
    }
  }

  //button
	bool btnState = digitalRead(BUTTON);
  if (btnState && !flag && millis() - btnTimer > 100) {
    flag = true;
    btnTimer = millis();
    rlog_i("info loop >>>>>", "PRESS");
  }
  
  if (btnState && flag && millis() - btnTimer > BTN_HOLD_SETUP) {
    btnTimer = millis();
    rlog_i("info loop >>>>>", "PRESS HOLD");
    setupBoard();
  }
  
  if (!btnState && flag && millis() - btnTimer > BTN_CLICK) {
    btnTimer = millis();
#ifndef WEB_DISABLE
    if(webActive) {
      webActive = stopWeb();
    } else {
      webActive = startWeb();
    }
#endif
    rlog_i("info loop >>>>>", "MAKE CLICK");
  }
  
  if (!btnState && flag && millis() - btnTimer > 100) {
    flag = false;
    btnTimer = millis();
    rlog_i("info loop >>>>>", "RELEASE");
  }
  
  // timers
  // mqtt
  if((WiFi.status() == WL_CONNECTED)) {

    if (MQTT_ENABLE && data.conf.mqtt_period && millis() - mqttTimer >= progressDelay + data.conf.mqtt_period * PERIOD_MIN) {
      mqttTimer = millis();
      rlog_i("info loop >>>>>", "timer MQTT. progress delay = %d", progressDelay);
      
      if(isMQTT(data.conf) && (WiFi.status() == WL_CONNECTED)) {
        getData();
        if (reconnect()) {
          getJSONData(data, json_data);
          String topic = data.conf.mqtt_topic;
          removeSlash(topic);
          publishStorage(mqttClient, topic, data.calc.energy1+data.offset.energy1, data.calc.energy2+data.offset.energy2, data.offset.energy0, data.conf.coeff);
          publishData(mqttClient, topic, json_data, data.conf.mqtt_auto_discovery);
          String discovery_topic = data.conf.mqtt_discovery_topic;
          publishHA(mqttClient, topic, discovery_topic);
        }
        flashLED();
      }
    }
  }
  
  // storage mqtt
  if((WiFi.status() == WL_CONNECTED)) {
    if (MQTT_ENABLE && data.conf.mqtt_period && millis() - storageTimer >= progressDelay + 1 * PERIOD_MIN) {
      storageTimer = millis();

      if(isMQTT(data.conf) && (WiFi.status() == WL_CONNECTED)) {
        rlog_i("info loop >>>>>", "STORAGE timer MQTT. progress delay = %d", progressDelay);
        getData();
        if (reconnect()) {
          String topic = data.conf.mqtt_topic;
          removeSlash(topic);
          publishStorage(mqttClient, topic, data.calc.energy1+data.offset.energy1, data.calc.energy2+data.offset.energy2, data.offset.energy0, data.conf.coeff);
        }
        flashLED();
      }
    }
  }

  // statistic
#define STAT_ENABLE 1
  if((WiFi.status() == WL_CONNECTED) && data.conf.stat_period) {
    if (STAT_ENABLE && data.conf.stat_period && millis() - statisticTimer >= data.conf.stat_period * PERIOD_SEC) {
      statisticTimer = millis();

      if(isStat(data.conf) && (WiFi.status() == WL_CONNECTED)) {
        rlog_i("info loop >>>>>", "timer Statistic");
        getData();
        getJSONData(data, json_data);
        sendHTTP(data.conf, json_data);
        flashLED();
      }
    }
  }

  // measurement
  if (millis() - measurementTimer >= PERIOD_MEASUREMENT) {
    measurementTimer = millis();
    getData();
    flashLED();
  }
  
  // check own state
  if (millis() - stateTimer >= PERIOD_CHECK_STATE) {
    stateTimer = millis();
    data.data.impulses1 = imp1;
    data.data.impulses2 = imp2;
  }
  
  // OTA check firmware
  if (millis() - otaTimer >= PERIOD_CHECK_OTA) {
    otaTimer = millis();
#ifndef WEB_DISABLE
    if(webActive) {
      webActive = stopWeb();
      needOTA = isFirmwareReady();
      webActive = startWeb();
    } else {
      needOTA = isFirmwareReady();
    }
    rlog_i("info loop >>>>>", "timer OTA ready=%d", needOTA);
#endif
    success = syncTime(data.conf);
    rlog_i("info loop >>>>>", "sync_ntp_time=%d", success);
  }
  
  // OTA processing one sec timer
  if (millis() - secTimer >= 5 * PERIOD_SEC) {
#ifndef WEB_DISABLE
    if(needOTA == OTA_UPDATE_FINISH) {
      ESP.restart();
    }
    if(needOTA == OTA_UPDATE_START) {
      startOTA();
      needOTA = OTA_UPDATE_FINISH;
    }
    secTimer = millis();
#endif
// //сторож конфигурации
//     if (!testConfig(data.conf)) {
//         ESP.restart();
//     }
  }
  delay(50);
}
