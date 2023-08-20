#include "web.h"
#include "rlog.h"
#include "data.h"
#include "config.h"
#include "utils.h"
#include <ESP8266WiFi.h> 
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include <ESP8266HTTPUpdateServer.h>

extern Measurements data;
extern Extra ext;
extern bool needOTA;
WiFiClient client;
ESP8266WebServer server;   
ESP8266HTTPUpdateServer httpUpdater;


void startOTA() {
  
  rlog_i("info", "OTA start OTA: ");
  httpUpdater.setup(&server);
  //server.begin();

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
  page += FPSTR(HTTP_SCRIPT1_MAIN);
  page += FPSTR(HTTP_SCRIPT2_MAIN);
  page += FPSTR(HTTP_STYLE_MAIN);
  page += FPSTR(HTTP_BODY_MAIN);
  rlog_i("info", "WEB root request");
  server.send(200, F("text/html"), page);
}

void handleUpdate() {
  String page = FPSTR(HTTP_HEADER_MAIN);
  page += FPSTR(HTTP_STYLE_UPDATE);
  page += FPSTR(HTTP_STYLE_MAIN);
  page += FPSTR(HTTP_BODY_UPDATE);
  rlog_i("info", "WEB /update request");
  server.sendHeader(F("Content-Encoding"), F("bin"));
  server.send(200, F("text/html"), page);
}

bool startWeb() {

  rlog_i("info", "WEB start");
  server.on("/", handleRoot);
  server.on("/states", handleStates);
  server.on("/update", handleUpdate);
  httpUpdater.setup(&server);
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
