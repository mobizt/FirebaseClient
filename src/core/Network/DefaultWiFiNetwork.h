#ifndef CORE_NETWORK_DEFAULT_WIFI_NETWORK_H
#define CORE_NETWORK_DEFAULT_WIFI_NETWORK_H


#if !defined(DISABLE_NERWORKS)

#include <Arduino.h>
#include "./core/Network/DefaultNetwork.h"

class DefaultWiFiNetwork : public DefaultNetwork
{
public:
    /**
     * The WiFi network class with multiple APs supports.
     * @param wifi The FirebaseWiFi class object that used for keeping the network credentials (WiFi APs and WiFi passwords).
     * @param reconnect  The bool option for network reconnection. It should set with `false` when the WiFi reconnection was controlled by your code or third-party library e.g. WiFiManager.
     *
     * The FirebaseWiFi class holds the WiFi credentials list. The AP and password can be added to list with FirebaseWiFi::addAP.
     * The FirebaseWiFi object should be defined at the same usage scope of DefaultWiFiNetwork and AsyncClientClass.
     */
    explicit DefaultWiFiNetwork(FirebaseWiFi &wifi, bool reconnect = true)
    {
        init();
        network_data.wifi = &wifi;
        network_data.reconnect = reconnect;
        network_data.network_type = firebase_network_default;
    }
    ~DefaultWiFiNetwork() { clear(); }
};

#endif

#endif