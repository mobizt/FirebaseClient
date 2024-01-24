#ifndef CORE_TIMER_H
#define CORE_TIMER_H

#include <Arduino.h>
#include "./Config.h"

class Timer
{
private:
    unsigned long ts = 0;
    unsigned long end = 0;
    unsigned long period = 0;
    unsigned long now = 0;
    unsigned long ms = 0;
    bool enable = false;

public:
    Timer(unsigned long sec = 60) { setInterval(sec); }
    ~Timer(){};

    void reset()
    {
        end = ts + period;
    }

    void start()
    {
        enable = true;
        loop();
        reset();
    }

    void stop() { enable = false; }

    void setInterval(unsigned long sec)
    {
        loop();
        period = sec;
        reset();
    }

    void loop()
    {
        if (enable && (unsigned long)(millis() - now) > 100)
        {
            now = millis();

            if (now / 1000 >= ts)
            {
                ts = now / 1000;
                ms = now;
            }
            else // millis overflow handling
                ts += (unsigned long)(now - ms) / 1000;
        }
    }

    unsigned long remaining()
    {
        return ready() ? 0 : end - ts;
    }

    bool ready()
    {
        loop();
        return ts >= end;
    }
};

#endif