#ifndef _DATA_H
#define _DATA_H
#include <Arduino.h>

struct Measurements {
    float voltage;
    float current;
    float power;
    float frequency;

    float maxvoltage;
    float maxcurrent;
    float maxpower;
    float maxfreq;

    float minvoltage;
    float mincurrent;
    float minpower;
    float minfreq;

    float energy;
    uint32_t impulses;
    float offset_energy;
    uint32_t offset_imp;
};

#endif