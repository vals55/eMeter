#ifndef __DATA_H
#define __DATA_H
#include <Arduino.h>

struct Measurements {
    float voltage;
    float current;
    float power;
    float frequency;
    float energy;
    float pf;

    uint32_t impulses1;
    uint32_t impulses2;
};
struct Offset {
    float energy;
    uint32_t impulses1;
    uint32_t impulses2;
};
struct Calculations {
    float voltage = 220.0;
    float current1;
    float power1;
    float energy1;
    float current2;
    float power2;
    float energy2;
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