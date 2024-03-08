#ifndef CORE_H_
#define CORE_H_

#include <Arduino.h>

static void sys_idle()
{
#if defined(ARDUINO_ESP8266_MAJOR) && defined(ARDUINO_ESP8266_MINOR) && defined(ARDUINO_ESP8266_REVISION) && ((ARDUINO_ESP8266_MAJOR == 3 && ARDUINO_ESP8266_MINOR >= 1) || ARDUINO_ESP8266_MAJOR > 3)
    esp_yield();
#else
    delay(0);
#endif
}

#endif