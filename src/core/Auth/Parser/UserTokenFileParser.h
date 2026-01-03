/*
 * SPDX-FileCopyrightText: 2026 Suwatchai K. <suwatchai@outlook.com>
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef CORE_AUTH_PARSER_USER_TOKEN_PARSER_H
#define CORE_AUTH_PARSER_USER_TOKEN_PARSER_H

#include <Arduino.h>
#include "./core/File/FileConfig.h"
#include "./core/Auth/UserAuthData.h"

namespace firebase_ns
{
#if defined(ENABLE_FS)
#if defined(ENABLE_USER_AUTH) || defined(ENABLE_ACCESS_TOKEN) || defined(ENABLE_CUSTOM_TOKEN) || defined(ENABLE_ID_TOKEN) || defined(ENABLE_LEGACY_TOKEN)
    class UserTokenFileParser
    {
    public:
        enum token_type
        {
            token_type_user_data,
            token_type_id_token,
            token_type_access_token,
            token_type_custom_token,
            token_type_legacy_token
        };

        static bool parseUserFile(token_type type, File userfile, user_auth_data &auth_data)
        {
            String buff;
            if (userfile && userfile.size())
            {
                while (userfile.available())
                {
                    buff += (char)userfile.read();
                }
                Memory mem;
                StringUtil sut;
                int tokenSize = 1;
                for (size_t i = 0; i < buff.length(); i++)
                {
                    if (buff[i] == ',')
                        tokenSize++;
                }
                String tokens[tokenSize], tmp;
                char *p = reinterpret_cast<char *>(mem.alloc(buff.length()));
                strcpy(p, buff.c_str());
                char *pp = p, *end = p;
                int i = 0;
                while (pp != NULL)
                {
                    sut.strsepImpl(&end, ",");
                    if (strlen(pp) > 0)
                    {
                        tmp = pp;
                        tokens[i] = tmp;
                        i++;
                    }
                    pp = end;
                }
                // release memory
                mem.release(&p);
                if (type == token_type_user_data && tokenSize == 3)
                {
                    for (size_t j = 0; j < 3; j++)
                        auth_data.user.val[j] = tokens[j];
                    return true;
                }
                else if (type == token_type_access_token && tokenSize == 5)
                {
#if defined(ENABLE_ACCESS_TOKEN)
                    for (size_t j = 0; j < access_tk_ns::max_type; j++)
                        auth_data.access_token.val[j] = tokens[j];
                    auth_data.access_token.expire = atoi(tokens[4].c_str());
#endif
                    return true;
                }
                else if (type == token_type_id_token && tokenSize == 4)
                {
#if defined(ENABLE_ID_TOKEN)
                    auth_data.id_token.val[id_tk_ns::token] = tokens[0];
                    auth_data.id_token.val[id_tk_ns::refresh] = tokens[1];
                    auth_data.user.val[user_ns::api_key] = tokens[2];
                    auth_data.id_token.expire = atoi(tokens[3].c_str());
#endif
                }
                else if (type == token_type_custom_token && tokenSize == 3)
                {
#if defined(ENABLE_CUSTOM_TOKEN)
                    auth_data.custom_token.val[cust_tk_ns::token] = tokens[0];
                    auth_data.user.val[user_ns::api_key] = tokens[1];
                    auth_data.custom_token.expire = atoi(tokens[2].c_str());
#endif
                }
                else if (type == token_type_legacy_token && tokenSize == 1)
                {
#if defined(ENABLE_LEGACY_TOKEN)
                    auth_data.legacy_token.val[legacy_tk_ns::token] = tokens[0];
#endif
                }
            }
            return false;
        }

        static bool saveUserFile(token_type type, File userfile, user_auth_data &auth_data)
        {
            String buff;
            if (userfile)
            {
                if (type == token_type_user_data)
                {
                    for (size_t i = 0; i < 3; i++)
                    {
                        userfile.print(auth_data.user.val[i].c_str());
                        if (i < 2)
                            userfile.print(",");
                    }
                    return true;
                }
                else if (type == token_type_access_token)
                {
#if defined(ENABLE_ACCESS_TOKEN)
                    for (size_t i = 0; i < access_tk_ns::max_type; i++)
                    {
                        userfile.print(auth_data.access_token.val[i].c_str());
                        userfile.print(",");
                    }
                    userfile.print(String(auth_data.access_token.expire).c_str());
#endif
                    return true;
                }
                else if (type == token_type_id_token)
                {
#if defined(ENABLE_ID_TOKEN)
                    userfile.print(auth_data.id_token.val[id_tk_ns::token].c_str());
                    userfile.print(",");
                    userfile.print(auth_data.id_token.val[id_tk_ns::refresh].c_str());
                    userfile.print(",");
                    userfile.print(auth_data.user.val[user_ns::api_key].c_str());
                    userfile.print(",");
                    userfile.print(String(auth_data.id_token.expire).c_str());
#endif
                    return true;
                }
                else if (type == token_type_custom_token)
                {
#if defined(ENABLE_CUSTOM_TOKEN)
                    userfile.print(auth_data.custom_token.val[cust_tk_ns::token].c_str());
                    userfile.print(",");
                    userfile.print(auth_data.user.val[user_ns::api_key].c_str());
                    userfile.print(",");
                    userfile.print(auth_data.custom_token.expire);
#endif
                    return true;
                }
                else if (type == token_type_legacy_token)
                {
#if defined(ENABLE_LEGACY_TOKEN)
                    userfile.print(auth_data.legacy_token.val[legacy_tk_ns::token].c_str());
#endif
                    return true;
                }
            }
            return false;
        }
    };
#endif
#endif
}
#endif