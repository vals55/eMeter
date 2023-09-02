#ifndef __JSON_H
#define __JSON_H

#include <ArduinoJson.h>
#include "config.h"
#include "data.h"

#define JSON_BUFFER 1024
#define JSON_SMALL_BUFFER 256

extern void getJSONData(const Data &data, DynamicJsonDocument &json_data);

#endif

