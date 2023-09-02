#ifndef __DATA_H
#define __DATA_H
#include <Arduino.h>

struct Measurements {               //измеренные данные
    float voltage;
    float current;
    float power;
    float frequency;
    float energy;
    float pf;

    uint32_t impulses1;
    uint32_t impulses2;
};
struct Offset {                     //считанные mqtt после загрузки
    float energy1;
    float energy2;
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
    float maxpf;

    float minvoltage;
    float mincurrent;
    float minpower;
    float minfreq;
    float minpf;
};
struct Data {
    BoardConfig conf;
    Measurements data;
    Extra ext;
    Offset offset;
    Calculations calc;
};

#endif