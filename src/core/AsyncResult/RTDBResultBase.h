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
#ifndef RTDB_RESULT_BASE_H_
#define RTDB_RESULT_BASE_H_

#include <Arduino.h>
#include "./core/AsyncResult/AsyncResult.h"

namespace firebase
{
    class RTDBResultBase: public RealtimeDatabaseResult
    {
        friend class AsyncResult;

    public:
        RTDBResultBase() {}

    protected:
        void setNullETagOption(RealtimeDatabaseResult *rtdbResult, bool val)
        {
            rtdbResult->null_etag = val;
        }

        bool getNullETagOption(const RealtimeDatabaseResult *rtdbResult)
        {
            return rtdbResult->null_etag;
        }

        void setRefPayload(RealtimeDatabaseResult *rtdbResult, String *payload)
        {
            rtdbResult->ref_payload = payload;
        }
    };

}

#endif