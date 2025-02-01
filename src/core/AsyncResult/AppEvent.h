#ifndef CORE_ASYNC_RESULT_APP_EVENT_H
#define CORE_ASYNC_RESULT_APP_EVENT_H

#include <Arduino.h>
#include <vector>

namespace firebase_ns
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
        int current_code = 0, new_code = 0, last = 0;
        bool available = false, isEventChecked = false;
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
