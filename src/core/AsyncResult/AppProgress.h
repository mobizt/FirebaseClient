/**
 * Created June 2, 2024
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
#ifndef APP_PROGRESS_H
#define APP_PROGRESS_H

#include <Arduino.h>
#include <vector>

struct app_progress_t
{
    friend class AsyncResult;
    friend class ResultBase;
    friend class download_data_t;
    friend class upload_data_t;
    friend class AsyncClientClass;

protected:
    void reset(app_progress_t &app_progress) { app_progress.reset(); }

private:
    struct progress_item
    {
        bool read = false;
        unsigned long ts = 0;
    };

    std::vector<progress_item> progressVec;
    uint32_t ms = 0, last_ms = 0;
    int value = -1;
    bool available = false;

    void update()
    {
        for (int i = progressVec.size() - 1; i >= 0; i--)
        {
            if (progressVec[i].read || millis() - progressVec[i].ts > 3000)
                progressVec.erase(progressVec.begin() + i);
        }
    }

    void limitQueue()
    {
        if (progressVec.size() > 5)
            progressVec.erase(progressVec.begin());
    }

    void setProgress(int value)
    {
        limitQueue();
        ms = millis();
        if (this->value != value)
        {
            progress_item progress;
            progress.ts = ms;
            progressVec.push_back(progress);
            available = true;
        }

        this->value = value;
    }

    void reset()
    {
        progressVec.clear();
        value = -1;
        available = false;
    }

    bool remaining() { return progressVec.size() && !progressVec[0].read; }

    bool isProgress()
    {
        if (available && last_ms < ms && ms > 0)
        {
            available = false;
            getProgress();
            return true;
        }

        // remaining to read?
        return false;
    }

    void getProgress()
    {
        last_ms = millis();
        if (progressVec.size())
            progressVec[0].read = true;
    }

public:
    // void clear() { reset(); }
};

#endif
