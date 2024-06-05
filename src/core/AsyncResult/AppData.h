/**
 * Created June 5, 2024
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
#ifndef APP_DATA_H
#define APP_DATA_H

#include <Arduino.h>
#include <vector>

struct app_data_t
{
    friend class AsyncResult;
    friend class ResultBase;

protected:
    void reset(app_data_t &app_data) { app_data.reset(); }

private:
    struct data_item
    {
        bool read = false;
        unsigned long ts = 0;
    };

    std::vector<data_item> dataVec;
    uint32_t ms = 0, last_ms = 0;
    bool available = false;

    void update()
    {
        for (int i = dataVec.size() - 1; i >= 0; i--)
        {
            if (dataVec[i].read || millis() - dataVec[i].ts > 3000)
                dataVec.erase(dataVec.begin() + i);
        }
    }

    void limitQueue()
    {
        if (dataVec.size() > 5)
            dataVec.erase(dataVec.begin());
    }

    void setData()
    {
        limitQueue();
        ms = millis();
        data_item data;
        data.ts = ms;
        dataVec.push_back(data);
        available = true;
    }

    void reset()
    {
        dataVec.clear();
        available = false;
    }

    bool remaining() { return dataVec.size() && !dataVec[0].read; }

    bool isData()
    {
        if (available && last_ms < ms && ms > 0)
        {
            available = false;
            getData();
            return true;
        }

        // remaining to read?
        return remaining();
    }

    void getData()
    {
        last_ms = millis();
        if (dataVec.size())
            dataVec[0].read = true;
    }

public:
    // void clear() { reset(); }
};

#endif
