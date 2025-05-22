#ifndef CORE_NETWORK_NET_CONFIG_H
#define CORE_NETWORK_NET_CONFIG_H

#include <Arduino.h>
#if !defined(DISABLE_NERWORKS)
#include "./core/Network/Data/NetworkConfigData.h"
#include "./core/Network/GenericNetwork.h"
#include "./core/Network/GSMNetwork.h"
#include "./core/Network/EthernetNetwork.h"
#include "./core/Network/DefaultEthernetNetwork.h"
#include "./core/Network/DefaultPPPNetwork.h"
#include "./core/Network/DefaultWiFiNetwork.h"
#include "./core/Network/DefaultNetwork.h"
#include "./core/Network/Data/StaticAddress.h"

template <typename T>
static network_config_data &getNetwork(T &net) { return net.get(); }
#endif
#endif