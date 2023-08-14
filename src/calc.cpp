#include "calc.h"
#include "data.h"

extern Measurements data;
extern Extra ext;

void calcExtraData(const Measurements &data, Extra &ext) {
  
  ext.maxvoltage = max(ext.maxvoltage, data.voltage);
  ext.maxcurrent = max(ext.maxcurrent, data.current);
  ext.maxpower = max(ext.maxpower, data.power);
  ext.maxfreq = max(ext.maxfreq, data.frequency);

  if(!ext.minvoltage) ext.minvoltage = data.voltage;
  if(!ext.mincurrent) ext.mincurrent = data.current;
  if(!ext.minpower) ext.minpower = data.power;
  if(!ext.minfreq) ext.minfreq = data.frequency;

  ext.minvoltage = min(ext.minvoltage, data.voltage);
  ext.mincurrent = min(ext.mincurrent, data.current);
  ext.minpower = min(ext.minpower, data.power);
  ext.minfreq = min(ext.minfreq, data.frequency);
}