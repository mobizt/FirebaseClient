#ifndef CORE_AUTH_REQUEST_H
#define CORE_AUTH_REQUEST_H
#include <Arduino.h>
#include "AsyncClient/AsyncClient.h"
#include "core/JSON.h"
#include "core/JWT.h"

class AuthRequest
{
private:
public:
    AuthRequest(){};
    ~AuthRequest(){};

    void asyncRequest(AsyncClient *aClient, const String &subdomain, const String &extras, const String &payload, AsyncResult &aResult)
    {
        String host;
        async_request_handler_t req;
        req.addGAPIsHost(host, subdomain.c_str());

        AsyncClient::async_data_item_t *sData = aClient->newSlot(firebase_client_list, host, extras, "", async_request_handler_t::http_post, AsyncClient::slot_options_t(false, false, true, false, false));
        req.addContentTypeHeader(sData->request.header, "application/json");
        sData->request.payload = payload;
        aClient->setContentLength(sData, sData->request.payload.length());
        sData->refResult = &aResult;
        aClient->process(firebase_client_list, sData->async);
    }

    void setLastError(AsyncResult &aResult, int code, const String &message)
    {
        aResult.lastError.err.message = message;
        if (code != 0)
            aResult.lastError.err.code = code;
    }

    void process(AsyncClient *aClient, AsyncResult &aResult, AsyncResultCallback resultCb)
    {
        aClient->process(firebase_client_list, true);

        if (aResult.lastError.code() != 0 && aResult.error_available)
        {
            if (resultCb)
                resultCb(aResult);
        }
    }

    void stop(AsyncClient *aClient)
    {
        aClient->stop();
    }

    void setEventResult(AsyncResult &aResult, const String &msg, int code)
    {
        aResult.app_event.ev_code = code;
        aResult.app_event.ev_msg = msg;
    }
};

#endif
