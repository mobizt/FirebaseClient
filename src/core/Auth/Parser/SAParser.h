/*
 * SPDX-FileCopyrightText: 2026 Suwatchai K. <suwatchai@outlook.com>
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef CORE_AUTH_PARSER_SA_PARSER_H
#define CORE_AUTH_PARSER_SA_PARSER_H

#include <Arduino.h>
#include "./core/File/FileConfig.h"
#include "./core/Auth/UserAuthData.h"

namespace firebase_ns
{
#if defined(ENABLE_FS)
#if defined(ENABLE_SERVICE_AUTH) || defined(ENABLE_CUSTOM_AUTH)
    class SAParser
    {
    public:
        static bool parseSAFile(File safile, user_auth_data &auth_data)
        {
            bool ret = false;
            if (safile && safile.size())
            {
                String buf;
                buf.reserve(safile.size() + 10);
                while (safile.available())
                {
                    buf += (char)safile.read();
                }
                safile.close();

                StringUtil sut;
                int p1 = 0, p2 = 0;
                sut.parse(buf, "\"project_id\"", ",", p1, p2);
                if (p1 > -1 && p2 > -1)
                {
                    auth_data.sa.val[sa_ns::pid] = buf.substring(p1 + 1, p2 - 1);
                    p1 = p2;
                }
                sut.parse(buf, "\"private_key_id\"", ",", p1, p2);
                if (p1 > -1 && p2 > -1)
                {
                    auth_data.sa.val[sa_ns::pkid] = buf.substring(p1 + 1, p2 - 1);
                    p1 = p2;
                }
                sut.parse(buf, "\"private_key\"", ",", p1, p2);
                if (p1 > -1 && p2 > -1)
                {
                    auth_data.sa.val[sa_ns::pk] = buf.substring(p1 + 1, p2 - 1);
                    auth_data.sa.val[sa_ns::pk].replace("\\n", "\n");
                    p1 = p2;
                    auth_data.initialized = true;
                    ret = true;
                }
                sut.parse(buf, "\"client_email\"", ",", p1, p2);
                if (p1 > -1 && p2 > -1)
                {
                    auth_data.sa.val[sa_ns::cm] = buf.substring(p1 + 1, p2 - 1);
                    p1 = p2;
                }
                sut.parse(buf, "\"client_id\"", ",", p1, p2);
                if (p1 > -1 && p2 > -1)
                {
                    auth_data.sa.val[sa_ns::cid] = buf.substring(p1 + 1, p2 - 1);
                }
            }

            return ret;
        }
    };

#endif
#endif
}
#endif