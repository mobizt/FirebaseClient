#ifndef CORE_JSON_H
#define CORE_JSON_H

#include <Arduino.h>
#include <Client.h>
#include "./Config.h"
#include "./core/Memory.h"

class JSON
{
private:
    /* data */
public:
    JSON(){};
    ~JSON(){};
    void addObject(String &buf, const String &name, const String &value, bool last = false)
    {
        if (buf.length() == 0)
            buf += "{";
        else
            buf += ",";
        buf += name;
        buf += ":";
        buf += value;
        if (last)
            buf += "}";
    }
    void addArray(String &buf, const String &value, bool last = false)
    {
        if (buf.length() == 0)
            buf += "[";
        else
            buf += ",";
        buf += value;
        if (last)
            buf += "]";
    }
    String toString(const String &value)
    {
        String buf = "\"";
        buf += value;
        buf += "\"";
        return buf;
    }
};

#endif