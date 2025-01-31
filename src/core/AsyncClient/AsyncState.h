#ifndef CORE_ASYNC_CLIENT_ASYNC_STATE_H
#define CORE_ASYNC_CLIENT_ASYNC_STATE_H

enum async_state
{
    astate_undefined,
    astate_send_header,
    astate_send_payload,
    astate_read_response,
    astate_complete
};
#endif