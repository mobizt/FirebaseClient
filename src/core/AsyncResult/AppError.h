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
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO Error SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#ifndef APP_ERROR_H
#define APP_ERROR_H

#include <Arduino.h>
#include <vector>

struct app_error_t
{
    friend class FirebaseError;

private:
    struct err_item
    {
        String msg;
        int code = 0;
        bool read = false;
        unsigned long ts = 0;
    };

    std::vector<err_item> errVec;
    uint32_t ms = 0, last_ms = 0;
    int current_code = 0, new_code = 0;
    bool available = false;
    bool isErrorChecked = false;
    int last = 0;
    err_item err;

    void update()
    {
        for (int i = errVec.size() - 1; i >= 0; i--)
        {
            if (errVec[i].read || millis() - errVec[i].ts > 30000)
                errVec.erase(errVec.begin() + i);
        }

        if (errVec.size() == 0)
            last = 0;
    }

    void limitQueue()
    {
        if (errVec.size() > 5)
            errVec.erase(errVec.begin());
    }

    void setError(int code, const String &msg)
    {

        if (code == 0 || code == last)
            return;

        last = code;
        limitQueue();

        ms = millis();
        err_item errt;
        errt.code = code;
        errt.msg = msg;
        errt.ts = ms;
        errVec.push_back(errt);
        available = true;
    }

    void reset()
    {
        errVec.clear();
        available = false;
        last = 0;
    }

    bool remaining() { return errVec.size() && !errVec[0].read; }

    bool isError()
    {
        isErrorChecked = true;
        current_code = new_code;
        if (available && last_ms < ms && ms > 0)
        {
            available = false;
            new_code = getError().code;
            if (errVec.size() && millis() - errVec[0].ts > 3000)
                errVec.erase(errVec.begin());
            return new_code != current_code;
        }

        // remaining to read?
        return remaining();
    }

    err_item getError()
    {
        last_ms = millis();

        if (errVec.size())
            errVec[0].read = true;

        return errVec.size() ? errVec[0] : err;
    }

public:
    String message() { return getError().msg; }

    int code()
    {
        // The workaround when isError() was not executed.
        if (!isErrorChecked)
        {
            isErrorChecked = true;
            return 0;
        }

        return getError().code;
    }

    bool avail() { return available; }

    bool remain() { return remaining(); }
};

#endif
