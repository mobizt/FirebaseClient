#ifndef CORE_AUTH_CONFIG_H
#define CORE_AUTH_CONFIG_H
#include <Arduino.h>
#include "Storage.h"
#include "StringHelper.h"
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
        auth_event_ready = 10,
        auth_event_error = 11
    };

    enum jwt_step
    {
        jwt_step_begin,
        jwt_step_encode_header_payload,
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
        firebase_auth_event_type event() { return _event; }
    };

    typedef void (*TimeStatusCallback)(uint32_t &ts);

    struct user_auth_data
    {
        friend class SAParser;
        friend class UserAuth;
        friend class ServiceAuth;
        friend class CustomAuth;
        friend class AppCore;
        friend class IDToken;
        friend class AccessToken;
        friend class CustomToken;
        friend class AuthRequest;
        friend class JWT;
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
            friend class JWT;
            friend class FirebaseClient;

        public:
            service_account() {}
            ~service_account() { clear(); }
            void copy(service_account &rhs)
            {
                this->client_email = rhs.client_email;
                this->project_id = rhs.project_id;
                this->private_key = rhs.private_key;
                this->private_key_id = rhs.private_key_id;
                this->client_id = rhs.client_id;
                this->timestatus_cb = rhs.timestatus_cb;
                this->expire = rhs.expire;
            }

            void clear()
            {
                client_email.clear();
                project_id.clear();
                private_key.clear();
                private_key_id.clear();
                client_id.clear();
                timestatus_cb = NULL;
                expire= 3600;
            }

        protected:
            String client_email;
            String project_id;
            String private_key;
            String private_key_id;
            String client_id;
            jwt_step step = jwt_step_begin;
            TimeStatusCallback timestatus_cb = NULL;
            size_t expire = 3600;
        };
#endif

#if defined(ENABLE_CUSTOM_AUTH)
        struct custom_data
        {
            // for custom claims
            String api_key;
            String uid;
            String scope;
            String claims;
            TimeStatusCallback timestatus_cb = NULL;

        public:
            custom_data() {}
            ~custom_data() { clear(); }
            void copy(custom_data &rhs)
            {
                this->api_key = rhs.api_key;
                this->uid = rhs.uid;
                this->scope = rhs.scope;
                this->claims = rhs.claims;
                this->timestatus_cb = rhs.timestatus_cb;
            }

            void clear()
            {
                api_key.clear();
                uid.clear();
                scope.clear();
                claims.clear();
                timestatus_cb = NULL;
            }
        };
#endif

        struct user_data
        {
            String email;
            String password;
            String api_key;
            // used with apiKey for current user verification and delete
            String id_token;
            size_t expire = 3600;

        public:
            user_data() {}
            ~user_data() { clear(); }
            void copy(user_data &rhs)
            {
                this->email = rhs.email;
                this->password = rhs.password;
                this->api_key = rhs.api_key;
                this->id_token = rhs.id_token;
                this->expire = rhs.expire;
            }
            void clear()
            {
                email.clear();
                password.clear();
                api_key.clear();
            }
        };

#if defined(ENABLE_ID_TOKEN)
        struct id_token_data
        {
            String token;
            String refresh;
            size_t expire = 3600;

        public:
            id_token_data() {}
            ~id_token_data() { clear(); }
            void copy(id_token_data &rhs)
            {
                this->token = rhs.token;
                this->refresh = rhs.refresh;
                this->expire = rhs.expire;
            }
            void clear()
            {
                token.clear();
                refresh.clear();
                expire = 3600;
            }
        };
#endif

#if defined(ENABLE_ACCESS_TOKEN)
        struct access_token_data
        {
            String token;
            String refresh;
            String client_id;
            String client_secret;
            size_t expire = 3600;
            TimeStatusCallback timestatus_cb = NULL;

        public:
            access_token_data() {}
            ~access_token_data() { clear(); }
            void copy(access_token_data &rhs)
            {
                this->token = rhs.token;
                this->refresh = rhs.refresh;
                this->client_id = rhs.client_id;
                this->client_secret = rhs.client_secret;
                this->expire = rhs.expire;
                this->timestatus_cb = rhs.timestatus_cb;
            }
            void clear()
            {
                token.clear();
                refresh.clear();
                client_id.clear();
                client_secret.clear();
                expire = 3600;
                timestatus_cb = NULL;
            }
        };
#endif

