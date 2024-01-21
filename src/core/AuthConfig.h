#ifndef CORE_AUTH_CONFIG_H
#define CORE_AUTH_CONFIG_H
#include <Arduino.h>
#include "Storage.h"
#include "StringHelper.h"

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
    friend class AsyncFirebaseClient;
    friend class DatabaseAppRequest;
    friend class FirebaseApp;
    friend class Database;
    friend class Messaging;
    friend class Firestore;
    friend class Storage;
    friend class Functions;
    friend class CloudStorage;
    friend class UserTokenFileParser;
    friend class LegacyToken;
    friend class NoAuth;
    friend class UserAccount;

    struct service_account
    {
        friend class SAParser;
        friend class user_auth_data;
        friend class CustomAuth;
        friend class ServiceAuth;
        friend class AppCore;
        friend class JWT;

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
        }

        void clear()
        {
            client_email.clear();
            project_id.clear();
            private_key.clear();
            private_key_id.clear();
            client_id.clear();
        }

    protected:
        String client_email;
        String project_id;
        String private_key;
        String private_key_id;
        String client_id;
        jwt_step step = jwt_step_begin;
    };

    struct custom_data
    {
        // for custom claims
        String api_key;
        String uid;
        String scope;
        String claims;

    public:
        custom_data() {}
        ~custom_data() { clear(); }
        void copy(custom_data &rhs)
        {
            this->api_key = rhs.api_key;
            this->uid = rhs.uid;
            this->scope = rhs.scope;
            this->claims = rhs.claims;
        }

        void clear()
        {
            api_key.clear();
            uid.clear();
            scope.clear();
            claims.clear();
        }
    };

    struct user_data
    {
        String email;
        String password;
        String api_key;
        // used with apiKey for current user verification and delete
        String id_token;

    public:
        user_data() {}
        ~user_data() { clear(); }
        void copy(user_data &rhs)
        {
            this->email = rhs.email;
            this->password = rhs.password;
            this->api_key = rhs.api_key;
            this->id_token = rhs.id_token;
        }
        void clear()
        {
            email.clear();
            password.clear();
            api_key.clear();
        }
    };

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

    struct access_token_data
    {
        String token;
        String refresh;
        String client_id;
        String client_secret;
        size_t expire = 3600;

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
        }
        void clear()
        {
            token.clear();
            refresh.clear();
            client_id.clear();
            client_secret.clear();
            expire = 3600;
        }
    };

    struct custom_token_data
    {
        String token;
        String refresh;
        size_t expire = 3600;

    public:
        custom_token_data() {}
        ~custom_token_data() { clear(); }
        void copy(custom_token_data &rhs)
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

    struct legacy_token_data
    {
        String token;

    public:
        legacy_token_data() {}
        ~legacy_token_data() { clear(); }
        void copy(legacy_token_data &rhs) { this->token = rhs.token; }
        void clear() { token.clear(); }
    };

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
        this->sa.copy(rhs.sa);
        this->cust.copy(rhs.cust);
        this->user.copy(rhs.user);
        this->id_token.copy(rhs.id_token);
        this->access_token.copy(rhs.access_token);
        this->custom_token.copy(rhs.custom_token);
        this->legacy_token.copy(rhs.legacy_token);

        this->authenticated = rhs.authenticated;
        this->anonymous = rhs.anonymous;
        this->initialized = rhs.initialized;
    }

    void clear()
    {
        sa.clear();
        cust.clear();
        user.clear();
        id_token.clear();
        access_token.clear();
        custom_token.clear();
        legacy_token.clear();

        authenticated = false;
        anonymous = false;
        initialized = false;
    }

