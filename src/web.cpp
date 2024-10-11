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

#define STOP_STATE_DEBUG

extern Data data;
extern uint8_t needOTA;
extern uint8_t secTimer;
extern String ver;
extern uint32_t start;

WiFiClient client;
ESP8266WebServer server;   
const char* msg[]  = {
  "\"Обновления не найдены.\"", 
  "\"Обновление не требуется.\"",
  "\"Есть новое обновление v. \"",
  "\"Установка обновления...\"",
  "\"Обновление загружено. Перезагрузка.\"",
  "\"Ошибка обновления.\""
};
char heap[10];
uint32_t bytes;
float kBytes;
char uptime[20];

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

void sendMessage(String &message) {

  message = F("{\"inner-voltage\": ");
  message += String(data.data.voltage);
  message += F(", \"inner-current\": ");
  message += String(data.data.current);
  message += F(", \"inner-power\": ");
  message += String(data.data.power);
  message += F(", \"inner-energy0\": ");
  message += String(data.data.energy);          //+data.offset.energy0);
  message += F(", \"inner-frequency\": ");
  message += String(data.data.frequency);
  message += F(", \"inner-pf\": ");
  message += String(data.data.pf);
  message += F(", \"inner-energy1\": ");
  message += String(data.calc.energy1+data.offset.energy1);
  message += F(", \"inner-energy2\": ");
  message += String(data.calc.energy2+data.offset.energy2);
  message += F(", \"inner-maxvoltage\": ");
  message += String(data.ext.maxvoltage);
  message += F(", \"inner-maxcurrent\": ");
  message += String(data.ext.maxcurrent);
  message += F(", \"inner-maxpower\": ");
  message += String(data.ext.maxpower);
  message += F(", \"inner-maxfreq\": ");
  message += String(data.ext.maxfreq);
  message += F(", \"inner-maxpf\": ");
  message += String(data.ext.maxpf);
  message += F(", \"inner-minvoltage\": ");
  message += String(data.ext.minvoltage);
  message += F(", \"inner-mincurrent\": ");
  message += String(data.ext.mincurrent);
  message += F(", \"inner-minpower\": ");
  message += String(data.ext.minpower);
  message += F(", \"inner-minfreq\": ");
  message += String(data.ext.minfreq);
  message += F(", \"inner-minpf\": ");
  message += String(data.ext.minpf);
  message += F(", \"inner-rssi\": ");
  message += String(WiFi.RSSI());
  message += F(", \"inner-heap\": ");
  bytes = ESP.getFreeHeap();
  kBytes = bytes / 1000.0f;
  sprintf(heap, "%.03f", kBytes);
  message += F("\"");
  message += heap;
  message += F("\"");
  message += F(", \"inner-freq\": ");
  message += String(ESP.getCpuFreqMHz());
  message += F(", \"inner-firmware\": ");
  message += String(FIRMWARE_VERSION);
  message += F(", \"inner-uptime\": ");
  getUpTime(start, uptime);
  message += F("\"");
  message += uptime;
  message += F("\"");
  message += F(", \"inner-msg\": ");
  message += String(msg[needOTA]);
  message += F(", \"inner-ver\": ");
  String buffer = needOTA == OTA_UPDATE_READY ? ver : "\"\"";
  message += String(buffer);
  message += F(", \"style-loader\": ");
  buffer = needOTA > 2 ? "\"display:inline-block\"" : "\"display:none\"";
  message += String(buffer);
  message += F(", \"oncli-btn-upd\": ");
  buffer = needOTA > 1 ? "\"upd();\"" : "\"history.back();\"";
  message += String(buffer);
  message += F(", \"inner-btn-upd\": ");
  buffer = needOTA > 1 ? "\"Обновить\"" : "\"Назад\"";
  message += String(buffer);
  message += F("}");
#ifndef STOP_STATE_DEBUG
  rlog_i("web", "WEB message %s", message.c_str());
#endif  
}

void handleStates() {
#ifndef STOP_STATE_DEBUG
  rlog_i("info", "WEB /states request");
#endif
  String message;
  message.reserve(690);
  sendMessage(message);
  server.send(200, F("text/plain"), message);
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
