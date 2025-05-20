#ifndef CORE_DEBUG_H
#define CORE_DEBUG_H
#include <Arduino.h>
#include "./core/AsyncResult/AsyncResult.h"
inline void firebase_bebug_callback(AsyncResultCallback cb, AsyncResult &res, const char *func, int line, const char *file)
{
#if defined(ENABLE_CORE_DEBUG)
    Serial.printf("######## Func: %s, File: %s, Line: %d\n", func, file, line);
#else
    (void)res;
    (void)func;
    (void)line;
    (void)file;
#endif
    if (cb)
        cb(res);
}
#endif