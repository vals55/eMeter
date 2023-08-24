#include "sync_time.h"
#include <time.h>
#include "config.h"

#define NTP_ATTEMPTS 24
#define START_VALID_TIME 1577826000UL   // Wed Jan 01 2020 00:00:00
#define TIME_FORMAT "%FT%T%z"

bool syncTime(BoardConfig &conf) {
  
  configTime(conf.tz*3600, 0, conf.ntp_server);
  
  int attempts = 0;
  time_t now = time(nullptr);
  while(now < START_VALID_TIME) {
    yield();
    now = time(nullptr);
    attempts++;
    if(attempts > NTP_ATTEMPTS) {
      return false;
    }
    delay(500);
  };
  return true;
}

String getCurrentTime() {
  
  char buf[100];
  time_t now = time(nullptr);
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  strftime(buf, sizeof(buf), TIME_FORMAT, &timeinfo);
  return String(buf);
}
