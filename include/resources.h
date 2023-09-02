#ifndef __RESOURCES_H
#define __RESOURCES_H

#include <Arduino.h>

#define MQTT_PARAM_COUNT 8
#define CHANNEL_COUNT 2
#define NONE -1

static const char s_sensor[] PROGMEM    = "sensor";
static const char s_number[] PROGMEM    = "number";
static const char s_float[] PROGMEM     = "float";
// class
static const char c_total[] PROGMEM         = "total";
static const char c_measurement[] PROGMEM   = "measurement";
// device
static const char d_current[] PROGMEM   = "current";
static const char d_energy[] PROGMEM    = "energy";
static const char d_power[] PROGMEM     = "power";
static const char d_pf[] PROGMEM        = "power_factor";
static const char d_voltage[] PROGMEM   = "voltage";
static const char d_freq[] PROGMEM      = "frequency";
static const char d_rssi[] PROGMEM      = "signal_strength";
static const char d_timestamp[] PROGMEM = "timestamp";
static const char d_duration[] PROGMEM  = "duration";
static const char d_none[] PROGMEM      = "None";
// names & units
static const char n_voltage[] PROGMEM   = "Voltage";
static const char u_voltage[] PROGMEM   = "V";
static const char n_current[] PROGMEM   = "Current";
static const char u_current[] PROGMEM   = "A";
static const char n_power[] PROGMEM     = "Power";
static const char u_power[] PROGMEM     = "W";
static const char n_freq[] PROGMEM      = "Frequency";
static const char u_freq[] PROGMEM      = "Hz";
static const char n_pf[] PROGMEM        = "Power factor";
static const char u_pf[] PROGMEM        = "None";
static const char n_energy[] PROGMEM    = "Energy";
static const char u_energy[] PROGMEM    = "kWh";
static const char n_rssi[] PROGMEM      = "RSSI";
static const char u_rssi[] PROGMEM      = "dBm";
static const char n_time[] PROGMEM      = "Last seen";
static const char n_mac[] PROGMEM       = "MAC Address";
static const char n_chip[] PROGMEM      = "Chip";
static const char n_t0[] PROGMEM        = "Energy T0";
static const char u_t0[] PROGMEM        = "kWh";
static const char n_t1[] PROGMEM        = "Energy T1";
static const char u_t1[] PROGMEM        = "kWh";
static const char n_mqtt_per[] PROGMEM  = "MQTT send period";
static const char u_min[] PROGMEM       = "min";
static const char n_stat_per[] PROGMEM  = "Statistics send period";
static const char u_sec[] PROGMEM       = "s";
static const char n_imp[] PROGMEM       = "Impulses";
static const char n_ip[] PROGMEM        = "IP Address";
// ids
static const char i_voltage[] PROGMEM   = "voltage";
static const char i_current[] PROGMEM   = "current";
static const char i_power[] PROGMEM     = "power";
static const char i_freq[] PROGMEM      = "frequency";
static const char i_pf[] PROGMEM        = "power_factor";
static const char i_energy[] PROGMEM    = "energy";
static const char i_rssi[] PROGMEM      = "rssi";
static const char i_time[] PROGMEM      = "timestamp";
static const char i_mac[] PROGMEM       = "mac";
static const char i_chip[] PROGMEM      = "chip";
static const char i_t0[] PROGMEM        = "energy1";
static const char i_t1[] PROGMEM        = "energy2";
static const char i_mqtt_per[] PROGMEM  = "mqtt_period";
static const char i_stat_per[] PROGMEM  = "stat_period";
static const char i_energy0[] PROGMEM   = "energy0";
static const char i_imp0[] PROGMEM      = "imp0";
static const char i_current0[] PROGMEM  = "current0";
static const char i_power0[] PROGMEM    = "power00";
static const char i_ip[] PROGMEM        = "ip";
// category
static const char cat_diag[] PROGMEM = "diagnostic";
static const char cat_conf[] PROGMEM = "config";
// icons
static const char icon[] PROGMEM = "mdi:counter";

static const char *const GENERAL_ENTITIES[][MQTT_PARAM_COUNT] PROGMEM = {
    // type,    name,       sensor_id,  state_class,    dev_class,   unit,       cat,       icon
    // single sensors
    {s_sensor,  n_voltage,  i_voltage,  c_measurement,  d_voltage,   u_voltage,  "",        icon}, 
    {s_sensor,  n_current,  i_current,  c_measurement,  d_current,   u_current,  "",        icon},
    {s_sensor,  n_power,    i_power,    c_measurement,  d_power,     u_power,    "",        icon},
    {s_sensor,  n_freq,     i_freq,     c_measurement,  d_freq,      u_freq,     "",        icon},
    {s_sensor,  n_pf,       i_pf,       c_measurement,  d_pf,        u_pf,       "",        icon},
    {s_sensor,  n_time,     i_time,     "",             d_timestamp, "",         cat_diag,  icon},
    {s_float,   n_t0,       i_t0,       "",             "",          u_t0,       cat_conf,  icon},
    {s_float,   n_t1,       i_t1,       "",             "",          u_t1,       cat_conf,  icon},
    {s_number,  n_mqtt_per, i_mqtt_per, "",             d_duration,  u_min,      cat_conf,  icon},
    {s_number,  n_stat_per, i_stat_per, "",             d_duration,  u_sec,      cat_conf,  icon},
    {s_sensor,  n_rssi,     i_rssi,     c_measurement,  d_rssi,      u_rssi,     cat_diag,  icon},
    {s_sensor,  n_ip,       i_ip,       "",             "",          "",         cat_diag,  icon},
    {s_sensor,  n_mac,      i_mac,      "",             "",          "",         cat_diag,  ""},
    {s_sensor,  n_chip,     i_chip,     "",             "",          "",         cat_diag,  ""}
};

static const char *const CHANNEL_ENTITIES[][MQTT_PARAM_COUNT] PROGMEM = {
    // type,    name,       sensor_id,  state_class,    dev_class,   unit,       cat,       icon
    // sensors with attrs
    {s_sensor, n_t0,        i_energy0,  c_total,        d_energy,    u_energy,   "",        icon},
    {s_sensor, n_imp,       i_imp0,     c_measurement,  "",          "",         cat_diag,  icon},
    {s_sensor, n_current,   i_current0, c_measurement,  d_current,   u_current,  cat_diag,  icon},
    {s_sensor, n_power,     i_power0,   c_measurement,  d_power,     u_power,    cat_diag,  icon}
};

static const char s_energy_t0[] PROGMEM = "Energy T0";
static const char s_energy_t1[] PROGMEM = "Energy T1";
static const char *const CHANNEL_NAMES[CHANNEL_COUNT] PROGMEM = {s_energy_t0, s_energy_t1};

static const char s_classic[] PROGMEM   = "Classic";
static const char s_mod[] PROGMEM       = "Lite";
static const char *const MODEL_NAMES[] PROGMEM = {s_classic, s_mod};

#endif