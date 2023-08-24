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

void getData();

BoardConfig conf;
Measurements data;
Extra ext;
#ifdef USEWEB
uint8_t needOTA = OTA_UPDATE_THE_SAME;
bool webActive = false;
String ver;
#endif

SoftwareSerial pzemSWSerial(PZEM_RX_PIN, PZEM_TX_PIN);
PZEM004Tv30 pzem(pzemSWSerial);

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

void getData() {
  
  String curr_time = getCurrentTime();
  
  float voltage = pzem.voltage();
  float current = pzem.current();
  float power = pzem.power();
  float energy = pzem.energy();
  float frequency = pzem.frequency();
  float pf = pzem.pf();

  data.voltage = isnan(voltage) ? 0.0 : round(voltage * 10)/10;
  data.current = isnan(current) ? 0.0 : round(current * 10)/10;
  data.power = isnan(power) ? 0.0 : round(power * 10)/10;
  data.energy = isnan(energy) ? 0.0 : round(energy * 10)/10;
  data.frequency = isnan(frequency) ? 0.0 : round(frequency * 10)/10;
  data.pf = isnan(pf) ? 0.0 : round(pf * 100)/100;

  rlog_i("info", "Address: %04x", pzem.readAddress());
  rlog_i("info", "Voltage: %.1f", data.voltage);
  rlog_i("info", "Current: %.1f", data.current);
  rlog_i("info", "Power: %.1f", data.power);
  rlog_i("info", "Energy: %.1f", data.energy);
  rlog_i("info", "Freq: %.1f", data.frequency);
  rlog_i("info", "pf: %.2f", data.pf);

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
    http.printf("GET " OTA_REQ "?%s HTTP/1.1\r\n", request.c_str());
    http.println("Host: " OTA_SERVER);
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
    //getData();
    flashLED();
  }
  // statistic
  if (conf.stat_period && millis() - statisticTimer >= conf.stat_period * PER_SEC) {
    statisticTimer = millis();
    //getData();
    rlog_i("info", "timer Statistic");
    flashLED();
  }
  // measurement
  if (millis() - statisticTimer >= PER_MEASUREMENT) {
    statisticTimer = millis();
    getData();
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
