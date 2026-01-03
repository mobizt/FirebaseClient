/*
 * SPDX-FileCopyrightText: 2026 Suwatchai K. <suwatchai@outlook.com>
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef CORE_ASYNC_RESULT_RTDB_RESULT_H
#define CORE_ASYNC_RESULT_RTDB_RESULT_H

#include <Arduino.h>
#include "./core/AsyncResult/AppLog.h"
#include "./core/AsyncResult/Value.h"
#include "./core/Utils/Timer.h"

namespace firebase_ns
{
#if defined(ENABLE_DATABASE)
    enum event_resume_status_t
    {
        event_resume_status_undefined,
        event_resume_status_resuming,
        event_resume_status_finished
    };

    class RealtimeDatabaseResult
    {
        friend class AsyncResult;
        friend class AsyncClientClass;
        friend class RTDBResultImpl;

    private:
        ValueConverter vcon;
        StringUtil sut;
        Timer sse_timer;
        bool sse = false;
        event_resume_status_t event_resume_status = event_resume_status_undefined;
        String node_name, etag;
        uint32_t data_path_p1 = 0, data_path_p2 = 0, event_p1 = 0, event_p2 = 0, data_p1 = 0, data_p2 = 0;
        bool null_etag = false;
        String *ref_payload = nullptr;

        void clearSSE()
        {
            data_path_p1 = 0;
            data_path_p2 = 0;
            event_p1 = 0;
            event_p2 = 0;
            data_p1 = 0;
            data_p2 = 0;
            sse = false;
            event_resume_status = event_resume_status_undefined;
        }
        void parseNodeName()
        {
            int p1 = 0, p2 = 0;
            sut.parse(*ref_payload, "\"name\"", "}", p1, p2);
            if (p1 > -1 && p2 > -1)
            {
                node_name = ref_payload->substring(p1 + 1, p2 - 1);
            }
        }
        void feed() { sse_timer.feed(FIREBASE_SSE_TIMEOUT_MS / 1000); }
        void parseSSE()
        {
            clearSSE();
            int p1 = 0, p2 = 0;
            sut.parse(*ref_payload, "event", "\n", p1, p2);
            if (p1 > -1 && p2 > -1)
            {
                event_p1 = p1;
                event_p2 = p2;
                p1 = p2;
                setEventResumeStatus(event_resume_status_undefined);
                sse_timer.feed(String(event()).indexOf("cancel") > -1 || String(event()).indexOf("auth_revoked") > -1 ? 0 : FIREBASE_SSE_TIMEOUT_MS / 1000);
                sse = true;
            }

            sut.parse(*ref_payload, "data", "\n", p1, p2);
            if (p1 > -1 && p2 > -1)
            {
                int p3 = p1, p4 = p2;
                if (ref_payload->substring(p1, p2) == "null")
                {
                    data_p1 = p1;
                    data_p2 = p2;
                    return;
                }

                sut.parse(*ref_payload, "\"path\"", ",", p1, p2);
                if (p1 > -1 && p2 > -1)
                {
                    data_path_p1 = p1 + 1;
                    data_path_p2 = p2 - 1;
                    p1 = p2;
                }

                sut.parse(*ref_payload, "\"data\"", "\n", p1, p2);
                if (p1 > -1 && p2 > -1)
                {
                    if ((*ref_payload)[p2 - 1] == '}')
                        p2--;
                    data_p1 = p1;
                    data_p2 = p2;
                }

                if (data_p1 == 0)
                {
                    data_p1 = p3;
                    data_p2 = p4;
                }
            }
        }
        void setEventResumeStatus(event_resume_status_t status) { event_resume_status = status; }
        event_resume_status_t eventResumeStatus() const { return event_resume_status; }

    protected:
        void setRefPayload(RealtimeDatabaseResult *rtdbResult, String *payload) { rtdbResult->ref_payload = payload; }
        void clearSSE(RealtimeDatabaseResult *rtdbResult) { rtdbResult->clearSSE(); }
        void parseNodeName(RealtimeDatabaseResult *rtdbResult) { rtdbResult->parseNodeName(); }
        void parseSSE(RealtimeDatabaseResult *rtdbResult) { rtdbResult->parseSSE(); }
        void feedSSETimer(RealtimeDatabaseResult *rtdbResult) { rtdbResult->feed(); }
        void setEventResumeStatus(RealtimeDatabaseResult *rtdbResult, event_resume_status_t status) { rtdbResult->setEventResumeStatus(status); }
        event_resume_status_t eventResumeStatus(const RealtimeDatabaseResult *rtdbResult) { return rtdbResult->eventResumeStatus(); }

    public:
        RealtimeDatabaseResult() {}

        /**
         * Convert the RealtimeDatabaseResult to any type of values.
         *
         * @return T The T type value e.g. boolean, integer, float, double and string.
         */
        template <typename T>
        T to() { return vcon.to<T>(data().c_str()); }

        /**
         * Check if the async task is SSE mode (HTTP Streaming) task.
         *
         * @return bool Returns true if the task is SSE mode (HTTP Streaming) task.
         */
        bool isStream() const { return sse; }

        /**
         * Get the name (random UID) of the node that will be creaated from RealtimeDatabase::Push.
         *
         * @return String The push UID string.
         */
        String name() const { return node_name.c_str(); }

        /**
         * Get the ETag from Realtime database get function.
         *
         * @return String The ETag string.
         */
        String ETag() const { return etag.c_str(); }

        /**
         * Get the SSE mode (HTTP Streaming) event data path which is the relative path of the data that has been changed in the database.
         *
         * @return String The relative path of data that has been changed.
         */
        String dataPath() const { return ref_payload ? ref_payload->substring(data_path_p1, data_path_p2).c_str() : String(); }

        /**
         * Get the `SSE mode (HTTP Streaming)` event type string.
         *
         * @return String The event type string e.g. `put`, `patch`, `keep-alive`, `cancel` and `auth_revoked`.
         */
        String event() { return ref_payload ? ref_payload->substring(event_p1, event_p2).c_str() : String(); }

        /**
         * Get the SSE mode (HTTP Streaming) event data that has been changed.
         *
         * @return String The data that has been changed.
         */
        String data()
        {
            if (data_p1 > 0)
                return ref_payload->substring(data_p1, data_p2);
            return ref_payload ? ref_payload->c_str() : String();
        }

        /**
         * Get the SSE mode (HTTP Streaming) event time out status.
         *
         * @return bool The SSE mode (HTTP Streaming) event time out status.
         */
        bool eventTimeout() { return sse && sse_timer.remaining() == 0; }

        /**
         * Get the type of Realtime database data.
         *
         * @return realtime_database_data_type The realtime_database_data_type enum represents the type of Realtime database data.
         *
         * The realtime_database_data_type enums are included the following.
         *
         * realtime_database_data_type_undefined or -1
         * realtime_database_data_type_null or 0.
         * realtime_database_data_type_integer or 1.
         * realtime_database_data_type_float or 2.
         * realtime_database_data_type_double or 3.
         * realtime_database_data_type_boolean or 4.
         * realtime_database_data_type_string or 5.
         * realtime_database_data_type_json or 6.
         * realtime_database_data_type_array or 7.
         */
        realtime_database_data_type type() { return vcon.getType(data().c_str()); }
    };
#endif
}
#endif