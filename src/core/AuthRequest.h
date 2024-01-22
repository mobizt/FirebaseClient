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

    void asyncRequest(AsyncClient *aClient, AsyncResult &aResult, JWT &jwt)
    {
        String host;
        async_request_handler_t req;
        req.addGAPIsHost(host, "oauth2");

        AsyncClient::async_data_item_t *sData = aClient->newSlot(firebase_client_list, host, "/token", "", async_request_handler_t::http_post, AsyncClient::slot_options_t(false, false, true, false, false));

        req.addContentTypeHeader(sData->request.header, "application/json");

        JSON json;
        json.addObject(sData->request.payload, json.toString("grant_type"), json.toString("urn:ietf:params:oauth:grant-type:jwt-bearer"));
        json.addObject(sData->request.payload, json.toString("assertion"), json.toString(jwt.token()), true);

        aClient->setContentLength(sData, sData->request.payload.length());

        sData->refResult = &aResult;

        aClient->process(firebase_client_list, sData->async);
    }
};

#endif
