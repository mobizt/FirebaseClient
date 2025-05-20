#ifndef CORE_NETWORK_DATA_STATIC_ADDRESS_H
#define CORE_NETWORK_DATA_STATIC_ADDRESS_H

#include <Arduino.h>

typedef struct firebase_client_static_address
{
    friend class AsyncClientClass;
    friend class network_config_data;
    friend class conn_handler;

public:
    /**
     * The Firebase Client Static address provider class.
     *
     * @param ipAddress The static IP.
     * @param netMask The subnet IP.
     * @param defaultGateway The default gateway IP.
     * @param dnsServer The dns server IP.
     * @param optional The boolean option to force use static IP only (not use DHCP).
     */
    explicit firebase_client_static_address(IPAddress ipAddress, IPAddress netMask, IPAddress defaultGateway, IPAddress dnsServer, bool optional)
    {
        this->ipAddress = ipAddress;
        this->netMask = netMask;
        this->defaultGateway = defaultGateway;
        this->dnsServer = dnsServer;
        this->optional = optional;
    };

    firebase_client_static_address() {}

private:
    IPAddress ipAddress, netMask, defaultGateway, dnsServer;
    bool optional = false;
} Firebase_StaticIP;
#endif