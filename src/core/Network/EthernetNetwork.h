
#ifndef CORE_NETWORK_ETHERNET_NETWORK_H
#define CORE_NETWORK_ETHERNET_NETWORK_H

#include <Arduino.h>
#include "./core/Network/DefaultNetwork.h"

#if defined(FIREBASE_ETHERNET_MODULE_IS_AVAILABLE) && defined(ENABLE_ETHERNET_NETWORK)
class EthernetNetwork : public DefaultNetwork
{
public:
    /**
     * The Ethernet network class for generic Ethernet modules.
     *
     * @param macAddress The mac address.
     * @param csPin The Ethernet module chip select/enable pin.
     * @param resetPin The Ethernet module reset pin. Assign -1 if not used.
     */
    explicit EthernetNetwork(uint8_t macAddress[6], int csPin, int resetPin)
    {
        init();
        network_data.ethernet.setMac(macAddress);
        network_data.ethernet.setCs(csPin);
        network_data.ethernet.setReset(resetPin);
        network_data.network_type = firebase_network_ethernet;
    }

    /**
     * The Ethernet network class for generic Ethernet modules.
     *
     * @param macAddress The mac address.
     * @param csPin The Ethernet module chip select/enable pin.
     * @param resetPin The Ethernet module reset pin. Assign -1 if not used.
     * @param staticIP TheFirebase_StaticIP object that holds the static ip configuration.
     *
     * The Firebase_StaticIP class constructor parameters for static IP are following.
     * @param ipAddress The static IP.
     * @param netMask The subnet IP.
     * @param defaultGateway The default gateway IP.
     * @param dnsServer The dns server IP.
     * @param optional The boolean option to force use static IP only (not use DHCP).
     *
     * By default the external Ethernet module can be used with the library when the macro ENABLE_ETHERNET_NETWORK was defined and Ethernet library was included in the user sketch.
     * The user defined Ethernet class and header other than `Ethernet.h` and `Ethernet` can be used, see https://github.com/mobizt/FirebaseClient?tab=readme-ov-file#library-build-options.
     */
    explicit EthernetNetwork(uint8_t macAddress[6], int csPin, int resetPin, const Firebase_StaticIP &staticIP)
    {
        init();
        network_data.ethernet.setMac(macAddress);
        network_data.ethernet.setCs(csPin);
        network_data.ethernet.setReset(resetPin);
        network_data.ethernet.setStaticIP(staticIP);
        network_data.network_type = firebase_network_ethernet;
    }
    ~EthernetNetwork() { clear(); }
};
#endif
#endif