#if defined(ENABLE_CUSTOM_TOKEN)
        struct custom_token_data
        {
            String token;
            size_t expire = 3600;
            TimeStatusCallback timestatus_cb = NULL;

        public:
            custom_token_data() {}
            ~custom_token_data() { clear(); }
            void copy(custom_token_data &rhs)
            {
                this->token = rhs.token;
                this->expire = rhs.expire;
                this->timestatus_cb = rhs.timestatus_cb;
            }
            void clear()
            {
                token.clear();
                expire = 3600;
                timestatus_cb = NULL;
            }
        };
#endif

#if defined(ENABLE_LEGACY_TOKEN)
        struct legacy_token_data
        {
            String token;

        public:
            legacy_token_data() {}
            ~legacy_token_data() { clear(); }
            void copy(legacy_token_data &rhs) { this->token = rhs.token; }
            void clear() { token.clear(); }
        };
#endif

    public:
        user_auth_data() {}
        ~user_auth_data() { clear(); }
        user_auth_data &operator=(user_auth_data &rhs)
        {
            copy(rhs);
            return *this;
        }
        void copy(user_auth_data &rhs)
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
#if defined(ENABLE_SERVICE_AUTH)
            sa.clear();
#endif

#if defined(ENABLE_CUSTOM_AUTH)
            cust.clear();
#endif

            user.clear();

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
#if defined(ENABLE_SERVICE_AUTH)
        service_account sa;
#endif
#if defined(ENABLE_CUSTOM_AUTH)
        custom_data cust;
