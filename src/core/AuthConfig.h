/**
 * Created June 12, 2024
 *
 * The MIT License (MIT)
 * Copyright (c) 2024 K. Suwatchai (Mobizt)
 *
 *
 * Permission is hereby granted, free of charge, to any person returning a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#ifndef CORE_AUTH_CONFIG_H
#define CORE_AUTH_CONFIG_H
#include <Arduino.h>
#include "./core/FileConfig.h"
#include "./core/StringUtil.h"

#include "./core/AsyncResult/AsyncResult.h"

#define FIREBASE_DEFAULT_TOKEN_TTL 3300

namespace sa_ns
{
    enum data_item_type_t
    {
        cm,   // client email
        pid,  // project Id
        pk,   // private key
        pkid, // private key Id
        cid,  // client Id
        max_type
    };
}

namespace access_tk_ns
{
    enum data_item_type_t
    {
        token,   // access token
        refresh, // refresh token
        cid,     // client id
        csec,    // client secret
        max_type
    };
}

namespace id_tk_ns
{
    enum data_item_type_t
    {
        token,   // id token
        refresh, // refresh token
        max_type
    };
}

namespace user_ns
{
    enum data_item_type_t
    {
        em,       // email
        psw,      // password
        api_key,  // API Key
                  // used with apiKey for current user verification and delete
        id_token, // Id token
        max_type
    };
}

namespace cust_ns
{
    enum data_item_type_t
    {
        api_key, // API Key
        uid,     // UID
        scope,   // scope
        claims,  // claims
        max_type
    };
}

namespace cust_tk_ns
{
    enum data_item_type_t
    {
        token, // id token
        max_type
    };
}

namespace legacy_tk_ns
{
    enum data_item_type_t
    {
        token, // token
        max_type
    };
}

namespace app_tk_ns
{
    enum data_item_type_t
    {
        type,    // token type
        token,   // auth token
        refresh, // refresh token
        uid,     // UID
        pid,     // project Id
        max_type
    };
}

namespace firebase
{
    enum firebase_core_auth_task_type
    {
        firebase_core_auth_task_type_undefined = -1,
        firebase_core_auth_task_type_authenticate,
        firebase_core_auth_task_type_refresh_token,
        firebase_core_auth_task_type_signup,
        firebase_core_auth_task_type_send_verify_email,
        firebase_core_auth_task_type_delete_user,
        firebase_core_auth_task_type_reset_password
    };

    enum firebase_auth_event_type
    {
        auth_event_error = -1,
        auth_event_uninitialized = 0,
        auth_event_initializing = 1,
        auth_event_signup = 2,
        auth_event_send_verify_email = 3,
        auth_event_delete_user = 4,
        auth_event_reset_password = 5,
        auth_event_token_signing = 6,
        auth_event_authenticating = 7,
        auth_event_auth_request_sent = 8,
        auth_event_auth_response_received = 9,
        auth_event_ready = 10
    };

    enum jwt_step
    {
        jwt_step_begin,
        jwt_step_create_jwt,
        jwt_step_sign,
        jwt_step_ready,
        jwt_step_error
    };

    enum auth_token_type
    {
        auth_unknown_token,
        auth_user_id_token,
        auth_sa_access_token,
        auth_sa_custom_token,
        auth_id_token,
        auth_access_token,
        auth_custom_token,
        auth_refresh_token,
        auth_user_signup,
        auth_user_reset_password,
    };

    enum user_auth_data_type
    {
        user_auth_data_undefined,
        user_auth_data_service_account,
        user_auth_data_custom_data,
        user_auth_data_user_data,
        user_auth_data_id_token,
        user_auth_data_access_token,
        user_auth_data_custom_token,
        user_auth_data_legacy_token,
        user_auth_data_no_token
    };

    struct auth_status
    {
        friend class user_auth_data;
        friend class FirebaseApp;

    private:
        firebase_auth_event_type _event = auth_event_uninitialized;

        String authEventString(firebase_auth_event_type ev)
        {
            String evstr;
            switch (ev)
            {
            case auth_event_initializing:
                evstr = FPSTR("initializing");
                break;
            case auth_event_signup:
                evstr = FPSTR("sign up");
                break;
            case auth_event_send_verify_email:
                evstr = FPSTR("send verification email");
                break;
            case auth_event_delete_user:
                evstr = FPSTR("delete user");
                break;
            case auth_event_reset_password:
                evstr = FPSTR("reset password");
                break;
            case auth_event_token_signing:
                evstr = FPSTR("token signing");
                break;
            case auth_event_authenticating:
                evstr = FPSTR("authenticating");
                break;
            case auth_event_auth_request_sent:
                evstr = FPSTR("auth request sent");
                break;
            case auth_event_auth_response_received:
                evstr = FPSTR("auth response received");
                break;
            case auth_event_ready:
                evstr = FPSTR("ready");
                break;
            case auth_event_error:
                evstr = FPSTR("error");
                break;
            default:
                evstr = FPSTR("undefined");
                break;
            }

            return evstr;
        }

    public:
        firebase_auth_event_type event() const { return _event; }
    };

    typedef void (*TimeStatusCallback)(uint32_t &ts);

    struct user_auth_data
    {
        friend class SAParser;
        friend class UserAuth;
        friend class ServiceAuth;
        friend class CustomAuth;
        friend class IDToken;
        friend class AccessToken;
        friend class CustomToken;
        friend class JWTClass;
        friend class FirebaseApp;
        friend class UserTokenFileParser;
        friend class LegacyToken;
        friend class NoAuth;
        friend class UserAccount;
        friend class FirebaseClient;

#if defined(ENABLE_SERVICE_AUTH)

        struct service_account
        {
            friend class SAParser;
            friend class user_auth_data;
            friend class CustomAuth;
            friend class ServiceAuth;
            friend class FirebaseApp;
            friend class JWTClass;
            friend class FirebaseClient;

        public:
            service_account() {}
            ~service_account() { clear(); }
            void copy(const service_account &rhs)
            {
                for (size_t i = 0; i < sa_ns::max_type; i++)
                    this->val[i] = rhs.val[i];
                this->timestatus_cb = rhs.timestatus_cb;
                this->expire = rhs.expire;
                this->step = rhs.step;
            }

            void clear()
            {
                for (size_t i = 0; i < sa_ns::max_type; i++)
                    val[i].remove(0, val[i].length());
                timestatus_cb = NULL;
                expire = FIREBASE_DEFAULT_TOKEN_TTL;
                step = jwt_step_begin;
            }

        protected:
            String val[sa_ns::max_type];
            jwt_step step = jwt_step_begin;
            TimeStatusCallback timestatus_cb = NULL;
            size_t expire = FIREBASE_DEFAULT_TOKEN_TTL;
        };
#endif

#if defined(ENABLE_CUSTOM_AUTH)
        struct custom_data
        {
            // For custom claims
            String val[cust_ns::max_type];
            TimeStatusCallback timestatus_cb = NULL;

        public:
            custom_data() {}
            ~custom_data() { clear(); }
            void copy(const custom_data &rhs)
            {
                for (size_t i = 0; i < cust_ns::max_type; i++)
                    this->val[i] = rhs.val[i];
                this->timestatus_cb = rhs.timestatus_cb;
            }

            void clear()
            {
                for (size_t i = 0; i < cust_ns::max_type; i++)
                    val[i].remove(0, val[i].length());
                timestatus_cb = NULL;
            }
        };
#endif

        struct user_data
        {
            String val[user_ns::max_type];
            size_t expire = FIREBASE_DEFAULT_TOKEN_TTL;

        public:
            user_data() {}
            ~user_data() { clear(); }
            void copy(const user_data &rhs)
            {
                for (size_t i = 0; i < user_ns::max_type; i++)
                    this->val[i] = rhs.val[i];
                this->expire = rhs.expire;
            }
            void clear()
            {
                for (size_t i = 0; i < user_ns::max_type; i++)
                    val[i].remove(0, val[i].length());
                expire = FIREBASE_DEFAULT_TOKEN_TTL;
            }
        };

#if defined(ENABLE_ID_TOKEN)
        struct id_token_data
        {
            String val[id_tk_ns::max_type];
            size_t expire = FIREBASE_DEFAULT_TOKEN_TTL;

        public:
            id_token_data() {}
            ~id_token_data() { clear(); }
            void copy(const id_token_data &rhs)
            {
                for (size_t i = 0; i < id_tk_ns::max_type; i++)
                    this->val[i] = rhs.val[i];
                this->expire = rhs.expire;
            }
            void clear()
            {
                for (size_t i = 0; i < id_tk_ns::max_type; i++)
                    val[i].remove(0, val[i].length());
                expire = FIREBASE_DEFAULT_TOKEN_TTL;
            }
        };
#endif

#if defined(ENABLE_ACCESS_TOKEN)
        struct access_token_data
        {
            String val[access_tk_ns::max_type];
            size_t expire = FIREBASE_DEFAULT_TOKEN_TTL;
            TimeStatusCallback timestatus_cb = NULL;

        public:
            access_token_data() {}
            ~access_token_data() { clear(); }
            void copy(const access_token_data &rhs)
            {
                for (size_t i = 0; i < access_tk_ns::max_type; i++)
                    this->val[i] = rhs.val[i];
                this->expire = rhs.expire;
                this->timestatus_cb = rhs.timestatus_cb;
            }
            void clear()
            {
                for (size_t i = 0; i < access_tk_ns::max_type; i++)
                    val[i].remove(0, val[i].length());
                expire = FIREBASE_DEFAULT_TOKEN_TTL;
                timestatus_cb = NULL;
            }
        };
#endif

#if defined(ENABLE_CUSTOM_TOKEN)
        struct custom_token_data
        {
            String val[cust_tk_ns::max_type];
            size_t expire = FIREBASE_DEFAULT_TOKEN_TTL;
            TimeStatusCallback timestatus_cb = NULL;

        public:
            custom_token_data() {}
            ~custom_token_data() { clear(); }
            void copy(const custom_token_data &rhs)
            {
                this->val[cust_tk_ns::token] = rhs.val[cust_tk_ns::token];
                this->expire = rhs.expire;
                this->timestatus_cb = rhs.timestatus_cb;
            }
            void clear()
            {
                val[cust_tk_ns::token].remove(0, val[cust_tk_ns::token].length());
                expire = FIREBASE_DEFAULT_TOKEN_TTL;
                timestatus_cb = NULL;
            }
        };
#endif

#if defined(ENABLE_LEGACY_TOKEN)
        struct legacy_token_data
        {
            String val[legacy_tk_ns::max_type];

        public:
            legacy_token_data() {}
            ~legacy_token_data() { clear(); }
            void copy(const legacy_token_data &rhs) { this->val[legacy_tk_ns::token] = rhs.val[legacy_tk_ns::token]; }
            void clear() { val[legacy_tk_ns::token].remove(0, val[legacy_tk_ns::token].length()); }
        };
#endif

    public:
        user_auth_data() {}
        ~user_auth_data() { clear(); }
        user_auth_data &operator=(const user_auth_data &rhs)
        {
            copy(rhs);
            return *this;
        }
        void copy(const user_auth_data &rhs)
        {
#if defined(ENABLE_SERVICE_AUTH)
            this->sa.copy(rhs.sa);
#endif

#if defined(ENABLE_CUSTOM_AUTH)
            this->cust.copy(rhs.cust);
#endif

            this->user.copy(rhs.user);

#if defined(ENABLE_ID_TOKEN)
            this->id_token.copy(rhs.id_token);
#endif

#if defined(ENABLE_ACCESS_TOKEN)
            this->access_token.copy(rhs.access_token);
#endif

#if defined(ENABLE_CUSTOM_TOKEN)
            this->custom_token.copy(rhs.custom_token);
#endif

#if defined(ENABLE_LEGACY_TOKEN)
            this->legacy_token.copy(rhs.legacy_token);
#endif
            this->auth_type = rhs.auth_type;
            this->auth_data_type = rhs.auth_data_type;
            this->anonymous = rhs.anonymous;
            this->initialized = rhs.initialized;
            this->timestatus_cb = rhs.timestatus_cb;
        }

        void clear()
        {
            user.clear();

#if defined(ENABLE_SERVICE_AUTH)
            sa.clear();
#endif

#if defined(ENABLE_CUSTOM_AUTH)
            cust.clear();
#endif

#if defined(ENABLE_ID_TOKEN)
            id_token.clear();
#endif

#if defined(ENABLE_ACCESS_TOKEN)
            access_token.clear();
#endif

#if defined(ENABLE_CUSTOM_TOKEN)
            custom_token.clear();
#endif

#if defined(ENABLE_LEGACY_TOKEN)
            legacy_token.clear();
#endif

            timestatus_cb = NULL;
            anonymous = false;
            initialized = false;
        }

    protected:
        user_data user;

#if defined(ENABLE_SERVICE_AUTH)
        service_account sa;
#endif
#if defined(ENABLE_CUSTOM_AUTH)
        custom_data cust;
#endif
#if defined(ENABLE_ID_TOKEN)
        id_token_data id_token;
#endif

#if defined(ENABLE_ACCESS_TOKEN)
        access_token_data access_token;
#endif
#if defined(ENABLE_CUSTOM_TOKEN)
        custom_token_data custom_token;
#endif
#if defined(ENABLE_LEGACY_TOKEN)
        legacy_token_data legacy_token;
#endif

        bool anonymous = false;
        bool initialized = false;
        bool jwt_signing = false;
        bool jwt_loop = false;
        bool jwt_time_debug = false;
        uint32_t jwt_ts = 0;
        auth_token_type auth_type = auth_unknown_token;
        user_auth_data_type auth_data_type = user_auth_data_undefined;
        firebase_core_auth_task_type task_type = firebase_core_auth_task_type_undefined;
        auth_status status;
        TimeStatusCallback timestatus_cb = NULL;
        file_config_data file_data;
    };

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

        static bool parseUserFile(token_type type, FILEOBJ userfile, user_auth_data &auth_data)
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

                String tokens[tokenSize];
                char *p = reinterpret_cast<char *>(mem.alloc(buff.length()));
                strcpy(p, buff.c_str());
                char *pp = p;
                char *end = p;
                String tmp;
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

        static bool saveUserFile(token_type type, FILEOBJ userfile, user_auth_data &auth_data)
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
                            userfile.print(FPSTR(","));
                    }
                    return true;
                }
                else if (type == token_type_access_token)
                {
#if defined(ENABLE_ACCESS_TOKEN)
                    for (size_t i = 0; i < access_tk_ns::max_type; i++)
                    {
                        userfile.print(auth_data.access_token.val[i].c_str());
                        userfile.print(FPSTR(","));
                    }
                    userfile.print(String(auth_data.access_token.expire).c_str());
#endif
                    return true;
                }
                else if (type == token_type_id_token)
                {

#if defined(ENABLE_ID_TOKEN)
                    userfile.print(auth_data.id_token.val[id_tk_ns::token].c_str());
                    userfile.print(FPSTR(","));
                    userfile.print(auth_data.id_token.val[id_tk_ns::refresh].c_str());
                    userfile.print(FPSTR(","));
                    userfile.print(auth_data.user.val[user_ns::api_key].c_str());
                    userfile.print(FPSTR(","));
                    userfile.print(String(auth_data.id_token.expire).c_str());
#endif
                    return true;
                }
                else if (type == token_type_custom_token)
                {
#if defined(ENABLE_CUSTOM_TOKEN)
                    userfile.print(auth_data.custom_token.val[cust_tk_ns::token].c_str());
                    userfile.print(FPSTR(","));
                    userfile.print(auth_data.user.val[user_ns::api_key].c_str());
                    userfile.print(FPSTR(","));
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

#if defined(ENABLE_SERVICE_AUTH) && defined(ENABLE_CUSTOM_AUTH)
    class SAParser
    {
    public:
        static bool parseSAFile(FILEOBJ safile, user_auth_data &auth_data)
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

#if defined(ENABLE_USER_AUTH)
    class UserAuth
    {
        friend class FirebaseApp;

    public:
        explicit UserAuth(const String &api_key, const String &email, const String &password, size_t expire = FIREBASE_DEFAULT_TOKEN_TTL)
        {
            data.clear();
            data.user.val[user_ns::api_key] = api_key;
            data.user.val[user_ns::em] = email;
            data.user.val[user_ns::psw] = password;
            data.auth_type = auth_user_id_token;
            data.auth_data_type = user_auth_data_user_data;
            data.user.expire = expire;
            data.initialized = true;
        };

        explicit UserAuth(const file_config_data &userFile)
        {
#if defined(ENABLE_FS)
            data.clear();
            if (userFile.initialized)
            {
                data.file_data.copy(userFile);
            }
#endif
        }

        bool save(file_config_data &userFile)
        {
            bool ret = false;
#if defined(ENABLE_FS)
            if (userFile.initialized)
            {
                userFile.cb(userFile.file, userFile.filename.c_str(), file_mode_open_write);
                if (userFile.file)
                {
                    ret = UserTokenFileParser::saveUserFile(UserTokenFileParser::token_type_user_data, userFile.file, data);
                    userFile.file.close();
                }
            }
#endif
            return ret;
        }

        ~UserAuth() { data.clear(); };
        void clear() { data.clear(); }
        user_auth_data &get()
        {
#if defined(ENABLE_FS)
            if (data.file_data.cb)
                data.file_data.cb(data.file_data.file, data.file_data.filename.c_str(), file_mode_open_read);
            if (data.file_data.file)
            {
                if (UserTokenFileParser::parseUserFile(UserTokenFileParser::token_type_user_data, data.file_data.file, data))
                    data.initialized = true;
                data.file_data.file.close();
            }
#endif
            return data;
        }

    private:
        user_auth_data data;
    };

#endif

#if defined(ENABLE_SERVICE_AUTH)
    class ServiceAuth
    {
        friend class FirebaseApp;

    public:
        explicit ServiceAuth(TimeStatusCallback timeCb, const String &clientEmail, const String &projectId, const String &privateKey, size_t expire = FIREBASE_DEFAULT_TOKEN_TTL)
        {
            data.clear();
            data.sa.val[sa_ns::cm] = clientEmail;
            data.sa.val[sa_ns::pid] = projectId;
            data.sa.val[sa_ns::pk] = privateKey;
            data.sa.expire = expire;
            data.initialized = isInitialized();
            data.auth_type = auth_sa_access_token;
            data.auth_data_type = user_auth_data_service_account;
            data.timestatus_cb = timeCb;
        };

        explicit ServiceAuth(TimeStatusCallback timeCb, const file_config_data &safile, size_t expire = FIREBASE_DEFAULT_TOKEN_TTL)
        {
#if defined(ENABLE_FS)
            data.clear();
            if (safile.initialized)
            {
                data.file_data.copy(safile);
                data.sa.expire = expire;
                data.timestatus_cb = timeCb;
            }
#endif
        }

        ~ServiceAuth() { data.clear(); };
        void clear() { data.clear(); }
        user_auth_data &get()
        {
#if defined(ENABLE_FS)
            if (data.file_data.cb)
                data.file_data.cb(data.file_data.file, data.file_data.filename.c_str(), file_mode_open_read);
            if (data.file_data.file)
            {
                bool ret = SAParser::parseSAFile(data.file_data.file, data);
                data.file_data.file.close();
                if (ret)
                {
                    data.initialized = isInitialized();
                    data.auth_type = auth_sa_access_token;
                    data.auth_data_type = user_auth_data_service_account;
                }
            }
#endif
            return data;
        }
        bool isInitialized() { return data.sa.val[sa_ns::cm].length() > 0 && data.sa.val[sa_ns::pid].length() > 0 && data.sa.val[sa_ns::pk].length() > 0; }

    private:
        user_auth_data data;
    };

#endif

#if defined(ENABLE_CUSTOM_AUTH)
    class CustomAuth
    {
        friend class FirebaseApp;

    public:
        explicit CustomAuth(TimeStatusCallback timeCb, const String &apiKey, const String &clientEmail, const String &projectId, const String &privateKey, const String &uid, const String &scope = "", const String &claims = "", size_t expire = FIREBASE_DEFAULT_TOKEN_TTL)
        {
            data.clear();
            data.sa.val[sa_ns::cm] = clientEmail;
            data.sa.val[sa_ns::pid] = projectId;
            data.sa.val[sa_ns::pk] = privateKey;
            data.sa.expire = expire;
            data.cust.val[cust_ns::api_key] = apiKey;
            data.cust.val[cust_ns::uid] = uid;
            data.cust.val[cust_ns::scope] = scope;
            data.cust.val[cust_ns::claims] = claims;
            data.initialized = isInitialized();
            data.auth_type = auth_sa_custom_token;
            data.auth_data_type = user_auth_data_custom_data;
            data.timestatus_cb = timeCb;
        };

        explicit CustomAuth(TimeStatusCallback timeCb, const file_config_data &safile, const String &apiKey, const String &uid, const String &scope = "", const String &claims = "", size_t expire = FIREBASE_DEFAULT_TOKEN_TTL)
        {
#if defined(ENABLE_FS)
            data.clear();
            if (safile.initialized)
            {
                data.file_data.copy(safile);
                data.sa.expire = expire;
                data.cust.val[cust_ns::api_key] = apiKey;
                data.cust.val[cust_ns::uid] = uid;
                data.cust.val[cust_ns::scope] = scope;
                data.cust.val[cust_ns::claims] = claims;
                data.timestatus_cb = timeCb;
            }
#endif
        }

        ~CustomAuth() { data.clear(); };

        user_auth_data &get()
        {
#if defined(ENABLE_FS)
            if (data.file_data.cb)
                data.file_data.cb(data.file_data.file, data.file_data.filename.c_str(), file_mode_open_read);
            if (data.file_data.file)
            {
                bool ret = SAParser::parseSAFile(data.file_data.file, data);
                data.file_data.file.close();
                if (ret)
                {
                    data.initialized = isInitialized();
                    data.auth_type = auth_sa_custom_token;
                    data.auth_data_type = user_auth_data_custom_data;
                }
            }
#endif
            return data;
        }

        bool isInitialized() { return data.sa.val[sa_ns::pk].length() > 0 && data.sa.val[sa_ns::cm].length() > 0 && data.sa.val[sa_ns::pid].length() > 0 && data.cust.val[cust_ns::uid].length() > 0; }

    private:
        user_auth_data data;
    };

#endif

#if defined(ENABLE_USER_AUTH) || defined(ENABLE_ID_TOKEN)
    class UserAccount
    {
        friend class FirebaseApp;

    public:
        explicit UserAccount(const String &apiKey)
        {
            data.user.val[user_ns::api_key] = apiKey;
            setType(auth_user_id_token, user_auth_data_user_data);
        }
        UserAccount &email(const String &email)
        {
            data.user.val[user_ns::em] = email;
            return setType(auth_user_id_token, user_auth_data_user_data);
        };
        UserAccount &password(const String &password)
        {
            data.user.val[user_ns::psw] = password;
            return setType(auth_user_id_token, user_auth_data_user_data);
        };
        UserAccount &idToken(const String &idToken)
        {
            data.user.val[user_ns::id_token] = idToken;
            return setType(auth_user_id_token, user_auth_data_user_data);
        };

        ~UserAccount() { data.clear(); };
        void clear() { data.clear(); }
        user_auth_data &get() { return data; }

    private:
        user_auth_data data;
        UserAccount &setType(auth_token_type auth_type, user_auth_data_type data_type)
        {
            data.auth_type = auth_type;
            data.auth_data_type = data_type;
            data.initialized = true;
            return *this;
        }
    };

#endif

#if defined(ENABLE_ID_TOKEN)
    class IDToken
    {
        friend class FirebaseApp;

    public:
        explicit IDToken(const String &api_key, const String &token, size_t expire = FIREBASE_DEFAULT_TOKEN_TTL, const String &refresh = "")
        {
            this->data.clear();
            this->data.user.val[user_ns::api_key] = api_key;
            this->data.id_token.val[id_tk_ns::token] = token;
            this->data.id_token.val[id_tk_ns::refresh] = refresh;
            this->data.id_token.expire = expire;
            this->data.initialized = true;
            this->data.auth_type = auth_user_id_token;
            this->data.auth_data_type = user_auth_data_id_token;
        }

        explicit IDToken(const file_config_data &tokenFile)
        {
#if defined(ENABLE_FS)
            data.clear();
            if (tokenFile.initialized)
            {
                data.file_data.copy(tokenFile);
            }
#endif
        }

        bool save(file_config_data &tokenFile)
        {
            bool ret = false;
#if defined(ENABLE_FS)
            if (tokenFile.initialized)
            {
                tokenFile.cb(tokenFile.file, tokenFile.filename.c_str(), file_mode_open_write);
                if (tokenFile.file)
                {
                    ret = UserTokenFileParser::saveUserFile(UserTokenFileParser::token_type_id_token, tokenFile.file, data);
                    tokenFile.file.close();
                }
            }
#endif
            return ret;
        }

        void clear() { data.clear(); }
        user_auth_data &get()
        {
#if defined(ENABLE_FS)
            if (data.file_data.cb)
                data.file_data.cb(data.file_data.file, data.file_data.filename.c_str(), file_mode_open_read);
            if (data.file_data.file)
            {
                if (UserTokenFileParser::parseUserFile(UserTokenFileParser::token_type_id_token, data.file_data.file, data))
                {
                    data.initialized = true;
                    data.auth_type = auth_id_token;
                    data.auth_data_type = user_auth_data_id_token;
                }
                data.file_data.file.close();
            }
#endif
            return data;
        }

    private:
        user_auth_data data;
    };

#endif

#if defined(ENABLE_ACCESS_TOKEN)

    class AccessToken
    {
        friend class FirebaseApp;

    public:
        explicit AccessToken(const String &token, size_t expire = FIREBASE_DEFAULT_TOKEN_TTL, const String &refresh = "", const String &client_id = "", const String &client_secret = "")
        {
            this->data.clear();
            this->data.access_token.val[access_tk_ns::token] = token;
            this->data.access_token.val[access_tk_ns::refresh] = refresh;
            this->data.access_token.val[access_tk_ns::cid] = client_id;
            this->data.access_token.val[access_tk_ns::csec] = client_secret;
            this->data.access_token.expire = expire;
            this->data.initialized = true;
            this->data.auth_type = auth_access_token;
            this->data.auth_data_type = user_auth_data_access_token;
        }

        explicit AccessToken(const file_config_data &tokenFile)
        {
#if defined(ENABLE_FS)
            data.clear();
            if (tokenFile.initialized)
            {
                data.file_data.copy(tokenFile);
            }
#endif
        }

        bool save(file_config_data &tokenFile)
        {
            bool ret = false;
#if defined(ENABLE_FS)
            if (tokenFile.initialized)
            {
                tokenFile.cb(tokenFile.file, tokenFile.filename.c_str(), file_mode_open_write);
                if (tokenFile.file)
                {
                    ret = UserTokenFileParser::saveUserFile(UserTokenFileParser::token_type_access_token, tokenFile.file, data);
                    tokenFile.file.close();
                }
            }
#endif
            return ret;
        }

        void clear() { data.clear(); }
        user_auth_data &get()
        {
#if defined(ENABLE_FS)
            if (data.file_data.cb)
                data.file_data.cb(data.file_data.file, data.file_data.filename.c_str(), file_mode_open_read);
            if (data.file_data.file)
            {
                if (UserTokenFileParser::parseUserFile(UserTokenFileParser::token_type_access_token, data.file_data.file, data))
                {
                    data.initialized = true;
                    data.auth_type = auth_access_token;
                    data.auth_data_type = user_auth_data_access_token;
                }
                data.file_data.file.close();
            }
#endif
            return data;
        }

    private:
        user_auth_data data;
    };

#endif

#if defined(ENABLE_CUSTOM_TOKEN)

    class CustomToken
    {
        friend class FirebaseApp;

    public:
        explicit CustomToken(const String &api_key, const String &token, size_t expire = FIREBASE_DEFAULT_TOKEN_TTL)
        {
            this->data.clear();
            this->data.custom_token.val[cust_tk_ns::token] = token;
            this->data.user.val[user_ns::api_key] = api_key;
            this->data.custom_token.expire = expire;
            this->data.initialized = data.custom_token.val[cust_tk_ns::token].length() > 0 && data.user.val[user_ns::api_key].length() > 0;
            this->data.auth_type = auth_custom_token;
            this->data.auth_data_type = user_auth_data_custom_token;
        }

        explicit CustomToken(const file_config_data &tokenFile)
        {
            data.clear();
            if (tokenFile.initialized)
            {
#if defined(ENABLE_FS)
                data.file_data.copy(tokenFile);
#endif
            }
        }

        bool save(file_config_data &tokenFile)
        {
            bool ret = false;
#if defined(ENABLE_FS)
            if (tokenFile.initialized)
            {
                tokenFile.cb(tokenFile.file, tokenFile.filename.c_str(), file_mode_open_write);
                if (tokenFile.file)
                {
                    ret = UserTokenFileParser::saveUserFile(UserTokenFileParser::token_type_custom_token, tokenFile.file, data);
                    tokenFile.file.close();
                }
            }
#endif
            return ret;
        }

        void clear() { data.clear(); }
        user_auth_data &get()
        {
#if defined(ENABLE_FS)
            if (data.file_data.cb)
                data.file_data.cb(data.file_data.file, data.file_data.filename.c_str(), file_mode_open_read);
            if (data.file_data.file)
            {
                if (UserTokenFileParser::parseUserFile(UserTokenFileParser::token_type_custom_token, data.file_data.file, data))
                {
                    data.initialized = data.custom_token.val[cust_tk_ns::token].length() > 0 && data.user.val[user_ns::api_key].length() > 0;
                    data.auth_type = auth_custom_token;
                    data.auth_data_type = user_auth_data_custom_token;
                }
                data.file_data.file.close();
            }
#endif
            return data;
        }

    private:
        user_auth_data data;
    };
#endif

#if defined(ENABLE_LEGACY_TOKEN)
    class LegacyToken
    {
        friend class FirebaseApp;

    public:
        explicit LegacyToken(const String &token)
        {
            this->data.clear();
            this->data.legacy_token.val[legacy_tk_ns::token] = token;
            this->data.initialized = true;
            this->data.auth_type = auth_unknown_token;
            this->data.auth_data_type = user_auth_data_legacy_token;
        }

        explicit LegacyToken(const file_config_data &tokenFile)
        {
#if defined(ENABLE_FS)
            data.clear();
            if (tokenFile.initialized)
            {
                data.file_data.copy(tokenFile);
            }
#endif
        }

        bool save(file_config_data &tokenFile)
        {
            bool ret = false;
#if defined(ENABLE_FS)
            if (tokenFile.initialized)
            {
                tokenFile.cb(tokenFile.file, tokenFile.filename.c_str(), file_mode_open_write);
                if (tokenFile.file)
                {
                    ret = UserTokenFileParser::saveUserFile(UserTokenFileParser::token_type_legacy_token, tokenFile.file, data);
                    tokenFile.file.close();
                }
            }
#endif
            return ret;
        }

        void clear() { data.clear(); }
        user_auth_data &get()
        {
#if defined(ENABLE_FS)
            if (data.file_data.cb)
                data.file_data.cb(data.file_data.file, data.file_data.filename.c_str(), file_mode_open_read);
            if (data.file_data.file)
            {
                if (UserTokenFileParser::parseUserFile(UserTokenFileParser::token_type_legacy_token, data.file_data.file, data))
                {
                    data.initialized = true;
                    data.auth_type = auth_unknown_token;
                    data.auth_data_type = user_auth_data_legacy_token;
                }
                data.file_data.file.close();
            }
#endif
            return data;
        }

    private:
        user_auth_data data;
    };

#endif

    class NoAuth
    {
        friend class FirebaseApp;

    public:
        NoAuth()
        {
            this->data.clear();
            this->data.initialized = true;
            this->data.auth_type = auth_unknown_token;
            this->data.auth_data_type = user_auth_data_no_token;
        }
        user_auth_data &get() { return data; }

    private:
        user_auth_data data;
    };

    struct app_token_t
    {
    public:
        String val[app_tk_ns::max_type];
        uint32_t expire = 0, auth_ts = 0;
        bool authenticated = false;
        auth_token_type auth_type = auth_unknown_token;
        user_auth_data_type auth_data_type = user_auth_data_undefined;
        void clear()
        {
            for (size_t i = 0; i < app_tk_ns::max_type; i++)
                val[i].remove(0, val[i].length());
            expire = FIREBASE_DEFAULT_TOKEN_TTL;
            authenticated = false;
            auth_type = auth_unknown_token;
            auth_data_type = user_auth_data_undefined;
        }
        app_token_t() {}
    };

    struct auth_data_t
    {
        user_auth_data user_auth;
        app_token_t app_token;
        AsyncResultCallback cb;
        AsyncResult *refResult = nullptr;
    };

};

#endif