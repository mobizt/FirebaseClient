#ifndef CORE_ASYNC_RESULT_APP_DATA_H
#define CORE_ASYNC_RESULT_APP_DATA_H

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
};

#endif
