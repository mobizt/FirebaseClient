#ifndef CORE_NETWORK_DEFAULT_PPP_NETWORK_H
#define CORE_NETWORK_DEFAULT_PPP_NETWORK_H

#include <Arduino.h>
#include "./core/Network/DefaultNetwork.h"

class DefaultPPPNetwork : public DefaultNetwork
{
public:
    /**
     * The default PPP network class for ESP32 with native PPP ptotocols supports.
     * See /examples/App/NetworkInterfaces/Async/Callback/DefaultNetworks/DefaultPPPNetwork/ESP32/ for using ESP32 with its native PPP library.
     */
    DefaultPPPNetwork()
    {
        init();
        network_data.network_type = firebase_network_default;
    }
    ~DefaultPPPNetwork() { clear(); }
};
#endif