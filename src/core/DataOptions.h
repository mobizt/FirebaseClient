#ifndef CORE_DATABASE_DATAOPTIONS_H
#define CORE_DATABASE_DATAOPTIONS_H

#include <Arduino.h>
#include "Config.h"
#include "core/Filter.h"


#if defined(ENABLE_DATABASE)


class DataOptions
{
    friend class Database;

public:
    uint32_t readTimeout = 0;
    String writeSizeLimit;
    bool shallow = false;
    bool silent = false;
    bool classicRequest = false;
    String customHeaders;
    String ETAG;
    Filter filter;

    void copy(DataOptions &rhs)
    {
        this->readTimeout = rhs.readTimeout;
        this->writeSizeLimit = rhs.writeSizeLimit;
        this->shallow = rhs.shallow;
        this->silent = rhs.silent;
        this->classicRequest = rhs.classicRequest;
        this->customHeaders = rhs.customHeaders;
        this->ETAG = rhs.ETAG;
        this->filter.copy(rhs.filter);
        this->ota = rhs.ota;
        this->base64 = rhs.base64;
    }

    void clear()
    {
        readTimeout = 0;
        writeSizeLimit.clear();
        shallow = false;
        silent = false;
        classicRequest = false;
        customHeaders.clear();
        ETAG.clear();
        filter.clear();
    }

private:
    bool base64 = false;
    bool ota = false;
};

#endif

#endif