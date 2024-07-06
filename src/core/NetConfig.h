/**
 * Created July 6, 2024
 *
 * The MIT License (MIT)
 * Copyright (c) 2024 K. Suwatchai (Mobizt)
 *
 *
 * Permission is hereby granted, free of charge, to any person returning a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#ifndef CORE_NET_CONFIG_H
#define CORE_NET_CONFIG_H

#include <Arduino.h>
#include "./Config.h"
#include "./core/Network.h"
#include "./core/Memory.h"

#define FIREBASE_NET_RECONNECT_TIMEOUT_SEC 10

static unsigned long wifi_reconnection_ms = 0;
static uint32_t gsm_network_owner_addr = 0;
static uint32_t ethernet_network_owner_addr = 0;
static uint32_t generic_network_owner_addr = 0;

struct network_config_data
{
    friend class DefaultNetwork;
    friend class GenericNetwork;
    friend class EthernetNetwork;
    friend class GSMNetwork;
    friend class DefaultEthernetNetwork;
    friend class DefaultWiFiNetwork;
    friend class AsyncClientClass;

private:
    struct generic_data
    {
        NetworkConnectionCallback net_con_cb = NULL;
        NetworkStatusCallback net_status_cb = NULL;

    public:
        void copy(const generic_data &rhs)
        {
            this->net_con_cb = rhs.net_con_cb;
            this->net_status_cb = rhs.net_status_cb;
        }
        void clear()
        {
            net_con_cb = NULL;
            net_status_cb = NULL;
        }
    };
#if defined(FIREBASE_GSM_MODEM_IS_AVAILABLE) && defined(ENABLE_GSM_NETWORK)

    enum gsm_conn_status
    {
        gsm_conn_status_idle,
        gsm_conn_status_waits_network,
        gsm_conn_status_network_connected,
        gsm_conn_status_waits_gprs
    };

    struct gsm_data
    {
        String pin, apn, user, password;
        void *modem = nullptr;
        gsm_conn_status conn_status;

    public:
        void copy(const gsm_data &rhs)
        {
            this->modem = rhs.modem;
            this->pin = rhs.pin;
            this->apn = rhs.apn;
            this->user = rhs.user;
            this->password = rhs.password;
        }
        void clear()
        {
            modem = nullptr;
            pin.clear();
            apn.clear();
            user.clear();
            password.clear();
        }
    };
#endif
#if defined(FIREBASE_ETHERNET_MODULE_IS_AVAILABLE) && defined(ENABLE_ETHERNET_NETWORK)

    enum ethernet_conn_status
    {
        ethernet_conn_status_idle,
        ethernet_conn_status_rst_pin_unselected,
        ethernet_conn_status_rst_pin_selected,
        ethernet_conn_status_rst_pin_released,
        ethernet_conn_status_begin,
        ethernet_conn_status_waits
    };

    // SPI Ethernet Module Data
    struct ethernet_data
    {
        friend class AsyncClientClass;

    private:
        int ethernet_reset_pin = -1;
        int ethernet_cs_pin = -1;
        uint8_t *ethernet_mac = nullptr;
        Firebase_StaticIP static_ip;
        ethernet_conn_status conn_satatus;
        unsigned long stobe_ms = 0;

    public:
        ethernet_data() { ethernet_mac = new uint8_t[6]; }
        ~ethernet_data() { clear(); }
        void copy(const ethernet_data &rhs)
        {
            this->ethernet_reset_pin = rhs.ethernet_reset_pin;
            this->ethernet_cs_pin = rhs.ethernet_cs_pin;
            this->static_ip = rhs.static_ip;
            setMac(rhs.ethernet_mac);
        }

        void setMac(uint8_t *mac)
        {
            if (mac)
            {
                clearMac();
                this->ethernet_mac = new uint8_t[6];
                memcpy(this->ethernet_mac, mac, 6);
            }
        }

        void setCs(int cs) { ethernet_cs_pin = cs; }

        void setReset(int reset) { ethernet_reset_pin = reset; }

        void setStaticIP(const Firebase_StaticIP &static_ip) { this->static_ip = static_ip; }

        void clearMac()
        {
            if (this->ethernet_mac)
            {
                delete[] this->ethernet_mac;
                this->ethernet_mac = nullptr;
            }
        }

        void clear()
        {
            clearMac();
            ethernet_mac = nullptr;
            ethernet_cs_pin = -1;
            ethernet_reset_pin = -1;
        }
    };
#endif

    firebase_network_data_type network_data_type = firebase_network_data_undefined;

    generic_data generic;
#if defined(FIREBASE_GSM_MODEM_IS_AVAILABLE) && defined(ENABLE_GSM_NETWORK)
    gsm_data gsm;
#endif

#if defined(ENABLE_ETHERNET_NETWORK)
#if defined(FIREBASE_ETHERNET_MODULE_IS_AVAILABLE)
    ethernet_data ethernet;
#endif
#if defined(FIREBASE_LWIP_ETH_IS_AVAILABLE)
    Firebase_SPI_ETH_Module *eth = NULL;
#endif
#endif
    bool initialized = false;
    bool network_status = false;
    bool reconnect = true;
    FirebaseWiFi *wifi = nullptr;
    Timer net_timer;
    Timer eth_timer;
    unsigned long disconnected_ms = 0;

public:
    ~network_config_data() { clear(); }
    network_config_data &operator=(const network_config_data &rhs)
    {
        copy(rhs);
        return *this;
    }

    void copy(const network_config_data &rhs)
    {
        this->network_data_type = rhs.network_data_type;
        this->initialized = rhs.initialized;
        this->reconnect = rhs.reconnect;
        generic.copy(rhs.generic);
#if defined(FIREBASE_GSM_MODEM_IS_AVAILABLE) && defined(ENABLE_GSM_NETWORK)
        gsm.copy(rhs.gsm);
#endif

#if defined(FIREBASE_ETHERNET_MODULE_IS_AVAILABLE) && defined(ENABLE_ETHERNET_NETWORK)
        ethernet.copy(rhs.ethernet);
#endif

        this->initialized = rhs.initialized;
        this->network_status = rhs.network_status;
        this->reconnect = rhs.reconnect;
        this->wifi = rhs.wifi;
#if defined(FIREBASE_LWIP_ETH_IS_AVAILABLE) && defined(ENABLE_ETHERNET_NETWORK)
        this->eth = rhs.eth;
#endif
        this->net_timer = rhs.net_timer;
        this->net_timer.start();
    }

    void clear()
    {
        generic.clear();
#if defined(FIREBASE_GSM_MODEM_IS_AVAILABLE) && defined(ENABLE_GSM_NETWORK)
        gsm.clear();
#endif
#if defined(FIREBASE_ETHERNET_MODULE_IS_AVAILABLE) && defined(ENABLE_ETHERNET_NETWORK)
        ethernet.clear();
#endif
        network_data_type = firebase_network_data_undefined;
        initialized = false;
        network_status = false;
        reconnect = true;
        wifi = nullptr;
#if defined(FIREBASE_LWIP_ETH_IS_AVAILABLE) && defined(ENABLE_ETHERNET_NETWORK)
        eth = NULL;
#endif
        net_timer.stop();
        net_timer.setInterval(0);
    }
};

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
        network_data.network_data_type = firebase_network_data_default_network;
        network_data.reconnect = reconnect;
    }
    ~DefaultNetwork() { clear(); }

    /**
     * Clear the internal network data.
     */
    void clear()
    {
        network_data.clear();
    }

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
        network_data.network_data_type = firebase_network_data_generic_network;
    }
    ~GenericNetwork() { clear(); }
};

