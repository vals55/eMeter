#include "web.h"
#include "rlog.h"
#include "data.h"
#include "config.h"
#include "utils.h"
#include <ESP8266WiFi.h> 
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include <Ticker.h>

extern Measurements data;
extern Extra ext;
extern uint8_t needOTA;
extern uint8_t secTimer;
extern String ver;

WiFiClient client;
ESP8266WebServer server;   
const char* msg[]  = {
  "\"Нет новых обновлений.\"", 
  "\"Обновление уже установлено.\"",
  "\"Есть свежее обновление v. \"",
  "\"Загрузка обновления...\"",
  "\"Обновление загружено. Перезагрузка.\"",
  "\"Ошибка обновления.\""
};
//Ticker start_later;

void rebootBoard() {
  ESP.restart();
}

void updateStarted() {
  rlog_i("info", "CALLBACK: HTTP update process started");
  //needOTA = OTA_UPDATE_START;
}
 
void updateFinished() {
  rlog_i("info", "CALLBACK: HTTP update process finished");
  //needOTA = OTA_UPDATE_FINISH;
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
  
  t_httpUpdate_return ret = ESPhttpUpdate.update(client, "http://home.shokurov.ru/bin/firmware.bin");
  // t_httpUpdate_return ret = ESPhttpUpdate.update(client, "192.168.1.70", 3000, "/update", FIRMWARE_VERSION);
  switch (ret) {
    case HTTP_UPDATE_FAILED:
      rlog_i("info", "HTTP_UPDATE_FAILD Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
      rlog_i("info", "Retry in 10secs!");
      needOTA = OTA_UPDATE_ERROR;
      delay(10000); // Wait 10secs
      break;

    case HTTP_UPDATE_NO_UPDATES:
      rlog_i("info", "HTTP_UPDATE_NO_UPDATES");
      rlog_i("info", "Your code is up to date!");
      needOTA = NO_UPDATE;
      delay(10000); // Wait 10secs
      break;

    case HTTP_UPDATE_OK:
      rlog_i("info", "HTTP_UPDATE_OK");
      // delay(1000); // Wait a second and restart
      // ESP.restart();
      //needOTA = OTA_UPDATE_FINISH;
      //secTimer = millis();
      break;
  }
}

void sendMessage(String &message) {

  message = F("{\"inner-voltage\": ");
  message += String(data.voltage);
  message += F(", \"inner-current\": ");
  message += String(data.current);
  message += F(", \"inner-power\": ");
  message += String(data.power);
  message += F(", \"inner-frequency\": ");
  message += String(data.frequency);
  message += F(", \"inner-energy\": ");
  message += String(data.energy);
  message += F(", \"inner-maxvoltage\": ");
  message += String(ext.maxvoltage);
  message += F(", \"inner-maxcurrent\": ");
  message += String(ext.maxcurrent);
  message += F(", \"inner-maxpower\": ");
  message += String(ext.maxpower);
  message += F(", \"inner-maxfreq\": ");
  message += String(ext.maxfreq);
  message += F(", \"inner-minvoltage\": ");
  message += String(ext.minvoltage);
  message += F(", \"inner-mincurrent\": ");
  message += String(ext.mincurrent);
  message += F(", \"inner-minpower\": ");
  message += String(ext.minpower);
  message += F(", \"inner-minfreq\": ");
  message += String(ext.minfreq);
  message += F(", \"inner-rssi\": ");
  message += String(WiFi.RSSI());
  message += F(", \"inner-heap\": ");
  message += String(ESP.getFreeHeap()>>10);
  message += F(", \"inner-freq\": ");
  message += String(ESP.getCpuFreqMHz());
  message += F(", \"inner-firmware\": ");
  message += String(FIRMWARE_VERSION);
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
  rlog_i("info", "WEB message %s", message.c_str());
}

void handleStates() {

  rlog_i("info", "WEB /states request");
  String message;
  message.reserve(200);
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
  //start_later.attach(2, startOTA);
  //startOTA();
}

bool startWeb() {

  rlog_i("info", "WEB start");
  server.on("/", handleRoot);
  server.on("/states", handleStates);
  server.on("/update", handleUpdate);
  server.on("/load", handleLoad);
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
