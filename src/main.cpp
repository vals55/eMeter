#include <Arduino.h>
#include <Ticker.h>
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
#define BUTTON 5
#define BTN_HOLD_SETUP 5000

void sendData();

Ticker work_cycle;
Config conf;
Measurements data;
Extra ext;

void setupBoard() {
  
  work_cycle.detach();
  WiFi.persistent(false);
  WiFi.disconnect();
  WiFi.setSleepMode(WIFI_NONE_SLEEP);

  wifiSetMode(WIFI_AP_STA);
  delay(1000);

  startAP(conf);

  wifiShutdown();
  
  rlog_i("info", "Restart ESP");
  ESP.restart();
}

void sendData() {
  
  String curr_time = getCurrentTime();
  rlog_i("info", "Current time: %s", curr_time.c_str());
  
  data.voltage = (float)random(1000, 200000)/100.0;
  data.current = (float)random(1000, 200000)/100.f;
  data.power = (float)random(1000, 200000)/100.f;
  data.frequency = (float)random(1000, 200000)/100.f;

  calcExtraData(data, ext);
}

void setup() {
  boolean success = false;

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
  
  work_cycle.attach(conf.send_period, sendData);
  
  #ifdef USEOTA
    ArduinoOTA.begin();
  #endif

  #ifdef USEWEB
    startWeb();
  #endif

}

bool flag = false;
uint32_t btnTimer = 0;

void loop() {

  #ifdef USEOTA
  ArduinoOTA.handle();
  #endif
  #ifdef USEWEB
    handleWeb();
  #endif

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
  if (!btnState && flag && millis() - btnTimer > 100) {
    flag = false;
    btnTimer = millis();
    rlog_i("info", "RELEASE");
  }
}
