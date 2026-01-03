/*
 * SPDX-FileCopyrightText: 2026 Suwatchai K. <suwatchai@outlook.com>
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef DATABASE_DATA_OPTIONS_H
#define DATABASE_DATA_OPTIONS_H

#include <Arduino.h>
#include "./database/DatabaseFilter.h"
#include "./core/Utils/StringUtil.h"

#if defined(ENABLE_DATABASE)
class DatabaseOptions
{
    friend class RealtimeDatabase;

private:
    StringUtil sut;

public:
    uint32_t readTimeout = 0;
    String writeSizeLimit;
    bool shallow = false, silent = false;
    DatabaseFilter filter;

    void copy(const DatabaseOptions &rhs)
    {
        this->readTimeout = rhs.readTimeout;
        this->writeSizeLimit = rhs.writeSizeLimit;
        this->shallow = rhs.shallow;
        this->filter.copy(rhs.filter);
    }

    void clear()
    {
        readTimeout = 0;
        sut.clear(writeSizeLimit);
        shallow = false;
        silent = false;
        filter.clear();
    }

private:
    bool base64 = false, ota = false;
};
#endif
#endif