/*
 * SPDX-FileCopyrightText: 2026 Suwatchai K. <suwatchai@outlook.com>
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef CORE_FILE_FILE_CONFIG_H
#define CORE_FILE_FILE_CONFIG_H

#include <Arduino.h>
#include "./core/Utils/StringUtil.h"
#include "./core/File/FileConfigData.h"
#include "./core/File/DefaultFileConfig.h"
#include "./core/File/BlobConfig.h"

#if defined(ENABLE_FS)
class FileConfig
{
public:
    /**
     * File config class.
     *
     * @param filename The file name of file to be read and write.
     * @param cb The FileConfigCallback function that accepts File object, file name and file_operating_mode to be processed.
     *
     * The file_operating_mode enums are included file_mode_open_read, file_mode_open_write, file_mode_open_append, and file_mode_remove.
     */
    explicit FileConfig(const String &filename = "", FileConfigCallback cb = NULL) { setFile(filename, cb); }

    ~FileConfig() {}

    /**
     * Set the File config.
     *
     * @param filename The file name of file to be read and write.
     * @param cb The FileConfigCallback function that accepts File object, file name and file_operating_mode to be processed.
     *
     * The file_operating_mode enums are included file_mode_open_read, file_mode_open_write, file_mode_open_append, and file_mode_remove.
     */
    void setFile(const String &filename, FileConfigCallback cb)
    {
        clear();
        if (cb && filename.length())
        {
            setFilename(filename);
            setCallback(cb);
            data.initialized = true;
        }
    }

    /**
     * Clear the internal data.
     */
    void clear() { data.clear(); }

    /**
     * Set the file name.
     *
     * @param filename The file name of file to be read and write.
     */
    FileConfig &setFilename(const String &filename)
    {
        if (filename.length() > 0 && filename[0] != '/')
            data.filename += '/';
        data.filename += filename;
        if (data.cb && data.filename.length())
            data.initialized = true;
        return *this;
    }

    /**
     * Set the callback.
     *
     * @param cb The FileConfigCallback function that accepts File object, file name and file_operating_mode to be processed.
     *
     * The file_operating_mode enums are included file_mode_open_read, file_mode_open_write, file_mode_open_append, and file_mode_remove.
     */
    FileConfig &setCallback(FileConfigCallback cb)
    {
        data.cb = cb;
        if (data.cb && data.filename.length())
            data.initialized = true;
        return *this;
    }

    /**
     * Get the reference to the internal file_config_data.
     * @return file_config_data & The reference to file_config_data.
     */
    file_config_data &get() { return data; }

private:
    file_config_data data;
};
#endif

/**
 * The static function to get the reference of file_config_data from FileConfig.
 *
 * @return file_config_data & The reference to the internal file_config_data.
 */
template <typename T>
static file_config_data &getFile(T &file) { return file.get(); }

/**
 * The static function to get the reference of file_config_data from BlobConfig.
 *
 * @return file_config_data & The reference to the internal file_config_data.
 */
template <typename T>
static file_config_data &getBlob(T &blob) { return blob.getData(); }
#endif