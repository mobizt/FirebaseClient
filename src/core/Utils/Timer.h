/*
 * SPDX-FileCopyrightText: 2026 Suwatchai K. <suwatchai@outlook.com>
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef CORE_UTILS_TIMER_H
#define CORE_UTILS_TIMER_H

#include <Arduino.h>

class Timer
{
private:
    unsigned long ts = 0, end = 0, period = 0, now = 0, ms = 0;
    bool enable = false;
    uint8_t feed_count = 0;

public:
    explicit Timer(unsigned long sec = 60) { setInterval(sec); }

    ~Timer() {}

    void reset() { end = ts + period; }

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

    void feed(unsigned long sec)
    {
        feed_count++;
        if (sec == 0 || feed_count == 0)
            feed_count = 1;
        stop();
        setInterval(sec);
        start();
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

    unsigned long remaining() { return ready() ? 0 : end - ts; }

    uint8_t feedCount() const { return feed_count; }

    bool isRunning() const { return enable; };
    
    bool ready()
    {
        loop();
        return ts >= end;
    }
};
#endif