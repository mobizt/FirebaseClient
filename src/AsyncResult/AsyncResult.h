
#ifndef ASYNC_RESULT_H
#define ASYNC_RESULT_H
#include "Value.h"
#include "./core/CRC16.h"
#include "./core/Error.h"

#define FIREBASE_CHUNK_SIZE 2048
#define FIREBASE_BASE64_CHUNK_SIZE 1026
#define FIREBASE_SSE_TIMEOUT 40 * 1000;

class AsyncResult
{
    friend class AsyncClient;
    friend class Database;

    struct download_data_t
    {
    public:
        String sourcePath;
        size_t total = 0;
        size_t downloaded = 0;
        bool progress_available = false;
        int progress = -1;
        void reset()
        {
            sourcePath = "";
            total = 0;
            downloaded = 0;
            progress_available = false;
            progress = -1;
        }
    };

    struct upload_data_t
    {
    public:
        String targetPath;
        size_t total = 0;
        size_t uploaded = 0;
        bool progress_available = false;
        int progress = -1;
        void reset()
        {
            targetPath = "";
            total = 0;
            uploaded = 0;
            progress_available = false;
            progress = -1;
        }
    };

private:
    FirebaseError lastError;
    String data_path;
    String data_payload;
    bool data_available = false;
    bool error_available = false;
    download_data_t download_data;
    upload_data_t upload_data;

    void setPayload(const String &data)
    {
        data_available = true;
        data_payload = data;
        database.ref_payload = &data_payload;
    }

public:
    struct database_data_t
    {
        friend class Database;
        friend class AsyncResult;
        friend class AsyncClient;

    public:
        template <typename T>
        T to()
        {
            return value.to<T>(data().c_str());
        }
        bool isStream() { return sse; }
        String name() { return node_name; }
        String ETag() { return etag; }
        String dataPath() { return ref_payload ? ref_payload->substring(data_path_p1, data_path_p2) : ""; }
        String event() { return ref_payload ? ref_payload->substring(event_p1, event_p2) : ""; }
        String data()
        {
            if (data_p1 > 0)
                return ref_payload->substring(data_p1, data_p2);
            return ref_payload ? *ref_payload : "";
        }

        bool eventTimeout()
        {
            bool ret = sse && millis() - event_time > FIREBASE_SSE_TIMEOUT;
            if (ret)
                sse_request = true;
            return ret;
        }

        database_data_type type()
        {
            return value.getType(data().c_str());
        }

    private:
        Value value;
        String *ref_payload = nullptr;
        bool null_etag = false;
        bool sse = false;
        bool sse_request = false;
        String node_name; // database node name that value was pushed
        String etag;
        uint16_t data_path_p1 = 0, data_path_p2 = 0;
        uint16_t event_p1 = 0, event_p2 = 0;
        uint16_t data_p1 = 0, data_p2 = 0;
        unsigned long event_time = 0;

        void clearSSE()
        {
            data_path_p1 = 0;
            data_path_p2 = 0;
            event_p1 = 0;
            event_p2 = 0;
            data_p1 = 0;
            data_p2 = 0;
            sse = false;
        }

        void parseNodeName()
        {
            StringHelper sh;
            int p1 = 0, p2 = 0;
            sh.parse(*ref_payload, "\"name\"", "}", p1, p2);
            if (p1 > -1 && p2 > -1)
            {
                node_name = ref_payload->substring(p1 + 1, p2 - 1);
            }
        }

        void parseSSE()
        {
            clearSSE();

            StringHelper sh;
            int p1 = 0, p2 = 0;
            sh.parse(*ref_payload, "event", "\n", p1, p2);
            if (p1 > -1 && p2 > -1)
            {
                event_p1 = p1;
                event_p2 = p2;
                p1 = p2;
                event_time = millis();
                sse = true;
                sse_request = false;
            }

            sh.parse(*ref_payload, "data", "\n", p1, p2);
            if (p1 > -1 && p2 > -1)
            {
                if (ref_payload->substring(p1, p2) == "null")
                {
                    data_p1 = p1;
                    data_p2 = p2;
                    return;
                }

                sh.parse(*ref_payload, "\"path\"", ",", p1, p2);
                if (p1 > -1 && p2 > -1)
                {
                    data_path_p1 = p1 + 1;
                    data_path_p2 = p2 - 1;
                    p1 = p2;
                }

                sh.parse(*ref_payload, "\"data\"", "\n", p1, p2);
                if (p1 > -1 && p2 > -1)
                {
                    if ((*ref_payload)[p2 - 1] == '}')
                        p2--;
                    data_p1 = p1;
                    data_p2 = p2;
                }
            }
        }
    };

    AsyncResult() { database.ref_payload = &data_payload; };
    ~AsyncResult(){};
    String payload() { return data_payload; }
    String path() { return data_path; }
    bool available()
    {
        bool ret = data_available;
        data_available = false;
        return ret;
    }

    bool uploadProgress()
    {
        if (upload_data.uploaded > 0)
        {
            int progress = (float)upload_data.uploaded / upload_data.total * 100;
            if (upload_data.progress != progress && (progress == 0 || progress == 100 || upload_data.progress + 2 <= progress))
            {
                upload_data.progress = progress;
                return true;
            }
        }
        return false;
    }

    upload_data_t uploadInfo() { return upload_data; }

    bool downloadProgress()
    {
        if (download_data.downloaded > 0)
        {
            int progress = (float)download_data.downloaded / download_data.total * 100;
            if (download_data.progress != progress && (progress == 0 || progress == 100 || download_data.progress + 2 <= progress))
            {
                download_data.progress = progress;
                return true;
            }
        }
        return false;
    }

    download_data_t downloadInfo() { return download_data; }

    bool isError()
    {
        bool err = lastError.code() != 0 && lastError.code() != FIREBASE_ERROR_HTTP_CODE_OK;
        if (error_available)
        {
            error_available = false;
            return err;
        }
        return false;
    }

    FirebaseError error()
    {
        return lastError;
    }

    database_data_t database;
};

typedef void (*AsyncResultCallback)(AsyncResult &aResult);

#endif
