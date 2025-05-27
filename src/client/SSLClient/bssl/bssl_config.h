#ifndef BSSL_CONFIG_H
#define BSSL_CONFIG_H

// Only used in bssl
#include <Arduino.h>
#if (defined(ESP8266) || defined(ARDUINO_ARCH_RP2040)) && !defined(ARDUINO_NANO_RP2040_CONNECT)
#define USE_EMBED_SSL_ENGINE
#else
#define USE_LIB_SSL_ENGINE
#endif

// workaround when these macros 
// are previousely defined.
#if defined(MAX)
#undef MAX
#endif

#if defined(MIN)
#undef MIN
#endif


#endif