#include <Arduino.h>
#include <WiFiManager.h>
#include <rlog.h>
#include "sync_time.h"
#include "utils.h"
#include "config.h"
#include "data.h"
#include "wifi.h"
#include "setup.h"
#include "calc.h"

#define USEOTA
// enable OTA
#ifdef USEOTA
  #include <WiFiUdp.h>
  #include <ArduinoOTA.h>
#endif

#define USEWEB
// enable OTA
#ifdef USEWEB
  #include "web.h"
#endif

#ifndef ESP8266
  #define ESP8266
#endif
#define BUTTON 15
#define SETUP_LED 2

#define BTN_HOLD_SETUP 5000
#define BTN_CLICK 200

void sendData();

Config conf;
Measurements data;
Extra ext;
#ifdef USEWEB
uint8_t needOTA = OTA_UPDATE_THE_SAME;
bool webActive = false;
String ver;
#endif

void flashLED() {
  digitalWrite(SETUP_LED, HIGH);
  delay(20);
  digitalWrite(SETUP_LED, LOW);
}

void setupBoard() {
  
  digitalWrite(SETUP_LED, HIGH);
#ifdef USEWEB
    if(webActive) {
      webActive = stopWeb();
    }
#endif
  WiFi.persistent(false);
  WiFi.disconnect();
  WiFi.setSleepMode(WIFI_NONE_SLEEP);

  wifiSetMode(WIFI_AP_STA);
  delay(1000);

  startAP(conf);

  wifiShutdown();

  digitalWrite(SETUP_LED, LOW);
  
  rlog_i("info", "Restart ESP");
  ESP.restart();
}

void sendData() {
  
  String curr_time = getCurrentTime();
  rlog_i("info", "Current time: %s", curr_time.c_str());
  rlog_i("info", "WiFi RSSI %d", WiFi.RSSI());
  rlog_i("info", "heap %d", ESP.getFreeHeap()>>10);
  rlog_i("info", "CPU freq %d", ESP.getCpuFreqMHz());
  
  data.voltage = (float)random(1000, 200000)/100.0;
  data.current = (float)random(1000, 200000)/100.f;
  data.power = (float)random(1000, 200000)/100.f;
  data.frequency = (float)random(1000, 200000)/100.f;

  calcExtraData(data, ext);
}

#ifdef USEWEB
uint8_t isFirmwareReady() {
  
  WiFiClient http;
  String ret;
  int ind_md5;

  if(http.connect(OTA_SERVER, OTA_PORT)) {
    String request("mac=");
    request += WiFi.macAddress();
    http.printf("GET "OTA_REQ"?%s HTTP/1.1\r\n", request.c_str());
    http.println("Host: "OTA_SERVER);
    http.println("User-agent: ESP8266/1.0");
    http.println("Connection: close");
    http.println();
    ret = http.readString();
    ind_md5 = ret.indexOf("md5:");
    ver = ret.substring(ret.indexOf("firmware:") + 9, ind_md5);
    ret = ret.substring(ind_md5 + 4);
  }
  http.stop();

  if(ret == "-1") {
    rlog_i("info", "OTA not found");
    return NO_UPDATE;
  } else if (ret == ESP.getSketchMD5()) {
    rlog_i("info", "OTA not need");
    return OTA_UPDATE_THE_SAME;
  } else if (ver != FIRMWARE_VERSION) {
    rlog_i("info", "OTA need");
    return OTA_UPDATE_READY;
  } else {
    rlog_i("info", "OTA need");
    return OTA_UPDATE_READY;
  }
#endif  
  
  rlog_i("info", "firmware=%s vs sketch=%s", ret.c_str(), ESP.getSketchMD5().c_str());
  return true;
}