#endif
        user_data user;

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
        auth_token_type auth_type = auth_unknown_token;
        user_auth_data_type auth_data_type = user_auth_data_undefined;
        firebase_core_auth_task_type task_type = firebase_core_auth_task_type_undefined;
        auth_status status;
        TimeStatusCallback timestatus_cb = NULL;
    };

    struct token_info_t
    {
        String jwt_token;
        String auth_token;
        String refresh_token;
        String uid;
        String header_auth_type;
        uint32_t expire = 0;

    public:
        token_info_t() {}
        ~token_info_t() { clear(); }
        void clear()
        {
            jwt_token.clear();
            auth_token.clear();
            refresh_token.clear();
            uid.clear();
            header_auth_type.clear();
            expire = 0;
        }

        void copy(token_info_t &rhs)
        {
            this->jwt_token = rhs.jwt_token;
            this->auth_token = rhs.auth_token;
            this->refresh_token = rhs.refresh_token;
            this->uid = rhs.uid;
            this->header_auth_type = rhs.header_auth_type;
            this->expire = rhs.expire;
        }
    };

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
                StringHelper sh;
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
                    sh.strsepImpl(&end, ",");
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
                    auth_data.user.api_key = tokens[0];
                    auth_data.user.email = tokens[1];
                    auth_data.user.password = tokens[2];
                    return true;
                }
                else if (type == token_type_access_token && tokenSize == 5)
                {
#if defined(ENABLE_ACCESS_TOKEN)
                    auth_data.access_token.token = tokens[0];
                    auth_data.access_token.refresh = tokens[1];
                    auth_data.access_token.client_id = tokens[2];
                    auth_data.access_token.client_secret = tokens[3];
                    auth_data.access_token.expire = atoi(tokens[4].c_str());
#endif
                    return true;
                }
                else if (type == token_type_id_token && tokenSize == 4)
                {
#if defined(ENABLE_ID_TOKEN)
                    auth_data.user.api_key = tokens[0];
                    auth_data.id_token.token = tokens[1];
                    auth_data.id_token.refresh = tokens[2];
                    auth_data.id_token.expire = atoi(tokens[3].c_str());
#endif
                }
                else if (type == token_type_custom_token && tokenSize == 3)
                {
                    auth_data.user.api_key = tokens[0];
#if defined(ENABLE_CUSTOM_TOKEN)
                    auth_data.custom_token.token = tokens[1];
                    auth_data.custom_token.expire = atoi(tokens[2].c_str());
#endif
                }
                else if (type == token_type_legacy_token && tokenSize == 1)
                {
#if defined(ENABLE_LEGACY_TOKEN)
                    auth_data.legacy_token.token = tokens[0];
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
                    userfile.print(auth_data.user.api_key.c_str());
                    userfile.print(",");
                    userfile.print(auth_data.user.email.c_str());
                    userfile.print(",");
                    userfile.print(auth_data.user.password.c_str());
                    return true;
                }
                else if (type == token_type_access_token)
                {
#if defined(ENABLE_ACCESS_TOKEN)
                    userfile.print(auth_data.access_token.token.c_str());
                    userfile.print(",");
                    userfile.print(auth_data.access_token.refresh.c_str());
                    userfile.print(",");
                    userfile.print(auth_data.access_token.client_id.c_str());
                    userfile.print(",");
                    userfile.print(auth_data.access_token.client_secret.c_str());
                    userfile.print(",");
                    userfile.print(String(auth_data.access_token.expire).c_str());
#endif
                    return true;
                }
                else if (type == token_type_id_token)
                {
                    userfile.print(auth_data.user.api_key.c_str());
                    userfile.print(",");
#if defined(ENABLE_ID_TOKEN)
                    userfile.print(auth_data.id_token.token.c_str());
                    userfile.print(",");
                    userfile.print(auth_data.id_token.refresh.c_str());
                    userfile.print(",");
                    userfile.print(String(auth_data.id_token.expire).c_str());
#endif
                    return true;
                }
                else if (type == token_type_custom_token)
                {
                    userfile.print(auth_data.user.api_key.c_str());
                    userfile.print(",");
#if defined(ENABLE_CUSTOM_TOKEN)
                    userfile.print(auth_data.custom_token.token.c_str());
                    userfile.print(",");
                    userfile.print(auth_data.custom_token.expire);
#endif
                    return true;
                }
                else if (type == token_type_legacy_token)
                {
#if defined(ENABLE_LEGACY_TOKEN)
                    userfile.print(auth_data.legacy_token.token.c_str());
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

                StringHelper sh;
                int p1 = 0, p2 = 0;
                sh.parse(buf, "\"project_id\"", ",", p1, p2);
                if (p1 > -1 && p2 > -1)
                {
                    auth_data.sa.project_id = buf.substring(p1 + 1, p2 - 1);
                    p1 = p2;
                }
                sh.parse(buf, "\"private_key_id\"", ",", p1, p2);
                if (p1 > -1 && p2 > -1)
                {
                    auth_data.sa.private_key_id = buf.substring(p1 + 1, p2 - 1);
                    p1 = p2;
                }
                sh.parse(buf, "\"private_key\"", ",", p1, p2);
                if (p1 > -1 && p2 > -1)
                {
                    auth_data.sa.private_key = buf.substring(p1 + 1, p2 - 1);
                    p1 = p2;
                    auth_data.initialized = true;
                    ret = true;
                }
                sh.parse(buf, "\"client_email\"", ",", p1, p2);
                if (p1 > -1 && p2 > -1)
                {
                    auth_data.sa.client_email = buf.substring(p1 + 1, p2 - 1);
                    p1 = p2;
                }
                sh.parse(buf, "\"client_id\"", ",", p1, p2);
                if (p1 > -1 && p2 > -1)
                {
                    auth_data.sa.client_id = buf.substring(p1 + 1, p2 - 1);
                    p1 = p2;
                }
            }

            return ret;
        }
    };

#endif

#if defined(ENABLE_USER_AUTH)
    class UserAuth
    {
        friend class Firebase;
        friend class FirebaseApp;
        friend class AsyncFirebaseClient;

    public:
        UserAuth(const String &api_key, const String &email, const String &password, size_t expire = 3600)
        {
            data.clear();
            data.user.api_key = api_key;
            data.user.email = email;
            data.user.password = password;
            data.auth_type = auth_user_id_token;
            data.auth_data_type = user_auth_data_user_data;
            data.user.expire = expire;
            data.initialized = true;
        };

        UserAuth(file_config_data &userFile)
        {
            data.clear();
            if (userFile.initialized)
            {
                userFile.cb(userFile.file, userFile.filename.c_str(), file_mode_open_read);
                if (userFile.file)
                {
                    if (UserTokenFileParser::parseUserFile(UserTokenFileParser::token_type_user_data, userFile.file, data))
                    {

                        data.initialized = true;
                    }
                    userFile.file.close();
                }
            }
        }

        bool save(file_config_data &userFile)
        {
            bool ret = false;
            if (userFile.initialized)
            {
                userFile.cb(userFile.file, userFile.filename.c_str(), file_mode_open_write);
                if (userFile.file)
                {
                    ret = UserTokenFileParser::saveUserFile(UserTokenFileParser::token_type_user_data, userFile.file, data);
                    userFile.file.close();
                }
            }
            return ret;
        }

        void clear()
        {
            data.clear();
        }

        ~UserAuth() { data.clear(); };

        user_auth_data &get() { return data; }

    private:
        user_auth_data data;
    };

