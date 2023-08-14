#ifndef SETUP_H_
#define SETUP_H_

#include <Arduino.h>
#include <WiFiManager.h>
#include "config.h"

void startAP(Config &conf);

class LongParameter : public WiFiManagerParameter {
public:
    LongParameter(const char *id, const char *placeholder, long value, const uint8_t length = 10) : WiFiManagerParameter("") {
        init(id, placeholder, String(value).c_str(), length, " type=\"number\"", WFM_LABEL_BEFORE);
    }
    long getValue() {
        return String(WiFiManagerParameter::getValue()).toInt();
    }
};

class ShortParameter : public WiFiManagerParameter {
public:
    ShortParameter(const char *id, const char *placeholder, short value, const uint8_t length = 10) : WiFiManagerParameter("") {
        init(id, placeholder, String(value).c_str(), length, " type=\"number\"", WFM_LABEL_BEFORE);
    }
    short getValue() {
        return String(WiFiManagerParameter::getValue()).toInt();
    }
};

class FloatParameter : public WiFiManagerParameter {
public:
    FloatParameter(const char *id, const char *placeholder, float value, const uint8_t length = 10) : WiFiManagerParameter("") {
        init(id, placeholder, String(value, 3).c_str(), length, " type=\"number\" step=\"0.01\" placeholder=\"0,00\"", WFM_LABEL_BEFORE);
    }
    float getValue() {
        String val(WiFiManagerParameter::getValue());
        val.replace(",", ".");
        return val.toFloat();
    }
};

class IPAddressParameter : public WiFiManagerParameter {
public:
    IPAddressParameter(const char *id, const char *placeholder, IPAddress address) : WiFiManagerParameter("") {
        String val("0.0.0.0"); //иначе (IP Unset) и не проходит проверка pattern
        if (address.isSet()) {
            val = address.toString();
        }
        init(id, placeholder, val.c_str(), 16, "minlength=\"7\" maxlength=\"15\" size=\"15\" pattern=\"^((\\d{1,2}|1\\d\\d|2[0-4]\\d|25[0-5])\\.){3}(\\d{1,2}|1\\d\\d|2[0-4]\\d|25[0-5])$\"", WFM_LABEL_BEFORE);
    }
    IPAddress getValue() {
        IPAddress ip;
        ip.fromString(WiFiManagerParameter::getValue());
        return ip;
    }
};

class DropdownParameter : public WiFiManagerParameter {
public:
    String options;
    DropdownParameter(const char *id) : WiFiManagerParameter(id, "", true, String(0).c_str(), 10) {
        options.reserve(200);
    }
    void add_option(const int value, const char *title, const int default_value) {
        options += "<option ";
        if (value == default_value) {
            options += "selected ";
        }
        options += "value=\'";
        options += value;
        options += "\'>";
        options += title;
        options += "</option>";

        setCustomHtml(options.c_str());
    }
    uint8_t getValue() {
        return String(WiFiManagerParameter::getValue()).toInt();
    }
};

class EmailParameter : public WiFiManagerParameter {
public:
    EmailParameter(const char *id, const char *label, const char *defaultValue, const uint8_t length = 50) : WiFiManagerParameter("") {
        init(id, label, defaultValue, length, " type=\"email\" pattern=\"[^@\\s]+@[^@\\s]+\\.[^@\\s]+\"", WFM_LABEL_BEFORE);
    }
};

class CheckBoxParameter : public WiFiManagerParameter {
public:
    CheckBoxParameter(const char *id, const char *label, bool default_value) : WiFiManagerParameter("") {
        if (default_value) {
            init(id, label, "T", 2, " type=\"checkbox\" checked", WFM_NO_LABEL);
        } else {
            init(id, label, "T", 2, " type=\"checkbox\"", WFM_NO_LABEL);
        }

    }
    bool getValue() {
        return strncmp(WiFiManagerParameter::getValue(), "T", 1) == 0;
    }
};

#endif