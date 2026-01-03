/*
 * SPDX-FileCopyrightText: 2026 Suwatchai K. <suwatchai@outlook.com>
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef CORE_ASYNC_CLIENT_ASYNC_DATA_H
#define CORE_ASYNC_CLIENT_ASYNC_DATA_H

#include "./core/AsyncClient/ConnectionHandler.h"
#include "./core/AsyncClient/RequestHandler.h"
#include "./core/AsyncClient/ResponseHandler.h"
#include "./core/AsyncResult/AsyncResult.h"
#include "./core/AsyncClient/AsyncState.h"

struct async_data
{
    friend class FirebaseApp;

public:
    struct async_error_t
    {
        async_state state = astate_undefined;
        int code = 0;
    };

    async_state state = astate_undefined;
    function_return_type return_type = ret_undefined;
    req_handler request;
    res_handler response;
    async_error_t error;
    bool to_remove = false, auth_used = false, complete = false, async = false, stop_current_async = false, sse = false, path_not_existed = false;
    bool download = false, upload_progress_enabled = false, upload = false;
    uint32_t auth_ts = 0, addr = 0, ref_result_addr = 0;
    AsyncResult aResult;
    AsyncResult *refResult = nullptr;
    AsyncResultCallback cb = NULL;
    Timer err_timer;

    async_data()
    {
        addr = reinterpret_cast<uint32_t>(this);
        err_timer.feed(0);
    }

    void setRefResult(AsyncResult *refResult, uint32_t rvec_addr)
    {
        this->refResult = refResult;
        ref_result_addr = reinterpret_cast<uint32_t>(refResult);
        this->refResult->rvec_addr = rvec_addr;
        if (rvec_addr > 0)
        {
            std::vector<uint32_t> *rVec = reinterpret_cast<std::vector<uint32_t> *>(rvec_addr);
            List vec;
            vec.addRemoveList(*rVec, ref_result_addr, true);
        }
    }

    void reset()
    {
        state = astate_undefined;
        return_type = ret_undefined;
        request.clear();
        response.clear();
        error.code = 0;
        error.state = astate_undefined;
        to_remove = false;
        auth_used = false;
        async = false;
        stop_current_async = false;
        sse = false;
        path_not_existed = false;
        cb = NULL;
        err_timer.reset();
    }
};
#endif