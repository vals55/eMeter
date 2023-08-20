#include "web.h"
#include "rlog.h"
#include "data.h"
#include "config.h"
#include "utils.h"
#include <ESP8266WiFi.h> 
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>

extern Measurements data;
extern Extra ext;
extern bool needOTA;
bool needStartOTA = false;
WiFiClient client;
ESP8266WebServer server;   

void updateStarted() {
  rlog_i("info", "CALLBACK: HTTP update process started");
  //strncpy0(upd.status, "Обновление началось", 20);
}
 
void updateFinished() {
  rlog_i("info", "CALLBACK: HTTP update process finished");
  //strncpy0(upd.status, "Завершено. Перезапуск.", 23);
  needOTA = false;
  needStartOTA = false;
}
 
void updateProgress(int cur, int total) {
  rlog_i("info", "CALLBACK: HTTP update process at %d of %d bytes...", cur, total);
  String message;
  message.reserve(100);
  message = F("{\"style-progress\": ");
  char buffer[64];
  sprintf(buffer, "\"width:%d%%\"", 100 * cur / total);
  message += String(buffer);
  message += F(", \"inner-status\": ");
  sprintf(buffer, "\"Скачано %d из %d\"", cur, total);
  message += String(buffer);
  message += F("}");
  server.send(200, F("text/plain"), message);
}
 
void updateError(int err) {
  rlog_i("info", "CALLBACK: HTTP update fatal error code %d", err);
  //strncpy0(upd.status,"Ошибка обновления.", 18);
}

void startOTA() {
  
  rlog_i("info", "OTA start OTA: ");

  ESPhttpUpdate.setLedPin(LED_BUILTIN, LOW);
  ESPhttpUpdate.onStart(updateStarted);
  ESPhttpUpdate.onEnd(updateFinished);
  ESPhttpUpdate.onProgress(updateProgress);
  ESPhttpUpdate.onError(updateError);
  ESPhttpUpdate.rebootOnUpdate(false);
  
  t_httpUpdate_return ret = ESPhttpUpdate.update(client, "http://home.shokurov.ru/bin/firmware.bin");
  // t_httpUpdate_return ret = ESPhttpUpdate.update(client, "192.168.1.70", 3000, "/update", FIRMWARE_VERSION);
  switch (ret) {
    case HTTP_UPDATE_FAILED:
      rlog_i("info", "HTTP_UPDATE_FAILD Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
      rlog_i("info", "Retry in 10secs!");
      delay(10000); // Wait 10secs
      break;

    case HTTP_UPDATE_NO_UPDATES:
      rlog_i("info", "HTTP_UPDATE_NO_UPDATES");
      rlog_i("info", "Your code is up to date!");
        delay(10000); // Wait 10secs
      break;

    case HTTP_UPDATE_OK:
      rlog_i("info", "HTTP_UPDATE_OK");
      delay(1000); // Wait a second and restart
      ESP.restart();
      break;
  }
}

char buffer[32] = {0};

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
  message += F(", \"style-check\": ");
  String buffer = needOTA ? "\"display:block\"" : "\"display:none\"";
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

  rlog_i("info", "WEB root request");
  server.send(200, F("text/html"), FPSTR(HTTP_MAIN));
}

void handleUpdate() {

  rlog_i("info", "WEB /update request");
  startOTA();
}

bool startWeb() {

  rlog_i("info", "WEB start");
  server.on("/", handleRoot);
  server.on("/states", handleStates);
  server.on("/update", handleUpdate);
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
  if(needOTA && needStartOTA) {
    startOTA();
  }
}