protected:
    service_account sa;
    custom_data cust;
    user_data user;
    id_token_data id_token;
    access_token_data access_token;
    custom_token_data custom_token;
    legacy_token_data legacy_token;
    bool authenticated = false;
    bool anonymous = false;
    bool initialized = false;
    auth_token_type auth_type = auth_unknown_token;
    user_auth_data_type auth_data_type = user_auth_data_undefined;
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
                auth_data.access_token.token = tokens[0];
                auth_data.access_token.refresh = tokens[1];
                auth_data.access_token.client_id = tokens[2];
                auth_data.access_token.client_secret = tokens[3];
                auth_data.access_token.expire = atoi(tokens[4].c_str());
                return true;
            }
            else if (type == token_type_id_token && tokenSize == 4)
            {
                auth_data.user.api_key = tokens[0];
                auth_data.id_token.token = tokens[1];
                auth_data.id_token.refresh = tokens[2];
                auth_data.id_token.expire = atoi(tokens[3].c_str());
            }
            else if (type == token_type_custom_token && tokenSize == 4)
            {
                auth_data.user.api_key = tokens[0];
                auth_data.custom_token.token = tokens[1];
                auth_data.custom_token.refresh = tokens[2];
                auth_data.custom_token.expire = atoi(tokens[3].c_str());
            }
            else if (type == token_type_legacy_token && tokenSize == 1)
            {
                auth_data.legacy_token.token = tokens[0];
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
                userfile.print(auth_data.access_token.token.c_str());
                userfile.print(",");
                userfile.print(auth_data.access_token.refresh.c_str());
                userfile.print(",");
                userfile.print(auth_data.access_token.client_id.c_str());
                userfile.print(",");
                userfile.print(auth_data.access_token.client_secret.c_str());
                userfile.print(",");
                userfile.print(String(auth_data.access_token.expire).c_str());
                return true;
            }
            else if (type == token_type_id_token)
            {
                userfile.print(auth_data.user.api_key.c_str());
                userfile.print(",");
                userfile.print(auth_data.id_token.token.c_str());
                userfile.print(",");
                userfile.print(auth_data.id_token.refresh.c_str());
                userfile.print(",");
                userfile.print(String(auth_data.id_token.expire).c_str());
                return true;
            }
            else if (type == token_type_custom_token)
            {

                userfile.print(auth_data.user.api_key.c_str());
                userfile.print(",");
                userfile.print(auth_data.custom_token.token.c_str());
                userfile.print(",");
                userfile.print(auth_data.custom_token.refresh.c_str());
                userfile.print(",");
                userfile.print(auth_data.custom_token.expire);
                return true;
            }
            else if (type == token_type_legacy_token)
            {
                userfile.print(auth_data.legacy_token.token.c_str());
                return true;
            }
        }
        return false;
    }
};

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

class UserAuth
{
    friend class Firebase;
    friend class FirebaseApp;
    friend class AsyncFirebaseClient;

public:
    UserAuth(const String &api_key, const String &email, const String &password)
    {
        data.clear();
        data.user.api_key = api_key;
        data.user.email = email;
        data.user.password = password;
        data.auth_type = auth_user_id_token;
        data.auth_data_type = user_auth_data_user_data;
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

class ServiceAuth
{
    friend class Firebase;
    friend class FirebaseApp;
    friend class AsyncFirebaseClient;

public:
    ServiceAuth(const String &clientEmail, const String &projectId, const String &privateKey)
    {
        data.clear();
        data.sa.client_email = clientEmail;
        data.sa.project_id = projectId;
        data.sa.private_key = privateKey;
        data.initialized = isInitialized();
        data.auth_type = auth_sa_access_token;
        data.auth_data_type = user_auth_data_service_account;
    };

    ServiceAuth(file_config_data &safile)
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
        }
    }

    ~ServiceAuth() { data.clear(); };

    user_auth_data &get() { return data; }

    bool isInitialized() { return data.sa.client_email.length() > 0 && data.sa.project_id.length() > 0 && data.sa.private_key.length() > 0; }

private:
    user_auth_data data;
};

class CustomAuth
{
    friend class Firebase;
    friend class FirebaseApp;
    friend class AsyncFirebaseClient;

public:
    CustomAuth(const String &apiKey, const String &clientEmail, const String &projectId, const String &privateKey, const String &uid, const String &scope = "", const String &claims = "")
    {
        data.clear();
        data.sa.client_email = clientEmail;
        data.sa.project_id = projectId;
        data.sa.private_key = privateKey;
        data.cust.api_key = apiKey;
        data.cust.uid = uid;
        data.cust.scope = scope;
        data.cust.claims = claims;
        data.initialized = isInitialized();
        data.auth_type = auth_sa_custom_token;
        data.auth_data_type = user_auth_data_custom_data;
    };

