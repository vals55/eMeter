#ifndef __HTTP_HELPER_H
#define __HTTP_HELPER_H

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "config.h"
#include "json.h"
#include "rlog.h"
#include "utils.h"

#define HTTP_SEND_ATTEMPTS 3
#define SERVER_TIMEOUT 10000

bool postData(String &url, String &payload) {
  void *pClient = nullptr;
  HTTPClient httpClient;
  bool result = false;
  
  rlog_i("info", "HTTP: JSON POST request");

  String proto = getProtocol(url);
  rlog_i("info", "HTTP: JSON POST protocol=%s", proto);
  if (proto == PROTO_HTTP) {
    rlog_i("info", "HTTP: JSON POST Create insecure client");
    pClient = new WiFiClient;
  } else if (proto == PROTO_HTTPS) {
    rlog_i("info", "HTTP: JSON POST Create secure client");
    pClient = new WiFiClientSecure;
    (*(WiFiClientSecure *)pClient).setInsecure();
  }

  httpClient.setTimeout(SERVER_TIMEOUT);
  httpClient.setReuse(false);

  if (httpClient.begin(*(WiFiClient *)pClient, url)) {
    httpClient.addHeader("Content-Type", "application/json");

    int response_code = httpClient.POST(payload);
    rlog_i("info", "HTTP: JSON POST Response code: %d", response_code);
    result = response_code == 200;
    String response_body = httpClient.getString();
    rlog_i("info", "HTTP: JSON POST Response body: %s", response_body.c_str());
    httpClient.end();
    (*(WiFiClient *)pClient).stop();
  }
  delete (WiFiClient*)pClient;
  return result;
}

bool sendHTTP(const BoardConfig &conf, const DynamicJsonDocument &json_data) {

  if (!isStat(conf)) {
    rlog_i("info", "HTTP: skip stat");
    return false;
  }

  uint32_t start_time = millis();

  rlog_i("info", "HTTP: start stat");

  String payload = "";
  serializeJson(json_data, payload);
  String url = conf.stat_host;
  rlog_i("info", "HTTP: payload: %s", payload.c_str());

  int attempts = HTTP_SEND_ATTEMPTS;
  bool result = false;
  do {
    rlog_i("info", "HTTP: send attempt %d of %d", HTTP_SEND_ATTEMPTS - attempts + 1, HTTP_SEND_ATTEMPTS);
    result = postData(url, payload);
  } while (!result && --attempts);

  if (result) {
    rlog_i("info", "HTTP: Data sent. Time %d ms", millis() - start_time);
  } else {
     rlog_i("info", "HTTP: Failed send. Time %d ms", millis() - start_time);
  }
  return result;
}
#endif

