/*
 * SPDX-FileCopyrightText: 2026 Suwatchai K. <suwatchai@outlook.com>
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef CORE_ASYNC_RESULT_APP_LOG_H
#define CORE_ASYNC_RESULT_APP_LOG_H

#include <Arduino.h>
#include <vector>

struct app_log_t
{
    friend class FirebaseError;
    friend class AsyncClientClass;
    friend class AppBase;
    friend class ConnBase;
    friend class JWTClass;
    friend class AsyncResult;

private:
    struct log_item
    {
        String msg;
        int code = 0;
        bool read = false;
        uint32_t ms = 0;
    };

    std::vector<log_item> logVec;
    uint16_t max_queue = 30, timeout = 10000;

public:
    void push_back(int code, const String &msg)
    {
        pop_front();

        if (code == 0 || (remaining() && code == -2 /* data_log */))
            return;

        log_item log;
        log.code = code;
        log.msg = msg;
        log.ms = millis();
        logVec.push_back(log);
    }
    void reset() { logVec.clear(); }
    bool remaining()
    {
        pop_front();
        return logVec.size() && !logVec[0].read;
    }
    bool isAvailable() { return remaining(); }
    void pop_front()
    {
        if (logVec.size() && (logVec[0].read || millis() - logVec[0].ms > timeout || logVec.size() > max_queue))
            logVec.erase(logVec.begin());
    }
    void read()
    {
        if (logVec.size())
            logVec[0].read = true;
    }
    String message()
    {
        if (logVec.size())
        {
            logVec[0].read = true;
            return logVec[0].msg;
        }
        return String();
    }
    int code()
    {
        if (logVec.size())
        {
            logVec[0].read = true;
            return logVec[0].code;
        }
        return 0;
    }
};

#endif
