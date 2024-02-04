/**
 * Created February 4, 2024
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
#ifndef ASYNC_CLOUD_STORAGE_H
#define ASYNC_CLOUD_STORAGE_H
#include <Arduino.h>
#include "./core/FirebaseApp.h"
using namespace std;

using namespace firebase;

#if defined(ENABLE_CLOUD_STORAGE)

class CloudStorage
{
private:
    AsyncClient *aClient = nullptr;
    String service_url;
    String path;
    String uid;
    uint32_t app_addr = 0;
    app_token_t *app_token = nullptr;

public:
    ~CloudStorage(){};
    CloudStorage(const String &url = "")
    {
        this->service_url = url;
    };

    CloudStorage &operator=(CloudStorage &rhs)
    {
        this->service_url = rhs.service_url;
        return *this;
    }

    /**
     * Set the database URL
     * @param url The CloudStorage URL.
     */
    void url(const String &url)
    {
        this->service_url = url;
    }

    void setApp(uint32_t app_addr, app_token_t *app_token)
    {
        this->app_addr = app_addr;
        this->app_token = app_token;
    }

    app_token_t *appToken()
    {
        List list;
        return list.existed(firebase_app_list, app_addr) ? app_token : nullptr;
    }

    /**
     * Perform the async task repeatedly.
     * Should be places in main loop function.
     */
    void loop()
    {
        for (size_t clientSlot = 0; clientSlot < firebase_client_list.size(); clientSlot++)
        {
            AsyncClient *aClient = reinterpret_cast<AsyncClient *>(firebase_client_list[clientSlot]);
            if (aClient)
            {
                for (size_t slot = 0; slot < aClient->slotCount(); slot++)
                {
                    aClient->process(true);
                }
                aClient->handleRemove();
            }
        }
    }
};

#endif

#endif