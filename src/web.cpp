// Copyright 2023 vals. All rights reserved.
// SPDX-License-Identifier: MIT
#include "web.h"
#include "rlog.h"
#include "data.h"
#include "config.h"
#include "utils.h"
#include "sync_time.h"
#include <ESP8266WiFi.h> 
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include "buffer.h"
#include "json.h"

#define STOP_STATE_DEBUG

extern Data data;
extern uint8_t needOTA;
extern uint8_t secTimer;
extern String ver;
extern uint32_t start;
extern EEPROMBuff<BoardConfig> storage;

WiFiClient client;
ESP8266WebServer server;   
DynamicJsonDocument json_state(JSON_BUFFER);

void updateStarted() {
  rlog_i("info", "CALLBACK: HTTP update process started");
}
 
void updateFinished() {
  rlog_i("info", "CALLBACK: HTTP update process finished");
}
 
void updateProgress(int cur, int total) {
  rlog_i("info", "CALLBACK: HTTP update process at %d of %d bytes...", cur, total);
}
 
void updateError(int err) {
  rlog_i("info", "CALLBACK: HTTP update fatal error code %d", err);
  needOTA = OTA_UPDATE_ERROR;
}

void startOTA() {
  
  rlog_i("info", "OTA start OTA: ");

  ESPhttpUpdate.setLedPin(LED_BUILTIN, LOW);
  // ESPhttpUpdate.onStart(updateStarted);
  // ESPhttpUpdate.onEnd(updateFinished);
  // ESPhttpUpdate.onProgress(updateProgress);
  ESPhttpUpdate.onError(updateError);
  ESPhttpUpdate.rebootOnUpdate(false);
  
  t_httpUpdate_return ret = ESPhttpUpdate.update(client, OTA_SERVER, OTA_PORT, OTA_REQ, FIRMWARE_VERSION);
  switch (ret) {
    case HTTP_UPDATE_FAILED:
      rlog_i("info", "HTTP_UPDATE_FAILD Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
      rlog_i("info", "Retry in 10secs!");
      needOTA = OTA_UPDATE_ERROR;
      delay(10000); 
      break;

    case HTTP_UPDATE_NO_UPDATES:
      rlog_i("info", "HTTP_UPDATE_NO_UPDATES");
      rlog_i("info", "Your code is up to date!");
      needOTA = NO_UPDATES;
      delay(10000); 
      break;

    case HTTP_UPDATE_OK:
      rlog_i("info", "HTTP_UPDATE_OK");
      ESP.restart();
      break;
  }
}

void handleStates() {
#ifndef STOP_STATE_DEBUG
  rlog_i("info", "WEB /states request");
#endif
  getJSONState(data, json_state);
  String message = "";
  message.reserve(JSON_BUFFER);
  serializeJson(json_state, message);
#ifndef STOP_STATE_DEBUG
  rlog_i("web", "WEB message %s", message.c_str());
#endif  
server.send(200, F("text/plain"), message);
message.clear();
}

void handleRoot() {
  String page = FPSTR(HTTP_HEADER_MAIN);
  page += FPSTR(HTTP_SCRIPT_MAIN);
  page += FPSTR(HTTP_SCRIPT2_MAIN);
  page += FPSTR(HTTP_STYLE_MAIN);
  page += FPSTR(HTTP_BODY_MAIN);
  rlog_i("info", "WEB root request");
  server.send(200, F("text/html"), page);
}

void handleUpdate() {
  String page = FPSTR(HTTP_HEADER_MAIN);
  page += FPSTR(HTTP_SCRIPT_MAIN);
  page += FPSTR(HTTP_SCRIPT_UPDATE);
  page += FPSTR(HTTP_STYLE_UPDATE);
  page += FPSTR(HTTP_STYLE_MAIN);
  page += FPSTR(HTTP_BODY_UPDATE);
  rlog_i("info", "WEB /update request");
  server.send(200, F("text/html"), page);
}

void handleLoad() {

  rlog_i("info", "WEB /load request");
  secTimer = millis();
  needOTA = OTA_UPDATE_START;
}

void handleReset() {

  rlog_i("info", "WEB /reset request");
  storage.erase();
  ESP.restart();
}

void handleReboot() {

  rlog_i("info", "WEB /reboot request");
  ESP.restart();
}

bool startWeb() {

  rlog_i("info", "WEB start");
  server.on("/", handleRoot);
  server.on("/states", handleStates);
  server.on("/update", handleUpdate);
  server.on("/load", handleLoad);
  server.on("/reset", handleReset);
  server.on("/reboot", handleReboot);
  server.begin();
  return true;
}

bool stopWeb() {

  rlog_i("info", "WEB stop");
  server.stop();
  return false;
}

void handleWeb() {

  server.handleClient();
}