    template <typename T = const char *>
    CustomAuth(file_config_data &safile, T uid)
    {
        data.clear();
        if (safile.initialized)
        {
            safile.cb(safile.file, safile.filename.c_str(), file_mode_open_read);
            if (safile.file)
            {
                if (SAParser::parseSAFile(safile.file, data))
                {
                    data.cust.uid = toStringPtr(uid);
                    data.auth_type = auth_sa_custom_token;
                    data.auth_data_type = user_auth_data_custom_data;
                }
                safile.file.close();
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

class UserAccount
{
    friend class Firebase;
    friend class FirebaseApp;
    friend class AsyncFirebaseClient;

public:
    template <typename T = const char *>
    UserAccount(T apiKey)
    {
        data.user.api_key = toStringPtr(apiKey);
        data.auth_type = auth_user_id_token;
        data.auth_data_type = user_auth_data_user_data;
        data.initialized = true;
    }
    template <typename T = const char *>
    UserAccount &email(T email)
    {
        data.user.email = toStringPtr(email);
        data.auth_type = auth_user_id_token;
        data.auth_data_type = user_auth_data_user_data;
        data.initialized = true;
        return *this;
    };

    template <typename T = const char *>
    UserAccount &password(T password)
    {
        data.user.password = toStringPtr(password);
        data.auth_type = auth_user_id_token;
        data.auth_data_type = user_auth_data_user_data;
        data.initialized = true;
        return *this;
    };

    template <typename T = const char *>
    UserAccount &idToken(T idToken)
    {
        data.user.id_token = toStringPtr(idToken);
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

class IDToken
{
    friend class Firebase;
    friend class FirebaseApp;
    friend class AsyncFirebaseClient;

public:
    template <typename T1 = const char *, typename T2 = const char *, typename T3 = const char *>
    IDToken(T1 api_key, T2 token, size_t expire = 3600, T3 refresh = "")
    {
        this->data.clear();
        this->data.user.api_key = toStringPtr(api_key);
        this->data.id_token.token = toStringPtr(token);
        this->data.id_token.refresh = toStringPtr(refresh);
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

class AccessToken
{
    friend class Firebase;
    friend class FirebaseApp;
    friend class AsyncFirebaseClient;

public:
    template <typename T1 = const char *, typename T2 = const char *, typename T3 = const char *, typename T4 = const char *>
    AccessToken(T1 token, size_t expire = 3600, T2 refresh = "", T3 client_id = "", T4 client_secret = "")
    {
        this->data.clear();
        this->data.access_token.token = toStringPtr(token);
        this->data.access_token.refresh = toStringPtr(refresh);
        this->data.access_token.client_id = toStringPtr(client_id);
        this->data.access_token.client_secret = toStringPtr(client_secret);
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

class CustomToken
{
    friend class Firebase;
    friend class FirebaseApp;
    friend class AsyncFirebaseClient;

public:
    CustomToken(const String &api_key, const String &token, size_t expire = 3600, const String &refresh = "")
    {
        this->data.clear();
        this->data.custom_token.token = token;
        this->data.user.api_key = api_key;
        this->data.custom_token.refresh = refresh;
        this->data.custom_token.expire = expire;
        this->data.initialized = data.custom_token.token.length() > 0 && data.user.api_key.length() > 0;
        this->data.auth_type = auth_custom_token;
        this->data.auth_data_type = user_auth_data_custom_token;
    }

    CustomToken(file_config_data &tokenFile)
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
        this->data.auth_data_type = user_auth_data_no_token;
    }
    user_auth_data &get() { return data; }

private:
    user_auth_data data;
};

struct auth_token_data_t
{
    user_auth_data user_auth;
    String token;
    uint32_t expire = 0;
};

#endif