#include <WiFiManager.h>

#include "setup.h"
#include "rlog.h"
#include "utils.h"
#include "porting.h"

WiFiManager wm;
bool WMISBLOCKING = true; // use blocking or non blocking mode, non global params wont work in non blocking

#define SETUP_TIME_SEC 600

// #define CALLBACK_NEED
#ifdef CALLBACK_NEED
void saveWifiCallback(){
  rlog_i("info", "[CALLBACK] saveWiFiCallback fired");
}

//gets called when WiFiManager enters configuration mode
void configModeCallback (WiFiManager *myWiFiManager) {
  rlog_i("info", "[CALLBACK] configModeCallback fired");
  // myWiFiManager->setAPStaticIPConfig(IPAddress(10,0,1,1), IPAddress(10,0,1,1), IPAddress(255,255,255,0)); 
  // rlog_i("info", WiFi.softAPIP());
  //if you used auto generated SSID, print it
  // rlog_i("info", myWiFiManager->getConfigPortalSSID());
  // 
  // esp_wifi_set_bandwidth(WIFI_IF_AP, WIFI_BW_HT20);
}

void saveParamCallback(){
  rlog_i("info", "[CALLBACK] saveParamCallback fired");
  // wm.stopConfigPortal();
}

void handleRoute(){
  rlog_i("info", "[HTTP] handle route");
  wm.server->send(200, "text/plain", "hello from user code");
}

void bindServerCallback(){
  wm.server->on("/custom", handleRoute); // this is now crashing esp32 for some reason
  // wm.server->on("/info",handleRoute); // you can override wm!
}

void handlePreOtaUpdateCallback(){
  Update.onProgress([](unsigned int progress, unsigned int total) {
    rlog_i("info", "CUSTOM Progress: %u%%\r", (progress / (total / 100)));
  });
}
#endif

void wifiInfo() {
  // can contain gargbage on esp32 if wifi is not ready yet
  rlog_i("info", "[WIFI] WIFI INFO DEBUG");
  // WiFi.printDiag(Serial);
  rlog_i("info", "[WIFI] SAVED: %s", (String)(wm.getWiFiIsSaved() ? "YES" : "NO"));
  rlog_i("info", "[WIFI] SSID: %s", (String)wm.getWiFiSSID());
  rlog_i("info", "[WIFI] PASS: %s", (String)wm.getWiFiPass());
  rlog_i("info", "[WIFI] HOSTNAME: %s", (String)WiFi.getHostname());
}

