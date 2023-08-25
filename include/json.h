#ifndef __JSON_H
#define __JSON_H

#include <ArduinoJson.h>
#include "config.h"
#include "data.h"

#define JSON_BUFFER 1024

extern void getJSONData(const BoardConfig &conf, const Measurements &data, const Offset &offset, const Calculations & calc, DynamicJsonDocument &json_data);

#endif

