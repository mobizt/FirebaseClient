#ifndef CORE_NETWORK_GENERIC_NETWORK_H
#define CORE_NETWORK_GENERIC_NETWORK_H


#if !defined(DISABLE_NERWORKS)

#include <Arduino.h>
#include "./core/Network/DefaultNetwork.h"

/**
 * The network class works for all networking interfaces e.g. Ethernet Module, WiFI Module, Bluetooth Module, NB-IoT module, and LoRa Module that have the internet access ability.
 *
 * @param networkConnectionCB The network connection callback function.
 * @param networkStatusCB The network status callback function.
 *
 * In the networkConnectionCB function, the complete operations for the carier (network) and internet connection should be performed and waits until the internet connection was established.
 * In the networkStatusCB function, the `status` (Boolean variable) that provided in the function, should set with the network status.
 *
 * See /examples/App/NetworkInterfaces/Async/Callback/GenericNetwork for using WiFi with GenericNetwork for demonstation.
 *
 */
class GenericNetwork : public DefaultNetwork
{
public:
    explicit GenericNetwork(NetworkConnectionCallback networkConnectionCB, NetworkStatusCallback networkStatusCB)
    {
        init();
        network_data.generic.net_con_cb = networkConnectionCB;
        network_data.generic.net_status_cb = networkStatusCB;
        network_data.network_type = firebase_network_generic;
    }
    ~GenericNetwork() { clear(); }
};

#endif

#endif