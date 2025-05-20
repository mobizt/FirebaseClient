#ifndef CORE_NETWORK_GSM_NETWORK_H
#define CORE_NETWORK_GSM_NETWORK_H

#include <Arduino.h>
#include "./core/Network/DefaultNetwork.h"

#if defined(FIREBASE_GSM_MODEM_IS_AVAILABLE) && defined(ENABLE_GSM_NETWORK)
class GSMNetwork : public DefaultNetwork
{
public:
    /**
     * The GSM network class works for TinyGSM only (https://github.com/vshymanskyy/TinyGSM).
     *
     * @param modem The pointer to TinyGsm modem object. Modem should be initialized and/or set the mode before transfering the data.
     *
     * This required the GSM module macro definitions, by defining the `TINY_GSM_MODEM_XXXXXXX` in the sketch before including 
     * the library header file `FirebaseClient.h` or adding `TINY_GSM_MODEM_XXXXXXX` in the compiler build flags.
     *
     * The TinyGsm modem should be defined at the same usage scope of GSMNetwork and AsyncClientClass.
     * See /examples/App/NetworkInterfaces/Async/Callback/GSMNetwork/ for using TinyGSM with this library.
     *
     */
    explicit GSMNetwork(TinyGsm *modem)
    {
        init();
        network_data.gsm.modem = modem;
        network_data.gsm.pin = "";
        network_data.gsm.apn = "";
        network_data.gsm.user = "";
        network_data.gsm.password = "";
        network_data.network_type = firebase_network_gsm;
        network_data.gsm.allowed_initialize = false;
    }

    /**
     * The GSM network class works for TinyGSM only (https://github.com/vshymanskyy/TinyGSM).
     *
     * @param modem The pointer to TinyGsm modem object. Modem should be initialized and/or set the mode before transfering the data.
     * @param pin The SIM pin
     * @param apn The GPRS APN (Access Point Name).
     * @param user The GPRS user.
     * @param password The GPRS password.
     *
     * This required the GSM module macro definitions, by defining the `TINY_GSM_MODEM_XXXXXXX` in the sketch before including 
     * the library header file `FirebaseClient.h` or adding `TINY_GSM_MODEM_XXXXXXX` in the compiler build flags.
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
        network_data.network_type = firebase_network_gsm;
        network_data.gsm.allowed_initialize = true;
    }
    ~GSMNetwork() { clear(); }
};
#endif
#endif