#endif

#if defined(ENABLE_SERVICE_AUTH)
    class ServiceAuth
    {
        friend class Firebase;
        friend class FirebaseApp;
        friend class AsyncFirebaseClient;

    public:
        ServiceAuth(TimeStatusCallback cb, const String &clientEmail, const String &projectId, const String &privateKey, size_t expire = 3600)
        {
            data.clear();
            data.sa.client_email = clientEmail;
            data.sa.project_id = projectId;
            data.sa.private_key = privateKey;
            data.sa.expire = expire;
            data.initialized = isInitialized();
            data.auth_type = auth_sa_access_token;
            data.auth_data_type = user_auth_data_service_account;
            data.timestatus_cb = cb;
        };

        ServiceAuth(file_config_data &safile, TimeStatusCallback cb)
        {
            data.clear();
            if (safile.initialized)
            {
                safile.cb(safile.file, safile.filename.c_str(), file_mode_open_read);
                if (safile.file)
                {
                    SAParser::parseSAFile(safile.file, data);
                    safile.file.close();
                }
                data.timestatus_cb = cb;
            }
        }

        ~ServiceAuth() { data.clear(); };

        user_auth_data &get() { return data; }

        bool isInitialized() { return data.sa.client_email.length() > 0 && data.sa.project_id.length() > 0 && data.sa.private_key.length() > 0; }

    private:
        user_auth_data data;
    };

#endif

#if defined(ENABLE_CUSTOM_AUTH)
    class CustomAuth
    {
        friend class Firebase;
        friend class FirebaseApp;
        friend class AsyncFirebaseClient;

    public:
        CustomAuth(TimeStatusCallback cb, const String &apiKey, const String &clientEmail, const String &projectId, const String &privateKey, const String &uid, const String &scope = "", const String &claims = "", size_t expire = 3600)
        {
            data.clear();
            data.sa.client_email = clientEmail;
            data.sa.project_id = projectId;
            data.sa.private_key = privateKey;
            data.sa.expire = expire;
            data.cust.api_key = apiKey;
            data.cust.uid = uid;
            data.cust.scope = scope;
            data.cust.claims = claims;
            data.initialized = isInitialized();
            data.auth_type = auth_sa_custom_token;
            data.auth_data_type = user_auth_data_custom_data;
            data.timestatus_cb = cb;
        };

        CustomAuth(TimeStatusCallback cb, file_config_data &safile, const String &uid)
        {
            data.clear();
            if (safile.initialized)
            {
                safile.cb(safile.file, safile.filename.c_str(), file_mode_open_read);
                if (safile.file)
                {
                    if (SAParser::parseSAFile(safile.file, data))
                    {
                        data.cust.uid = uid;
                        data.auth_type = auth_sa_custom_token;
                        data.auth_data_type = user_auth_data_custom_data;
                    }
                    safile.file.close();
                    data.timestatus_cb = cb;
                }
            }
        }

        ~CustomAuth() { data.clear(); };

        user_auth_data &get() { return data; }

        bool isInitialized()
        {
            return data.sa.private_key.length() > 0 && data.sa.client_email.length() > 0 && data.sa.project_id.length() > 0 && data.sa.private_key.length() > 0 && data.cust.uid.length() > 0;
        }

    private:
        user_auth_data data;
    };

#endif

#if defined(ENABLE_USER_AUTH) || defined(ENABLE_ID_TOKEN)
    class UserAccount
    {
        friend class Firebase;
        friend class FirebaseApp;
        friend class AsyncFirebaseClient;

    public:
        UserAccount(const String &apiKey)
        {
            data.user.api_key = apiKey;
            data.auth_type = auth_user_id_token;
            data.auth_data_type = user_auth_data_user_data;
            data.initialized = true;
        }
        UserAccount &email(const String &email)
        {
            data.user.email = email;
            data.auth_type = auth_user_id_token;
            data.auth_data_type = user_auth_data_user_data;
            data.initialized = true;
            return *this;
        };

        UserAccount &password(const String &password)
        {
            data.user.password = password;
            data.auth_type = auth_user_id_token;
            data.auth_data_type = user_auth_data_user_data;
            data.initialized = true;
            return *this;
        };

        UserAccount &idToken(const String &idToken)
        {
            data.user.id_token = idToken;
            data.auth_type = auth_user_id_token;
            data.auth_data_type = user_auth_data_user_data;
            data.initialized = true;
            return *this;
        };

        void clear()
        {
            data.clear();
        }

        ~UserAccount() { data.clear(); };

        user_auth_data &get() { return data; }

    private:
        user_auth_data data;
    };

