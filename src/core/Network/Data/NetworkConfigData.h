#ifndef CORE_NETWORK_DATA_NETWORK_CONFIG_DATA_H
#define CORE_NETWORK_DATA_NETWORK_CONFIG_DATA_H

#include <Arduino.h>
#include "./Config.h"
#include "./core/Utils/Memory.h"
#include "./core/Utils/Timer.h"
#include "./core/Network/NetworkOptions.h"
#include "./core/Network/Data/SPIEthernet.h"
#include "./core/Network/Data/FirebaseWiFi.h"

typedef enum
{
    firebase_network_undefined,
    firebase_network_default,
    firebase_network_generic,
    firebase_network_gsm,
    firebase_network_ethernet
} firebase_network_type;

static unsigned long wifi_reconnection_ms = 0;
static uint32_t gsm_network_owner_addr = 0;
static uint32_t ethernet_network_owner_addr = 0;
static uint32_t generic_network_owner_addr = 0;
typedef void (*NetworkConnectionCallback)(void);
typedef void (*NetworkStatusCallback)(bool &);

struct network_config_data
{
    friend class DefaultNetwork;
    friend class GenericNetwork;
    friend class EthernetNetwork;
    friend class GSMNetwork;
    friend class DefaultPPPNetwork;
    friend class DefaultEthernetNetwork;
    friend class DefaultWiFiNetwork;
    friend class AsyncClientClass;
    friend class conn_handler;

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
            pin.remove(0, pin.length());
            apn.remove(0, apn.length());
            user.remove(0, user.length());
            password.remove(0, password.length());
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
        int ethernet_reset_pin = -1, ethernet_cs_pin = -1;
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

    firebase_network_type network_type = firebase_network_undefined;
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
        this->network_type = rhs.network_type;
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
        network_type = firebase_network_undefined;
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
#endif