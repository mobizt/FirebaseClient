#ifndef CORE_ASYNC_RESULT_APP_DEBUG_H
#define CORE_ASYNC_RESULT_APP_DEBUG_H

#include <Arduino.h>
#include <vector>
#include "./core/Utils/StringUtil.h"

namespace firebase_ns
{
    struct app_debug_t
    {
        friend class AppBase;
        friend class ResultBase;
        friend class JWTClass;
        friend class ConnBase;

    protected:
        void setDebug(const String &msg)
        {

            if (msg.length() == 0 || strcmp(last.c_str(), msg.c_str()) == 0)
                return;

            limitQueue();

            last = msg;
            ms = millis();
            dbg_item dbgt;
            dbgt.msg = msg;
            dbgt.ts = ms;
            dbgVec.push_back(dbgt);
            available = true;
        }

    private:
        StringUtil sut;
        struct dbg_item
        {
            String msg;
            bool read = false;
            unsigned long ts = 0;
        };

        std::vector<dbg_item> dbgVec;
        uint32_t ms = 0, last_ms = 0;
        String current_msg, new_msg, last;
        bool available = false, isDebugChecked = false;
        dbg_item dbg;

        void update()
        {
            for (int i = dbgVec.size() - 1; i >= 0; i--)
            {
                if (dbgVec[i].read || millis() - dbgVec[i].ts > 30000)
                    dbgVec.erase(dbgVec.begin() + i);
            }

            if (dbgVec.size() == 0 && last.length())
                sut.clear(last);
        }

        void limitQueue()
        {
            if (dbgVec.size() > 3)
                dbgVec.erase(dbgVec.begin());
        }

        void reset()
        {
            dbgVec.clear();
            available = false;
            sut.clear(last);
        }

        bool remaining() { return dbgVec.size() && !dbgVec[0].read && dbgVec[0].msg.length(); }

        bool isDebug()
        {
            isDebugChecked = true;
            current_msg = new_msg;

            if (available && last_ms < ms && ms > 0)
            {
                available = false;
                new_msg = getDebug().msg;
                return new_msg.length() && strcmp(current_msg.c_str(), new_msg.c_str()) != 0;
            }

            // remaining to read?
            return remaining();
        }

        dbg_item getDebug()
        {
            last_ms = millis();

            if (dbgVec.size())
                dbgVec[0].read = true;

            return dbgVec.size() ? dbgVec[0] : dbg;
        }

    public:
        String message() { return getDebug().msg; }
    };
}
#endif