#endif

#if defined(ENABLE_ID_TOKEN)
    class IDToken
    {
        friend class Firebase;
        friend class FirebaseApp;
        friend class AsyncFirebaseClient;

    public:
        IDToken(const String &api_key, const String &token, size_t expire = 3600, const String &refresh = "")
        {
            this->data.clear();
            this->data.user.api_key = api_key;
            this->data.id_token.token = token;
            this->data.id_token.refresh = refresh;
            this->data.id_token.expire = expire;
            this->data.initialized = true;
            this->data.auth_type = auth_user_id_token;
            this->data.auth_data_type = user_auth_data_id_token;
        }

        IDToken(file_config_data &tokenFile)
        {
            data.clear();
            if (tokenFile.initialized)
            {
                tokenFile.cb(tokenFile.file, tokenFile.filename.c_str(), file_mode_open_read);
                if (tokenFile.file)
                {
                    if (UserTokenFileParser::parseUserFile(UserTokenFileParser::token_type_id_token, tokenFile.file, data))
                    {
                        data.initialized = true;
                        data.auth_type = auth_id_token;
                        data.auth_data_type = user_auth_data_id_token;
                    }
                    tokenFile.file.close();
                }
            }
        }

        bool save(file_config_data &tokenFile)
        {
            bool ret = false;
            if (tokenFile.initialized)
            {
                tokenFile.cb(tokenFile.file, tokenFile.filename.c_str(), file_mode_open_write);
                if (tokenFile.file)
                {
                    ret = UserTokenFileParser::saveUserFile(UserTokenFileParser::token_type_id_token, tokenFile.file, data);
                    tokenFile.file.close();
                }
            }
            return ret;
        }

        void clear()
        {
            data.clear();
        }

        user_auth_data &get() { return data; }

    private:
        user_auth_data data;
    };

#endif

#if defined(ENABLE_ACCESS_TOKEN)

    class AccessToken
    {
        friend class Firebase;
        friend class FirebaseApp;
        friend class AsyncFirebaseClient;

    public:
        AccessToken(const String &token, size_t expire = 3600, const String &refresh = "", const String &client_id = "", const String &client_secret = "")
        {
            this->data.clear();
            this->data.access_token.token = token;
            this->data.access_token.refresh = refresh;
            this->data.access_token.client_id = client_id;
            this->data.access_token.client_secret = client_secret;
            this->data.access_token.expire = expire;
            this->data.initialized = true;
            this->data.auth_type = auth_access_token;
            this->data.auth_data_type = user_auth_data_access_token;
        }

        AccessToken(file_config_data &tokenFile)
        {
            data.clear();
            if (tokenFile.initialized)
            {
                tokenFile.cb(tokenFile.file, tokenFile.filename.c_str(), file_mode_open_read);
                if (tokenFile.file)
                {
                    if (UserTokenFileParser::parseUserFile(UserTokenFileParser::token_type_access_token, tokenFile.file, data))
                    {
                        data.initialized = true;
                        data.auth_type = auth_access_token;
                        data.auth_data_type = user_auth_data_access_token;
                    }
                    tokenFile.file.close();
                }
            }
        }

        bool save(file_config_data &tokenFile)
        {
            bool ret = false;
            if (tokenFile.initialized)
            {
                tokenFile.cb(tokenFile.file, tokenFile.filename.c_str(), file_mode_open_write);
                if (tokenFile.file)
                {
                    ret = UserTokenFileParser::saveUserFile(UserTokenFileParser::token_type_access_token, tokenFile.file, data);
                    tokenFile.file.close();
                }
            }
            return ret;
        }

        void clear()
        {
            data.clear();
        }

        user_auth_data &get() { return data; }

    private:
        user_auth_data data;
    };

#endif

