/**
 * Created June 12, 2024
 *
 * The MIT License (MIT)
 * Copyright (c) 2024 K. Suwatchai (Mobizt)
 *
 *
 * Permission is hereby granted, free of charge, to any person returning a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
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