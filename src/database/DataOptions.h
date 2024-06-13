/**
 * Created March 13, 2024
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
#include "./database/DatabaseFilter.h"

#if defined(ENABLE_DATABASE)

class DatabaseOptions
{
    friend class RealtimeDatabase;

public:
    uint32_t readTimeout = 0;
    String writeSizeLimit;
    bool shallow = false;
    bool silent = false;
    bool classicRequest = false;
    DatabaseFilter filter;

    void copy(const DatabaseOptions &rhs)
    {
        this->readTimeout = rhs.readTimeout;
        this->writeSizeLimit = rhs.writeSizeLimit;
        this->shallow = rhs.shallow;
        this->silent = rhs.silent;
        this->classicRequest = rhs.classicRequest;
        this->filter.copy(rhs.filter);
        this->ota = rhs.ota;
        this->base64 = rhs.base64;
    }

    void clear()
    {
        readTimeout = 0;
        writeSizeLimit.remove(0, writeSizeLimit.length());
        shallow = false;
        silent = false;
        classicRequest = false;
        filter.clear();
    }

private:
    bool base64 = false;
    bool ota = false;
};

#endif

#endif