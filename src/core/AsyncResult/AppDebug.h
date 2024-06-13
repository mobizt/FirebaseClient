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
#ifndef APP_DEBUG_H
#define APP_DEBUG_H

#include <Arduino.h>
#include <vector>

namespace firebase
{
    struct app_debug_t
    {
        friend class AppBase;
        friend class ResultBase;
        friend class JWTClass;

    private:
        struct dbg_item
        {
            String msg;
            bool read = false;
            unsigned long ts = 0;
        };

        std::vector<dbg_item> dbgVec;
        uint32_t ms = 0, last_ms = 0;
        String current_msg, new_msg;
        bool available = false;
        bool isDebugChecked = false;
        String last;
        dbg_item dbg;

        void update()
        {
            for (int i = dbgVec.size() - 1; i >= 0; i--)
            {
                if (dbgVec[i].read || millis() - dbgVec[i].ts > 30000)
                    dbgVec.erase(dbgVec.begin() + i);
            }

            if (dbgVec.size() == 0 && last.length())
                last.remove(0, last.length());
        }

        void limitQueue()
        {
            if (dbgVec.size() > 3)
                dbgVec.erase(dbgVec.begin());
        }

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

        void reset()
        {
            dbgVec.clear();
            available = false;
            last.remove(0, last.length());
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
        String message()
        {
            return getDebug().msg;
        }
    };
}
#endif
