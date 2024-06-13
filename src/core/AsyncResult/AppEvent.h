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
#ifndef APP_EVENT_H
#define APP_EVENT_H

#include <Arduino.h>
#include <vector>

namespace firebase
{
    struct app_event_t
    {
        friend class AppBase;
        friend class ResultBase;

    private:
        struct ev_item
        {
            String msg;
            int code = 0;
            bool read = false;
            unsigned long ts = 0;
        };

        std::vector<ev_item> evntVec;
        uint32_t ms = 0, last_ms = 0;
        int current_code = 0, new_code = 0;
        bool available = false;
        bool isEventChecked = false;
        int last = 0;
        ev_item ev;

        void update()
        {
            for (int i = evntVec.size() - 1; i >= 0; i--)
            {
                if (evntVec[i].read || millis() - evntVec[i].ts > 30000)
                    evntVec.erase(evntVec.begin() + i);
            }

            if (evntVec.size() == 0)
                last = 0;
        }

        void limitQueue()
        {
            if (evntVec.size() > 5)
                evntVec.erase(evntVec.begin());
        }

        void setEvent(int code, const String &msg)
        {

            if (code == 0 || last == code)
                return;

            last = code;

            limitQueue();
            ms = millis();
            ev_item evt;
            evt.code = code;
            evt.msg = msg;
            evt.ts = ms;
            evntVec.push_back(evt);
            available = true;
        }

        void reset()
        {
            evntVec.clear();
            available = false;
            last = 0;
        }

        bool remaining() { return evntVec.size() && !evntVec[0].read; }

        bool isEvent()
        {
            isEventChecked = true;
            current_code = new_code;
            if (available && last_ms < ms && ms > 0)
            {
                available = false;
                new_code = getEvent().code;
                return new_code != current_code;
            }

            // remaining to read?
            return remaining();
        }

        ev_item getEvent()
        {
            last_ms = millis();

            if (evntVec.size())
                evntVec[0].read = true;

            return evntVec.size() ? evntVec[0] : ev;
        }

    public:
        String message() { return getEvent().msg; }

        int code()
        {
            // The workaround when isEvent() was not executed.
            if (!isEventChecked)
            {
                isEventChecked = true;
                return 0;
            }

            return getEvent().code;
        }
    };
}
#endif
