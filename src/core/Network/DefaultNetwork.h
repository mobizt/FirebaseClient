#ifndef CORE_NETWORK_DEFAULT_NETWORK_H
#define CORE_NETWORK_DEFAULT_NETWORK_H

#include <Arduino.h>
#include "./core/Network/Data/NetworkConfigData.h"

class DefaultNetwork
{
public:
    /**
     * The base class that provides the network data or default network class for built-in WiFi boards.
     *
     * @param reconnect The boolean option for network reconnection which applied for WiFi network only.
     */
    explicit DefaultNetwork(bool reconnect = true)
    {
        init();
        network_data.network_type = firebase_network_default;
        network_data.reconnect = reconnect;
    }
    ~DefaultNetwork() { clear(); }

    /**
     * Clear the internal network data.
     */
    void clear() { network_data.clear(); }

    /**
     * Get the reference to the internal network_config_data.
     *
     * @return network_config_data & The reference to internal network_config_data.
     */
    network_config_data &get() { return network_data; }

protected:
    network_config_data network_data;

    void init()
    {
        clear();
        network_data.initialized = true;
    }
};
#endif