#ifndef _DATA_H
#define _DATA_H
#include <Arduino.h>

struct Measurements {
    float voltage;
    float current;
    float power;
    float frequency;
    float energy;

    uint32_t impulses;
};
struct Offset {
    float energy;
    uint32_t impulses;
};
struct Calculations {
    float voltage;
    float current;
    float power;
    float energy;
};
struct Extra {
    float maxvoltage;
    float maxcurrent;
    float maxpower;
    float maxfreq;

    float minvoltage;
    float mincurrent;
    float minpower;
    float minfreq;
};

#endif