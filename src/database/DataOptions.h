/**
 * Created January 29, 2024
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
#ifndef DATABASE_DATAOPTIONS_H
#define DATABASE_DATAOPTIONS_H

#include <Arduino.h>
#include "./Config.h"
#include "./database/Filter.h"


#if defined(ENABLE_DATABASE)


class DataOptions
{
    friend class Database;

public:
    uint32_t readTimeout = 0;
    String writeSizeLimit;
    bool shallow = false;
    bool silent = false;
    bool classicRequest = false;
    String customHeaders;
    String ETAG;
    Filter filter;

    void copy(DataOptions &rhs)
    {
        this->readTimeout = rhs.readTimeout;
        this->writeSizeLimit = rhs.writeSizeLimit;
        this->shallow = rhs.shallow;
        this->silent = rhs.silent;
        this->classicRequest = rhs.classicRequest;
        this->customHeaders = rhs.customHeaders;
        this->ETAG = rhs.ETAG;
        this->filter.copy(rhs.filter);
        this->ota = rhs.ota;
        this->base64 = rhs.base64;
    }

    void clear()
    {
        readTimeout = 0;
        writeSizeLimit.clear();
        shallow = false;
        silent = false;
        classicRequest = false;
        customHeaders.clear();
        ETAG.clear();
        filter.clear();
    }

private:
    bool base64 = false;
    bool ota = false;
};

#endif

#endif