void startAP(Config &conf) {
#ifdef WIFI_DEBUG_INFO
  rlog_i("info", "WiFi debug info enabled");
  wm.setDebugOutput(true);
  wm.debugPlatformInfo();

  //reset settings - for testing
  // wm.resetSettings();
  // wm.erase();
#endif

#ifdef CALLBACK_NEED
  // callbacks
  wm.setAPCallback(configModeCallback);
  wm.setWebServerCallback(bindServerCallback);
  wm.setSaveConfigCallback(saveWifiCallback);
  wm.setSaveParamsCallback(saveParamCallback);
  wm.setPreOtaUpdateCallback(handlePreOtaUpdateCallback);
#endif  
  // std::vector<const char *> menu = {"wifi","info","param","sep","restart","exit"};
  std::vector<const char *> menu = {};
  wm.setMenu(menu);

  if (conf.ssid[0]) {
      struct station_config sconf;
      sconf.bssid_set = 0;
      memcpy(sconf.ssid, conf.ssid, sizeof(sconf.ssid));
      if (conf.password[0]) {
          memcpy(sconf.password, conf.password, sizeof(sconf.password));
      } else {
          sconf.password[0] = 0;
      }    
      wifi_station_set_config(&sconf);
  }

  WiFiManagerParameter subtitle_energy("<h3>Электроэнергия</h3>");
  wm.addParameter(&subtitle_energy);

  WiFiManagerParameter label_counter("<label class='label'>Показания счетчика, кВт*ч:</label>");
  wm.addParameter(&label_counter);
  FloatParameter param_counter_t0("counter_t0", "", conf.counter_t0);
  wm.addParameter(&param_counter_t0);

  WiFiManagerParameter label_coeff("<label class='label'>Коэффициент счетчика, имп./кВт*ч:</label>");
  wm.addParameter(&label_coeff);
  LongParameter param_coeff("coeff", "", conf.coeff);
  wm.addParameter(&param_coeff);

  WiFiManagerParameter label_send_period("<label class='label'>Период отправки показаний, мин.:</label>");
  wm.addParameter(&label_send_period);
  LongParameter param_send_period("send_period", "", conf.send_period);
  wm.addParameter(&param_send_period);

  WiFiManagerParameter subtitle_mqtt("<h3>MQTT</h3>");
  wm.addParameter(&subtitle_mqtt);

  WiFiManagerParameter label_broker("<label class='label'>Адрес брокера:</label>");
  wm.addParameter(&label_broker);
  WiFiManagerParameter param_mqtt_host("mqtt_host", "", conf.mqtt_host, MQTT_HOST_LEN-1);
  wm.addParameter(&param_mqtt_host);

  WiFiManagerParameter label_port("<label class='label'>Порт:</label>");
  wm.addParameter(&label_port);
  LongParameter param_mqtt_port("mqtt_port", "", conf.mqtt_port);
  wm.addParameter(&param_mqtt_port);

  WiFiManagerParameter label_login("<label class='label'>Логин:</label>");
  wm.addParameter(&label_login);
  WiFiManagerParameter param_mqtt_login("mqtt_login", "", conf.mqtt_login, MQTT_LOGIN_LEN-1);
  wm.addParameter(&param_mqtt_login);

  WiFiManagerParameter label_passw("<label class='label'>Пароль:</label>");
  wm.addParameter(&label_passw);
  WiFiManagerParameter param_mqtt_password("mqtt_password", "", conf.mqtt_password, MQTT_PASSWORD_LEN-1);
  wm.addParameter(&param_mqtt_password);

  WiFiManagerParameter label_topic("<label class='label'>Топик показаний:</label>");
  wm.addParameter(&label_topic);
  WiFiManagerParameter param_mqtt_topic("mqtt_topic", "", conf.mqtt_topic, MQTT_TOPIC_LEN-1);
  wm.addParameter(&param_mqtt_topic);

  WiFiManagerParameter label_ha_topic("<label class='label'>Топик Home Assistant:</label>");
  wm.addParameter(&label_ha_topic);
  WiFiManagerParameter param_mqtt_discovery_topic("mqtt_discovery_topic", "", conf.mqtt_discovery_topic, MQTT_TOPIC_LEN-1);
  wm.addParameter(&param_mqtt_discovery_topic);

  WiFiManagerParameter div_checkbox("<label class='chk-box'>Автоматическое добавление в Home Assistant");
  wm.addParameter(&div_checkbox);
  CheckBoxParameter param_mqtt_auto_discovery("mqtt_auto_discovery", "", conf.mqtt_auto_discovery);
  wm.addParameter(&param_mqtt_auto_discovery);
  WiFiManagerParameter div_end_checkbox("<span class='box'></span></label>");
  wm.addParameter(&div_end_checkbox);

//extra conf ------------------------------------------------------------------------------------
  WiFiManagerParameter checkbox("<label class='chk-box'>Расширенные настройки<input type='checkbox' id='chbox' name='chbox' onclick='extraConf()'><span class='box'></span></label>");
  wm.addParameter(&checkbox);
  WiFiManagerParameter div_start("<div id='extra_conf' style='display:none'>");
  wm.addParameter(&div_start);

  WiFiManagerParameter subtitle_network("<h3>Сетевые настройки</h3>");
  wm.addParameter(&subtitle_network);

  String mac("<label class='label'>MAC адрес: ");
  mac += WiFi.macAddress();
  mac += "</label><br /><br />";
  WiFiManagerParameter label_mac(mac.c_str());
  wm.addParameter(&label_mac);

  IPAddressParameter param_ip("ip", "Статический IP (оставьте 0.0.0.0 для DHCP):", IPAddress(conf.ip));
  wm.addParameter(&param_ip);
  IPAddressParameter param_gateway("gateway", "Шлюз:", IPAddress(conf.gateway));
  wm.addParameter(&param_gateway);
  IPAddressParameter param_mask("mask", "Маска подсети:", IPAddress(conf.mask));
  wm.addParameter(&param_mask);

  WiFiManagerParameter label_phy_mode("<label class='label'>Режим работы WiFi:</label>");
  wm.addParameter(&label_phy_mode);
  DropdownParameter dropdown_phy_mode("wifi_phy_mode");
  dropdown_phy_mode.add_option(0, "Авто", conf.wifi_phy_mode);
  dropdown_phy_mode.add_option(1, "11b",  conf.wifi_phy_mode);
  dropdown_phy_mode.add_option(2, "11g",  conf.wifi_phy_mode);
  dropdown_phy_mode.add_option(3, "11n",  conf.wifi_phy_mode);
  wm.addParameter(&dropdown_phy_mode);

  WiFiManagerParameter param_ntp_server("ntp_server", "Сервер времени (NTP):", conf.ntp_server, NTP_HOST_LEN - 1);
  wm.addParameter(&param_ntp_server);

  WiFiManagerParameter label_tz("<label class='label'>Часовой пояс (разница с UTC):</label>");
  wm.addParameter(&label_tz);
  LongParameter param_tz("tz", "", conf.tz);
  wm.addParameter(&param_tz);

//extra conf end---------------------------------------------------------------------------------
  WiFiManagerParameter div_end("</div>");
  wm.addParameter(&div_end);

// set custom channel
// wm.setWiFiAPChannel(13);
  
// set AP hidden
// wm.setAPHidden(true);

  if(!WMISBLOCKING){
    wm.setConfigPortalBlocking(false);
  }

  wm.setConfigPortalTimeout(SETUP_TIME_SEC);

  wm.setBreakAfterConfig(true); // needed to use saveWifiCallback

  // set custom webserver port, automatic captive portal does not work with custom ports!
  // wm.setHttpPort(8080);

  wm.startConfigPortal(getAppName().c_str());
  
#ifdef WIFI_DEBUG_INFO
   wifiInfo();
#endif  
    
  strncpy0(conf.ssid, wm.getWiFiSSID().c_str(), SSID_LEN);
  strncpy0(conf.password, wm.getWiFiPass().c_str(), PASSW_LEN);
  conf.counter_t0 = param_counter_t0.getValue();
  conf.coeff = param_coeff.getValue();
  conf.send_period = param_send_period.getValue();

  strncpy0(conf.mqtt_host, param_mqtt_host.getValue(), MQTT_HOST_LEN);
  conf.mqtt_port = param_mqtt_port.getValue();
  strncpy0(conf.mqtt_login, param_mqtt_login.getValue(), MQTT_LOGIN_LEN);
  strncpy0(conf.mqtt_password, param_mqtt_password.getValue(), MQTT_PASSWORD_LEN);
  strncpy0(conf.mqtt_topic, param_mqtt_topic.getValue(), MQTT_TOPIC_LEN);
  strncpy0(conf.mqtt_discovery_topic, param_mqtt_discovery_topic.getValue(), MQTT_TOPIC_LEN);
  conf.mqtt_auto_discovery = param_mqtt_auto_discovery.getValue();

  conf.ip = param_ip.getValue();
  if (conf.ip) {
    conf.gateway = param_gateway.getValue();
    conf.mask = param_mask.getValue();
  }
  conf.wifi_phy_mode = dropdown_phy_mode.getValue();
  strncpy0(conf.ntp_server, param_ntp_server.getValue(), NTP_HOST_LEN);
  conf.tz = param_tz.getValue();

  storeConfig(conf);
  rlog_i("info", "Config stored!");
}