#ifndef CORE_NETWORK_DEFAULT_ETHERNET_NETWORK_H
#define CORE_NETWORK_DEFAULT_ETHERNET_NETWORK_H


#if !defined(DISABLE_NERWORKS)

#include <Arduino.h>
#include "./core/Network/DefaultNetwork.h"

class DefaultEthernetNetwork : public DefaultNetwork
{
public:
    /**
     * The default Ethernet class for ESP8266 with lwIP Ethernet.
     *
     * @param eth The ESP8266 core SPI ethernet driver class that work with external SPI Ethernet modules that currently supported e.g. ENC28J60, Wiznet W5100 and Wiznet 5500.
     *
     * This eth should be defined at the same usage scope of AsyncClientCalss.
     *
     * To use ESP8266 native lwIP Ethernet, the one of following macros, `ENABLE_ESP8266_ENC28J60_ETH`, `ENABLE_ESP8266_W5500_ETH`
     * and `ENABLE_ESP8266_W5100_ETH` should be defined in the sketch before including the library header file `FirebaseClient.h`
     * or adding `ENABLE_ESP8266_ENC28J60_ETH`, `ENABLE_ESP8266_W5500_ETH` and `ENABLE_ESP8266_W5100_ETH` in the compiler build flags.
     *
     * Use Firebase_SPI_ETH_Module::enc28j60, Firebase_SPI_ETH_Module::w5100 and Firebase_SPI_ETH_Module::w5500 to assign the pointer to
     * ENC28J60lwIP, Wiznet5100lwIP and Wiznet5500lwIP classes objects respectively.
     *
     * In PlatformIO IDE, please set the 'lib_ldf_mode = chain+' option in platformio.ini.
     *
     * See /examples/App/NetworkInterfaces/Async/Callback/DefaultNetworks/DefaultEthernetNetwork/ESP8266/ for using ESP8266 with its native lwIP Ethernet library.
     */
    explicit DefaultEthernetNetwork(Firebase_SPI_ETH_Module &eth)
    {
        init();
#if defined(FIREBASE_LWIP_ETH_IS_AVAILABLE) && defined(ENABLE_ETHERNET_NETWORK)
        network_data.eth = &eth;
#else
        (void)eth;
#endif
        network_data.network_type = firebase_network_default;
    }

    /**
     * The default Ethernet class for ESP32 with native Ethernet boards.
     * See /examples/App/NetworkInterfaces/Async/Callback/DefaultNetworks/DefaultEthernetNetwork/ESP32/ for using ESP32 with its native Ethernet library.
     */
    DefaultEthernetNetwork()
    {
        init();
        network_data.network_type = firebase_network_default;
    }
    ~DefaultEthernetNetwork() { clear(); }
};
#endif

#endif