#include "web.h"
#include "rlog.h"
#include "data.h"
#include <ESP8266WiFi.h> 
#include <ESP8266WebServer.h>

extern Measurements data;
extern Extra ext;
ESP8266WebServer server;   

void sendMessage(String &message) {

  message = F("{\"voltage\": ");
  message += String(data.voltage);
  message += F(", \"current\": ");
  message += String(data.current);
  message += F(", \"power\": ");
  message += String(data.power);
  message += F(", \"frequency\": ");
  message += String(data.frequency);
  message += F(", \"energy\": ");
  message += String(data.energy);
  message += F(", \"maxvoltage\": ");
  message += String(ext.maxvoltage);
  message += F(", \"maxcurrent\": ");
  message += String(ext.maxcurrent);
  message += F(", \"maxpower\": ");
  message += String(ext.maxpower);
  message += F(", \"maxfreq\": ");
  message += String(ext.maxfreq);
  message += F(", \"minvoltage\": ");
  message += String(ext.minvoltage);
  message += F(", \"mincurrent\": ");
  message += String(ext.mincurrent);
  message += F(", \"minpower\": ");
  message += String(ext.minpower);
  message += F(", \"minfreq\": ");
  message += String(ext.minfreq);
  message += F(", \"rssi\": ");
  message += String(WiFi.RSSI());
  message += F(", \"heap\": ");
  message += String(ESP.getFreeHeap()>>10);
  message += F(", \"freq\": ");
  message += String(ESP.getCpuFreqMHz());
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

void startWeb() {

  rlog_i("info", "WEB start");
  server.on("/", handleRoot);
  server.on("/states", handleStates);
  server.begin();
}

void stoptWeb() {

  rlog_i("info", "WEB stop");
  server.stop();
}

void handleWeb() {

  server.handleClient();
}
