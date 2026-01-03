/*
 * SPDX-FileCopyrightText: 2026 Suwatchai K. <suwatchai@outlook.com>
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef CORE_ASYNC_RESULT_APP_PROGRESS_H
#define CORE_ASYNC_RESULT_APP_PROGRESS_H

#include <Arduino.h>
#include <vector>

struct app_progress_t
{
    friend class AsyncResult;
    friend class download_data_t;
    friend class upload_data_t;
    friend class AsyncClientClass;
    friend class SlotManager;

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
    bool isProgress(bool isUpdate)
    {
        if (available && last_ms < ms && ms > 0)
        {
            if (isUpdate)
                available = false;
            getProgress(isUpdate);
            return true;
        }

        // remaining to read?
        return false;
    }
    void getProgress(bool isUpdate)
    {
        if (isUpdate)
        {
            last_ms = millis();
            if (progressVec.size())
                progressVec[0].read = true;
        }
    }
};

#endif
