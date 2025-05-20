#ifndef CORE_NETWORK_DATA_SPI_ETHERNET_H
#define CORE_NETWORK_DATA_SPI_ETHERNET_H

#include <Arduino.h>
#include "./core/Network/NetworkOptions.h"

typedef struct firebase_spi_ethernet_module_t
{
#if defined(ESP8266) && defined(ESP8266_CORE_SDK_V3_X_X)
#ifdef INC_ENC28J60_LWIP
    ENC28J60lwIP *enc28j60 = nullptr;
#endif
#ifdef INC_W5100_LWIP
    Wiznet5100lwIP *w5100 = nullptr;
#endif
#ifdef INC_W5500_LWIP
    Wiznet5500lwIP *w5500 = nullptr;
#endif

#endif

} Firebase_SPI_ETH_Module;
#endif