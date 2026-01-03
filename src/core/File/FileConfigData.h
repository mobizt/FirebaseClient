/*
 * SPDX-FileCopyrightText: 2026 Suwatchai K. <suwatchai@outlook.com>
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef CORE_FILE_FILE_CONFIG_DATA_H
#define CORE_FILE_FILE_CONFIG_DATA_H

#include <Arduino.h>
#include "./core/Utils/Memory.h"
#include "./core/File/BlobWriter.h"
#include "./core/File/ResumableUpload.h"

enum file_operating_mode
{
    file_mode_open_read,
    file_mode_open_write,
    file_mode_open_append,
    file_mode_remove
};

#if defined(ENABLE_FS)
#include <FS.h>
typedef void (*FileConfigCallback)(File &file, const char *filename, file_operating_mode mode);
#endif

struct file_config_data
{

#if defined(ENABLE_FS)
    enum file_operating_status
    {
        file_status_closed,
        file_status_opened
    };
    File file;
    FileConfigCallback cb = NULL;
    String filename;
    file_operating_status file_status = file_status_closed;
#endif
#if defined(ENABLE_CLOUD_STORAGE)
    file_upload_resumable_data resumable;
#endif

    uint8_t *data = nullptr;
    size_t data_pos = 0, data_size = 0, file_size = 0;
    bool internal_data = false, initialized = false;
    firebase_blob_writer outB;
    StringUtil sut;
    Memory mem;

    void clearInternalData()
    {
        if (internal_data && data && data_size > 0)
            mem.release(&data);
        data_size = 0;
        internal_data = false;
    }
    void initBlobWriter(size_t size)
    {
        clearInternalData();
        data = reinterpret_cast<uint8_t *>(mem.alloc(size));
        if (data)
        {
            data_size = size;
            internal_data = true;
            outB.init(data, size);
        }
    }

public:
    file_config_data &operator=(const file_config_data &rhs)
    {
        copy(rhs);
        return *this;
    }
    void copy(const file_config_data &rhs)
    {
#if defined(ENABLE_FS)
        this->file = rhs.file;
        this->cb = rhs.cb;
        this->filename = rhs.filename;
        this->file_status = rhs.file_status;
#endif

        this->file_size = rhs.file_size;
        this->data_pos = rhs.data_pos;
        this->data_size = rhs.data_size;
        this->data = rhs.data;
        if (!rhs.internal_data && rhs.data && rhs.data_size > 0)
        {
            clearInternalData();
            this->data = rhs.data;
            this->data_size = rhs.data_size;
            this->internal_data = false;
        }
        this->initialized = rhs.initialized;
    }
    void clear()
    {
#if defined(ENABLE_FS)
        cb = NULL;
        sut.clear(filename);
#endif
        clearInternalData();
        data = nullptr;
        data_size = 0;
        internal_data = false;
        initialized = false;
    }
};
#endif