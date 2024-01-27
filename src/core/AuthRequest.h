#ifndef CORE_AUTH_REQUEST_H
#define CORE_AUTH_REQUEST_H
#include <Arduino.h>
#include "AsyncClient/AsyncClient.h"
#include "core/JSON.h"
#include "core/JWT.h"

class AuthRequest
{
private:
    AsyncClient::async_data_item_t *sData = nullptr;

public:
    AuthRequest(){};
    ~AuthRequest(){};

    void asyncRequest(AsyncClient *aClient, const String &subdomain, const String &extras, const String &payload, AsyncResult &aResult)
    {
        String host;
        async_request_handler_t req;
        req.addGAPIsHost(host, subdomain.c_str());
        sData = aClient->newSlot(firebase_client_list, host, extras, "", async_request_handler_t::http_post, AsyncClient::slot_options_t(true, false, true, false, false));
        req.addContentTypeHeader(sData->request.header, "application/json");
        sData->request.payload = payload;
        aClient->setContentLength(sData, sData->request.payload.length());
        sData->refResult = &aResult;
        sData->ref_result_addr = aResult.addr;
        aClient->process(sData->async);
        aClient->handleRemove();
    }

    void setLastError(AsyncResult &aResult, int code, const String &message)
    {
        aResult.lastError.err.message = message;
        aResult.lastError.err.code = code;
    }

    void clearLastError(AsyncResult &aResult)
    {
        aResult.lastError.err.message = "";
        aResult.lastError.err.code = 0;
    }

    void process(AsyncClient *aClient, AsyncResult &aResult, AsyncResultCallback resultCb)
    {
        aClient->process(true);
        aClient->handleRemove();

        if (resultCb && aResult.lastError.code() != 0 && aResult.error_available)
        {
            aResult.data_available = false;
            resultCb(aResult);
        }
    }

    void remove(AsyncClient *aClient)
    {
        aClient->handleRemove();
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