#if defined(FIREBASE_GSM_MODEM_IS_AVAILABLE) && defined(ENABLE_GSM_NETWORK)
class GSMNetwork : public DefaultNetwork
{
public:
    /**
     * The GSM network class works for TinyGSM only (https://github.com/vshymanskyy/TinyGSM).
     *
     * @param modem The pointer to TinyGsm modem object. Modem should be initialized and/or set the mode before transfering the data.
     * @param pin The SIM pin
     * @param apn The GPRS APN (Access Point Name).
     * @param user The GPRS user.
     * @param password The GPRS password.
     *
     * This required two places of the GSM module macro definitions, one in your sketch and another in /src/Config.h
     * or in your own defined config at src/UserConfig.h or adding `TINY_GSM_MODEM_XXXXXXX` in compiler build flags.
     *
     * The TinyGsm modem should be defined at the same usage scope of GSMNetwork and AsyncClientClass.
     * See /examples/App/NetworkInterfaces/Async/Callback/GSMNetwork/ for using TinyGSM with this library.
     *
     */
    explicit GSMNetwork(TinyGsm *modem, const String &pin, const String &apn, const String &user, const String &password)
    {
        init();
        network_data.gsm.modem = modem;
        network_data.gsm.pin = pin;
        network_data.gsm.apn = apn;
        network_data.gsm.user = user;
        network_data.gsm.password = password;
        network_data.network_data_type = firebase_network_data_gsm_network;
    }
    ~GSMNetwork() { clear(); }
};
#endif

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
        network_data.network_data_type = firebase_network_data_ethernet_network;
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
        network_data.network_data_type = firebase_network_data_ethernet_network;
    }
    ~EthernetNetwork() { clear(); }
};

#endif

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
     * To use ESP8266 native lwIP Ethernet, the one of following macros, ENABLE_ESP8266_ENC28J60_ETH, ENABLE_ESP8266_W5500_ETH
     * and ENABLE_ESP8266_W5100_ETH should be defined in src/Config.h or in your own defined config at src/UserConfig.h
     * or adding ENABLE_ESP8266_ENC28J60_ETH, ENABLE_ESP8266_W5500_ETH and ENABLE_ESP8266_W5100_ETH in the compiler build flags.
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
#endif
        network_data.network_data_type = firebase_network_data_default_network;
    }

    /**
     * The default Ethernet class for ESP32 with native Ethernet boards.
     * See /examples/App/NetworkInterfaces/Async/Callback/DefaultNetworks/DefaultEthernetNetwork/ESP32/ for using ESP32 with its native Ethernet library.
     */
    DefaultEthernetNetwork()
    {
        init();
        network_data.network_data_type = firebase_network_data_default_network;
    }
    ~DefaultEthernetNetwork() { clear(); }
};

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
        network_data.network_data_type = firebase_network_data_default_network;
    }
    ~DefaultWiFiNetwork() { clear(); }
};

template <typename T>
static network_config_data &getNetwork(T &net) { return net.get(); }

#endif