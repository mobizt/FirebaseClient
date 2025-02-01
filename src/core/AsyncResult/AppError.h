#ifndef CORE_ASYNC_RESULT_APP_ERROR_H
#define CORE_ASYNC_RESULT_APP_ERROR_H

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
    int current_code = 0, new_code = 0, last = 0;
    bool available = false, isErrorChecked = false;
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
