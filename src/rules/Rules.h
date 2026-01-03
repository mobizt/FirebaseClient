/*
 * SPDX-FileCopyrightText: 2026 Suwatchai K. <suwatchai@outlook.com>
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef RULES_RULES_H
#define RULES_RULES_H

#if defined(ENABLE_RULESETS)

#include <Arduino.h>
#include "./core/Utils/ObjectWriter.h"
#include "./core/Utils/Base64.h"

namespace Rules
{
    /**
     * File containing source content.
    */
    class File : public BaseO4
    {
    private:
        ObjectWriter owriter;
        Base64Util but;
        Memory mem;
        String toBase64(const String &str) { return but.encodeToChars(mem, (uint8_t *)str.c_str(), str.length()); }

    public:
        File() {}
        explicit File(const String &content, const String &name, const String &fingerprint = "")
        {
            this->content(content);
            this->name(name);
            this->fingerprint(fingerprint);
        }

        /**
         * Rules language statements. Use \n for line breaks.
        */
        void content(const String &content) { wr.set<File &, const char *>(*this, content.c_str(), buf, bufSize, 1, __func__); }
        
        /**
         * File name.
        */
        void name(const String &name) { wr.set<File &, const char *>(*this, name.c_str(), buf, bufSize, 2, __func__); }
        
        /**
         * Fingerprint (e.g. github sha) associated with the File.
         * Don't provide the base64 encoded fingerprint as it will encode automatically.
        */
        void fingerprint(const String &fingerprint) { wr.set<File &, const char *>(*this, toBase64(fingerprint).c_str(), buf, bufSize, 3, __func__); }
        const char *c_str() const { return buf[0].c_str(); }
        void clear() { owriter.clearBuf(buf, bufSize); }
    };

    /**
     *  One or more File messages comprising a logical set of rules.
     */
    class Source : public BaseO2
    {
    private:
        ObjectWriter owriter;
        BufWriter wr;
        JSONUtil jut;

    public:
        Source() {}
        explicit Source(const File &file) { files(file); }

        /**
         * File containing source content.
        */
        void files(const File &file) { wr.append<Source &, File>(*this, file, buf, bufSize, 1, __func__); }
        const char *c_str() const { return buf[0].c_str(); }
        void clear() { owriter.clearBuf(buf, bufSize); }
    };
}
#endif
#endif
