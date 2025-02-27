#ifndef CORE_DEBUG_H
#define CORE_DEBUG_H
#include <Arduino.h>
#include "./core/AsyncResult/AsyncResult.h"
static void firebase_bebug_callback(AsyncResultCallback cb, AsyncResult &res, const char *func, int line, const char *file)
{
    // Serial.printf("######## Func: %s, File: %s, Line: %d\n", func, file, line);
    if (cb)
        cb(res);
}
#endif