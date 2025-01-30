#ifndef CORE_FILE_DEFAULT_FILE_CONFIG_H
#define CORE_FILE_DEFAULT_FILE_CONFIG_H

#include <Arduino.h>
#include "./Config.h"
#include "./core/File/FileConfigData.h"

class DefaultFileConfig
{
public:
    template <typename T = const char *>
    DefaultFileConfig() {}
    ~DefaultFileConfig() {}
    file_config_data &get() { return data; }

private:
    file_config_data data;
};
#endif