#if defined(ENABLE_CUSTOM_TOKEN)

    class CustomToken
    {
        friend class Firebase;
        friend class FirebaseApp;
        friend class AsyncFirebaseClient;

    public:
        CustomToken(const String &api_key, const String &token, size_t expire = 3600)
        {
            this->data.clear();
            this->data.custom_token.token = token;
            this->data.user.api_key = api_key;
            this->data.custom_token.expire = expire;
            this->data.initialized = data.custom_token.token.length() > 0 && data.user.api_key.length() > 0;
            this->data.auth_type = auth_custom_token;
            this->data.auth_data_type = user_auth_data_custom_token;
        }

        CustomToken(TimeStatusCallback cb, file_config_data &tokenFile)
        {
            data.clear();
            if (tokenFile.initialized)
            {
                tokenFile.cb(tokenFile.file, tokenFile.filename.c_str(), file_mode_open_read);
                if (tokenFile.file)
                {
                    if (UserTokenFileParser::parseUserFile(UserTokenFileParser::token_type_custom_token, tokenFile.file, data))
                    {
                        data.initialized = data.custom_token.token.length() > 0 && data.user.api_key.length() > 0;
                        data.auth_type = auth_custom_token;
                        data.auth_data_type = user_auth_data_custom_token;
                    }
                    tokenFile.file.close();
                    data.timestatus_cb = cb;
                }
            }
        }

        bool save(file_config_data &tokenFile)
        {
            bool ret = false;
            if (tokenFile.initialized)
            {
                tokenFile.cb(tokenFile.file, tokenFile.filename.c_str(), file_mode_open_write);
                if (tokenFile.file)
                {
                    ret = UserTokenFileParser::saveUserFile(UserTokenFileParser::token_type_custom_token, tokenFile.file, data);
                    tokenFile.file.close();
                }
            }
            return ret;
        }

        void clear()
        {
            data.clear();
        }

        user_auth_data &get() { return data; }

    private:
        user_auth_data data;
    };
#endif

#if defined(ENABLE_LEGACY_TOKEN)
    class LegacyToken
    {
        friend class Firebase;
        friend class FirebaseApp;
        friend class AsyncFirebaseClient;

    public:
        LegacyToken(const String &token)
        {
            this->data.clear();
            this->data.legacy_token.token = token;
            this->data.initialized = true;
            this->data.auth_type = auth_unknown_token;
            this->data.auth_data_type = user_auth_data_legacy_token;
        }

        LegacyToken(file_config_data &tokenFile)
        {
            data.clear();
            if (tokenFile.initialized)
            {
                tokenFile.cb(tokenFile.file, tokenFile.filename.c_str(), file_mode_open_read);
                if (tokenFile.file)
                {
                    if (UserTokenFileParser::parseUserFile(UserTokenFileParser::token_type_legacy_token, tokenFile.file, data))
                    {
                        data.initialized = true;
                        data.auth_type = auth_unknown_token;
                        data.auth_data_type = user_auth_data_legacy_token;
                    }
                    tokenFile.file.close();
                }
            }
        }

        bool save(file_config_data &tokenFile)
        {
            bool ret = false;
            if (tokenFile.initialized)
            {
                tokenFile.cb(tokenFile.file, tokenFile.filename.c_str(), file_mode_open_write);
                if (tokenFile.file)
                {
                    ret = UserTokenFileParser::saveUserFile(UserTokenFileParser::token_type_legacy_token, tokenFile.file, data);
                    tokenFile.file.close();
                }
            }
            return ret;
        }

        void clear()
        {
            data.clear();
        }

        user_auth_data &get() { return data; }

    private:
        user_auth_data data;
    };

#endif

    class NoAuth
    {
        friend class Firebase;
        friend class FirebaseApp;
        friend class AsyncFirebaseClient;

    public:
        NoAuth()
        {
            this->data.clear();
            this->data.initialized = true;
            this->data.auth_type = auth_unknown_token;
            this->data.auth_data_type = user_auth_data_undefined;
        }
        user_auth_data &get() { return data; }

    private:
        user_auth_data data;
    };

    struct app_token_t
    {
    public:
        String token_type;
        String token;
        String refresh;
        String uid;
        uint32_t expire = 0;
        bool authenticated = false;
        auth_token_type auth_type = auth_unknown_token;
        user_auth_data_type auth_data_type = user_auth_data_undefined;
        void clear()
        {
            token_type = "";
            token = "";
            refresh = "";
            uid = "";
            expire = 0;
            authenticated = false;
            auth_type = auth_unknown_token;
            auth_data_type = user_auth_data_undefined;
        }
    };

    struct auth_data_t
    {
        user_auth_data user_auth;
        app_token_t app_token;
    };

};

#endif