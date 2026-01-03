/*
 * SPDX-FileCopyrightText: 2026 Suwatchai K. <suwatchai@outlook.com>
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef CORE_FILE_BLOB_WRITER_H
#define CORE_FILE_BLOB_WRITER_H

#include <Arduino.h>

struct firebase_blob_writer
{
public:
    firebase_blob_writer() {}
    
    void init(uint8_t *data, size_t size)
    {
        this->data = data;
        this->size = size;
        index = 0;
    }

    size_t curIndex() const { return index; }

    size_t write(const uint8_t *buf, size_t size)
    {
        uint32_t towrite = size;
        if (index + towrite > this->size)
            towrite = this->size - index;
        if (towrite > 0)
        {
            memcpy(data + index, buf, towrite);
            index += towrite;
        }
        return towrite;
    }

private:
    size_t size = 0;
    uint8_t *data = nullptr;
    uint32_t index = 0;
};
#endif