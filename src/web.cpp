#include "web.h"
#include "rlog.h"
#include "data.h"
#include <ESP8266WiFi.h> 
#include <ESP8266WebServer.h>

extern Measurements data;
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
  message += F(", \"maxvoltage\": ");
  message += String(data.maxvoltage);
  message += F(", \"maxcurrent\": ");
  message += String(data.maxcurrent);
  message += F(", \"maxpower\": ");
  message += String(data.maxpower);
  message += F(", \"maxfreq\": ");
  message += String(data.maxfreq);
  message += F(", \"minvoltage\": ");
  message += String(data.minvoltage);
  message += F(", \"mincurrent\": ");
  message += String(data.mincurrent);
  message += F(", \"minpower\": ");
  message += String(data.minpower);
  message += F(", \"minfreq\": ");
  message += String(data.minfreq);
  message += F(", \"energy\": ");
  message += String(data.energy);
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

void handleWeb() {

  server.handleClient();
}
