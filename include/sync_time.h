// Copyright 2023 vals. All rights reserved.
// SPDX-License-Identifier: MIT
#ifndef __SYNCTIME_H
#define __SYNCTIME_H

#include "config.h"

bool syncTime(BoardConfig &conf);
String getCurrentTime();
String getLocalTime();
void getUpTime(uint32_t &start, char* buf);
#endif