void setup() {
  bool success = false;

  pinMode(SETUP_LED, OUTPUT);
  digitalWrite(SETUP_LED, LOW);

  Serial.begin(115200);
  Serial.println();
  rlog_i("info", "Boot ok");
  
  success = loadConfig(conf);
  rlog_i("info", "loadConfig = %d", success);
  if (!success) {
    rlog_i("info", "Setup board entering");
    setupBoard();
  }
 
  success = wifiConnect(conf);
  rlog_i("info", "WiFi connect = %d", success);

  if (!success) {
    rlog_i("info", "Setup board entering");
    setupBoard();
  }

#ifdef WIFI_DEBUG_INFO
  rlog_i("info", "WiFi debug info enabled");
  
  // wm.setDebugOutput(true);
  // wm.debugPlatformInfo();

  //reset settings - for testing
  // wm.resetSettings();
  // wm.erase();
#endif

  success = syncTime(conf);
  rlog_i("info", "sync_ntp_time = %d", success);
  
  #ifdef USEOTA
    ArduinoOTA.begin();
  #endif
  
  #ifdef USEWEB
    webActive = startWeb();
  #endif
}

bool flag = false;
uint32_t btnTimer = 0;
uint32_t mqttTimer = 0;
uint32_t statisticTimer = 0;
uint32_t measurementTimer = 0;
uint32_t stateTimer = 0;
uint32_t otaTimer = 0;
uint32_t secTimer = 0;

void loop() {
  bool success = false;

  #ifdef USEOTA
  ArduinoOTA.handle();
  #endif
  
  if((WiFi.status() == WL_CONNECTED)) {
  #ifdef USEWEB
    handleWeb();
  #endif
  }

  //button
	bool btnState = digitalRead(BUTTON);
  if (btnState && !flag && millis() - btnTimer > 100) {
    flag = true;
    btnTimer = millis();
    rlog_i("info", "PRESS");
  }
  if (btnState && flag && millis() - btnTimer > BTN_HOLD_SETUP) {
    btnTimer = millis();
    rlog_i("info", "PRESS HOLD");
    setupBoard();
  }
  if (!btnState && flag && millis() - btnTimer > BTN_CLICK) {
    btnTimer = millis();
#ifdef USEWEB
    if(webActive) {
      webActive = stopWeb();
    } else {
      webActive = startWeb();
    }
#endif
    rlog_i("info", "MAKE CLICK");
  }
  if (!btnState && flag && millis() - btnTimer > 100) {
    flag = false;
    btnTimer = millis();
    rlog_i("info", "RELEASE");
  }
  
  // timers
  // mqtt
  if (conf.mqtt_period && millis() - mqttTimer >= conf.mqtt_period * PER_MIN) {
    mqttTimer = millis();
    rlog_i("info", "timer MQTT");
    //sendData();
    flashLED();
  }
  // statistic
  if (conf.stat_period && millis() - statisticTimer >= conf.stat_period * PER_SEC) {
    statisticTimer = millis();
    //sendData();
    rlog_i("info", "timer Statistic");
    flashLED();
  }
  // measurement
  if (millis() - statisticTimer >= PER_MEASUREMENT) {
    statisticTimer = millis();
    //sendData();
    flashLED();
  }
  // state
  if (millis() - stateTimer >= PER_CHECK_STATE) {
    stateTimer = millis();
    flashLED();
  }
  // OTA check firmware
  if (millis() - otaTimer >= PER_CHECK_OTA) {
    otaTimer = millis();
#ifdef USEWEB
    if(webActive) {
      webActive = stopWeb();
      needOTA = isFirmwareReady();
      webActive = startWeb();
    } else {
      needOTA = isFirmwareReady();
    }
    rlog_i("info", "timer OTA ready=%d", needOTA);
#endif
    success = syncTime(conf);
    rlog_i("info", "sync_ntp_time=%d", success);
  }
  // OTA processing one sec timer
  if (millis() - secTimer >= 5 * PER_SEC) {
#ifdef USEWEB
    if(needOTA == OTA_UPDATE_FINISH) {
      ESP.restart();
    }
    if(needOTA == OTA_UPDATE_START) {
      startOTA();
      needOTA = OTA_UPDATE_FINISH;
    }
    secTimer = millis();
  }
#endif
}
