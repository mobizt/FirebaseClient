/**
 * Created March 13, 2024
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
#ifndef MESSAGING_DATA_OPTIONS_H
#define MESSAGING_DATA_OPTIONS_H

#include <Arduino.h>
#include "./Config.h"
#include "./core/JSON.h"
#include "./core/ObjectWriter.h"

namespace Messages
{
    enum firebase_cloud_messaging_request_type
    {
        firebase_cloud_messaging_request_type_undefined,
        firebase_cloud_messaging_request_type_send
    };

    // https://firebase.google.com/docs/reference/fcm/rest/v1/projects.messages

    enum AndroidMessagePriority
    {
        NORMAL, // Default priority for data messages.
        _HIGH   // Default priority for notification messages.
    };

    // Priority levels of a notification.
    enum NotificationPriority
    {
        PRIORITY_UNSPECIFIED, //	If priority is unspecified, notification priority is set to PRIORITY_DEFAULT.
        PRIORITY_MIN,         //	Lowest notification priority. Notifications with this PRIORITY_MIN might not be shown to the user except under special circumstances, such as detailed notification logs.
        PRIORITY_LOW,         //	Lower notification priority. The UI may choose to show the notifications smaller, or at a different position in the list, compared with notifications with PRIORITY_DEFAULT.
        PRIORITY_DEFAULT,     //	Default notification priority. If the application does not prioritize its own notifications, use this value for all notifications.
        PRIORITY_HIGH,        //	Higher notification priority. Use this for more important notifications or alerts. The UI may choose to show these notifications larger, or at a different position in the notification lists, compared with notifications with PRIORITY_DEFAULT.
        PRIORITY_MAX,         //	Highest notification priority. Use this for the application's most important items that require the user's prompt attention or input.
    };

    // Different visibility levels of a notification.
    enum visibility
    {
        VISIBILITY_UNSPECIFIED, //	If unspecified, default to Visibility.PRIVATE.
        PRIVATE,                //	Show this notification on all lockscreens, but conceal sensitive or private information on secure lockscreens.
        PUBLIC,                 //	Show this notification in its entirety on all lockscreens.
        SECRET                  //	Do not reveal any part of this notification on a secure lockscreen.
    };

    // Basic notification template to use across all platforms.
    struct Notification : public Printable
    {
    private:
        size_t bufSize = 4;
        String buf[4];
        ObjectWriter owriter;
        Notification &setObject(String &buf_n, const String &key, const String &value, bool isString, bool last)
        {
            owriter.setObject(buf, bufSize, buf_n, key, value, isString, last);
            return *this;
        }

    public:
        // The notification's title.
        Notification &title(const String &value) { return setObject(buf[1], "title", value, true, true); }

        // The notification's body text.
        Notification &body(const String &value) { return setObject(buf[2], "body", value, true, true); }

        // Contains the URL of an image that is going to be downloaded on the device and displayed in a notification. JPEG, PNG, BMP have full support across platforms.
        Notification &image(const String &value) { return setObject(buf[3], "image", value, true, true); }

        const char *c_str() const { return buf[0].c_str(); }
        size_t printTo(Print &p) const { return p.print(buf[0].c_str()); }
        void clear() { owriter.clearBuf(buf, bufSize); }
    };

    struct Color : public Printable
    {
    private:
        size_t bufSize = 5;
        String buf[5];
        ObjectWriter owriter;
        Color &setObject(String &buf_n, const String &key, const String &value, bool isString, bool last)
        {
            owriter.setObject(buf, bufSize, buf_n, key, value, isString, last);
            return *this;
        }

    public:
        Color &red(float value)
        {
            if (value > 1)
                value = 1;
            else if (value < 0)
                value = 0;
            return setObject(buf[1], "red", String(value), false, true);
        }
        Color &green(float value)
        {
            if (value > 1)
                value = 1;
            else if (value < 0)
                value = 0;
            return setObject(buf[2], "green", String(value), false, true);
        }
        Color &blue(float value)
        {
            if (value > 1)
                value = 1;
            else if (value < 0)
                value = 0;
            return setObject(buf[3], "blue", String(value), false, true);
        }
        Color &alpha(float value)
        {
            if (value > 1)
                value = 1;
            else if (value < 0)
                value = 0;
            return setObject(buf[4], "alpha", String(value), false, true);
        }
        const char *c_str() const { return buf[0].c_str(); }
        size_t printTo(Print &p) const { return p.print(buf[0].c_str()); }
        void clear() { owriter.clearBuf(buf, bufSize); }
    };

    struct LightSettings : public Printable
    {
    private:
        size_t bufSize = 4;
        String buf[4];
        ObjectWriter owriter;
        LightSettings &setObject(String &buf_n, const String &key, const String &value, bool isString, bool last)
        {
            owriter.setObject(buf, bufSize, buf_n, key, value, isString, last);
            return *this;
        }

    public:
        LightSettings &color(Color color) { return setObject(buf[1], "color", color.c_str(), false, true); }
        LightSettings &light_on_duration(const String &value) { return setObject(buf[2], "light_on_duration", value, true, true); }
        LightSettings &light_off_duration(const String &value) { return setObject(buf[3], "light_off_duration", value, true, true); }
        const char *c_str() const { return buf[0].c_str(); }
        size_t printTo(Print &p) const { return p.print(buf[0].c_str()); }
        void clear() { owriter.clearBuf(buf, bufSize); }
    };

    struct AndroidNotification : public Printable
    {
    private:
        size_t bufSize = 25;
        String buf[25];
        ObjectWriter owriter;
        JsonHelper jh;
        AndroidNotification &setObject(String &buf_n, const String &key, const String &value, bool isString, bool last)
        {
            owriter.setObject(buf, bufSize, buf_n, key, value, isString, last);
            return *this;
        }

    public:
        AndroidNotification &title(const String &value) { return setObject(buf[1], "title", value, true, true); }
        AndroidNotification &body(const String &value) { return setObject(buf[2], "body", value, true, true); }
        AndroidNotification &icon(const String &value) { return setObject(buf[3], "icon", value, true, true); }
        AndroidNotification &color(const String &value) { return setObject(buf[4], "color", value, true, true); }
        AndroidNotification &sound(const String &value) { return setObject(buf[5], "sound", value, true, true); }
        AndroidNotification &tag(const String &value) { return setObject(buf[6], "tag", value, true, true); }
        AndroidNotification &click_action(const String &value) { return setObject(buf[7], "click_action", value, true, true); }
        AndroidNotification &body_loc_key(const String &value) { return setObject(buf[8], "body_loc_key", value, true, true); }
        AndroidNotification &body_loc_args(const String &value)
        {
            owriter.addMapArrayMember(buf, bufSize, buf[9], "body_loc_args", value, true);
            return *this;
        }
        AndroidNotification &title_loc_key(const String &value) { return setObject(buf[10], "title_loc_key", value, true, true); }
        AndroidNotification &title_loc_args(const String &value)
        {
            owriter.addMapArrayMember(buf, bufSize, buf[11], "title_loc_args", value, true);
            return *this;
        }
        AndroidNotification &channel_id(const String &value) { return setObject(buf[12], "channel_id", value, true, true); }
        AndroidNotification &ticker(const String &value) { return setObject(buf[13], "ticker", value, true, true); }
        AndroidNotification &sticky(bool value) { return setObject(buf[14], "sticky", owriter.getBoolStr(value), false, true); }
        AndroidNotification &event_time(const String &value) { return setObject(buf[15], "event_time", value, true, true); }
        AndroidNotification &local_only(bool value) { return setObject(buf[16], "local_only", owriter.getBoolStr(value), false, true); }
        AndroidNotification &notification_priority(NotificationPriority value)
        {
            if (value == PRIORITY_UNSPECIFIED)
                return setObject(buf[17], "notification_priority", "PRIORITY_UNSPECIFIED", true, true);
            else if (value == PRIORITY_MIN)
                return setObject(buf[17], "notification_priority", "PRIORITY_MIN", true, true);
            else if (value == PRIORITY_LOW)
                return setObject(buf[17], "notification_priority", "PRIORITY_LOW", true, true);
            else if (value == PRIORITY_DEFAULT)
                return setObject(buf[17], "notification_priority", "PRIORITY_DEFAULT", true, true);
            else if (value == PRIORITY_HIGH)
                return setObject(buf[17], "notification_priority", "PRIORITY_HIGH", true, true);
            else if (value == PRIORITY_MAX)
                return setObject(buf[17], "notification_priority", "PRIORITY_MAX", true, true);
            return *this;
        }

        AndroidNotification &default_sound(bool value) { return setObject(buf[18], "default_sound", owriter.getBoolStr(value), false, true); }
        AndroidNotification &default_vibrate_timings(bool value) { return setObject(buf[19], "default_vibrate_timings", owriter.getBoolStr(value), false, true); }
        AndroidNotification &default_light_settings(bool value) { return setObject(buf[20], "default_light_settings", owriter.getBoolStr(value), false, true); }
        AndroidNotification &vibrate_timings(const String &value)
        {
            owriter.addMapArrayMember(buf, bufSize, buf[21], "vibrate_timings", value, true);
            return *this;
        }
        AndroidNotification &visibility(visibility value)
        {
            if (value == VISIBILITY_UNSPECIFIED)
                return setObject(buf[22], "visibility", "VISIBILITY_UNSPECIFIED", true, true);
            else if (value == visibility::PRIVATE)
                return setObject(buf[22], "visibility", "PRIVATE", true, true);
            else if (value == visibility::PUBLIC)
                return setObject(buf[22], "visibility", "PUBLIC", true, true);
            else if (value == visibility::SECRET)
                return setObject(buf[22], "visibility", "SECRET", true, true);
            return *this;
        }
        AndroidNotification &notification_count(int value) { return setObject(buf[23], "notification_count", String(value), false, true); }
        AndroidNotification &light_settings(const LightSettings &value) { return setObject(buf[24], "light_settings", value.c_str(), false, true); }
        AndroidNotification &image(const String &value) { return setObject(buf[25], "image", value, true, true); }

        const char *c_str() const { return buf[0].c_str(); }
        size_t printTo(Print &p) const { return p.print(buf[0].c_str()); }
        void clear() { owriter.clearBuf(buf, bufSize); }
    };

    // Options for features provided by the FCM SDK for Android.
    struct AndroidFcmOptions : public Printable
    {
    private:
        String buf;
        JsonHelper jh;

    public:
        // Label associated with the message's analytics data.
        AndroidFcmOptions &analytics_label(const String &value)
        {
            clear();
            jh.addObject(buf, "analytics_label", value, true, true);
            return *this;
        }
        const char *c_str() const { return buf.c_str(); }
        size_t printTo(Print &p) const { return p.print(buf.c_str()); }
        void clear() { buf.remove(0, buf.length()); }
    };

    struct WebpushFcmOptions
    {
    private:
        size_t bufSize = 3;
        String buf[3];
        ObjectWriter owriter;
        JsonHelper jh;
        WebpushFcmOptions &setObject(String &buf_n, const String &key, const String &value, bool isString, bool last)
        {
            owriter.setObject(buf, bufSize, buf_n, key, value, isString, last);
            return *this;
        }

    public:
        // The link to open when the user clicks on the notification. For all URL values, HTTPS is required.
        WebpushFcmOptions &link(const String &value) { return setObject(buf[1], "link", value, false, true); }
        // Label associated with the message's analytics data.
        WebpushFcmOptions &analytics_label(const String &value) { return setObject(buf[2], "analytics_label", value, false, true); }
        const char *c_str() const { return buf[0].c_str(); }
        size_t printTo(Print &p) const { return p.print(buf[0].c_str()); }
        void clear() { owriter.clearBuf(buf, bufSize); }
    };

    // Platform independent options for features provided by the FCM SDKs.

    struct FcmOptions : public Printable
    {
    private:
        String buf;
        JsonHelper jh;

    public:
        // Label associated with the message's analytics data.
        FcmOptions &analytics_label(const String &value)
        {
            clear();
            jh.addObject(buf, "analytics_label", value, true, true);
            return *this;
        }
        const char *c_str() const { return buf.c_str(); }
        size_t printTo(Print &p) const { return p.print(buf.c_str()); }
        void clear() { buf.remove(0, buf.length()); }
    };

    struct WebpushConfig : public Printable
    {
    private:
        size_t bufSize = 5;
        String buf[5];
        ObjectWriter owriter;
        JsonHelper jh;
        WebpushConfig &setObject(String &buf_n, const String &key, const String &value, bool isString, bool last)
        {
            owriter.setObject(buf, bufSize, buf_n, key, value, isString, last);
            return *this;
        }

    public:
        WebpushConfig &headers(const object_t &value) { return setObject(buf[1], "headers", value.c_str(), false, true); }
        WebpushConfig &data(const object_t &value) { return setObject(buf[2], "data", value.c_str(), false, true); }
        WebpushConfig &notification(const object_t &value) { return setObject(buf[3], "notification", value.c_str(), false, true); }
        WebpushConfig &fcm_options(const WebpushFcmOptions &value) { return setObject(buf[4], "fcm_options", value.c_str(), false, true); }

        const char *c_str() const { return buf[0].c_str(); }
        size_t printTo(Print &p) const { return p.print(buf[0].c_str()); }
        void clear() { owriter.clearBuf(buf, bufSize); }
    };

    // Options for features provided by the FCM SDK for iOS.
    struct ApnsFcmOptions : public Printable
    {
    private:
        size_t bufSize = 3;
        String buf[3];
        ObjectWriter owriter;
        JsonHelper jh;
        ApnsFcmOptions &setObject(String &buf_n, const String &key, const String &value, bool isString, bool last)
        {
            owriter.setObject(buf, bufSize, buf_n, key, value, isString, last);
            return *this;
        }

    public:
        ApnsFcmOptions &analytics_label(const object_t &value) { return setObject(buf[1], "analytics_label", value.c_str(), false, true); }
        ApnsFcmOptions &image(const object_t &value) { return setObject(buf[2], "image", value.c_str(), false, true); }

        const char *c_str() const { return buf[0].c_str(); }
        size_t printTo(Print &p) const { return p.print(buf[0].c_str()); }
        void clear() { owriter.clearBuf(buf, bufSize); }
    };

    struct ApnsConfig : public Printable
    {
    private:
        size_t bufSize = 4;
        String buf[4];
        ObjectWriter owriter;
        JsonHelper jh;
        ApnsConfig &setObject(String &buf_n, const String &key, const String &value, bool isString, bool last)
        {
            owriter.setObject(buf, bufSize, buf_n, key, value, isString, last);
            return *this;
        }

    public:
        ApnsConfig &headers(const object_t &value) { return setObject(buf[1], "headers", value.c_str(), false, true); }
        ApnsConfig &payload(const object_t &value) { return setObject(buf[2], "payload", value.c_str(), false, true); }
        ApnsConfig &fcm_options(const ApnsFcmOptions &value) { return setObject(buf[3], "fcm_options", value.c_str(), false, true); }

        const char *c_str() const { return buf[0].c_str(); }
        size_t printTo(Print &p) const { return p.print(buf[0].c_str()); }
        void clear() { owriter.clearBuf(buf, bufSize); }
    };

    struct AndroidConfig : public Printable
    {
    private:
        size_t bufSize = 9;
        String buf[9];
        ObjectWriter owriter;
        AndroidConfig &setObject(String &buf_n, const String &key, const String &value, bool isString, bool last)
        {
            owriter.setObject(buf, bufSize, buf_n, key, value, isString, last);
            return *this;
        }

    public:
        AndroidConfig &collapse_key(const String &value) { return setObject(buf[1], "collapse_key", value, true, true); }

        AndroidConfig &priority(AndroidMessagePriority value)
        {
            if (value == AndroidMessagePriority::NORMAL)
                return setObject(buf[2], "priority", "NORMAL", true, true);
            else if (value == AndroidMessagePriority::_HIGH)
                return setObject(buf[2], "priority", "HIGH", true, true);
            return *this;
        }

        AndroidConfig &ttl(const String &value) { return setObject(buf[3], "ttl", value, true, true); }

        AndroidConfig &restricted_package_name(const String &value) { return setObject(buf[4], "restricted_package_name", value, true, true); }

        AndroidConfig &data(const object_t &value) { return setObject(buf[5], "data", value.c_str(), false, true); }

        AndroidConfig &notification(const AndroidNotification &value) { return setObject(buf[6], "notification", value.c_str(), false, true); }

        AndroidConfig &fcm_options(const AndroidFcmOptions &value) { return setObject(buf[7], "fcm_options", value.c_str(), false, true); }

        AndroidConfig &direct_boot_ok(bool value) { return setObject(buf[8], "direct_boot_ok", owriter.getBoolStr(value), false, true); }

        const char *c_str() const { return buf[0].c_str(); }
        size_t printTo(Print &p) const { return p.print(buf[0].c_str()); }
        void clear() { owriter.clearBuf(buf, bufSize); }
    };

    struct Message : public Printable
    {
    private:
        size_t bufSize = 11;
        String buf[11];
        ObjectWriter owriter;
        Message &setObject(String &buf_n, const String &key, const String &value, bool isString, bool last)
        {
            owriter.setObject(buf, bufSize, buf_n, key, value, isString, last);
            return *this;
        }

    public:
        Message &name(const String &value) { return setObject(buf[1], "name", value, true, true); }
        Message &data(const object_t &value) { return setObject(buf[2], "data", value.c_str(), false, true); }
        Message &notification(const Notification &value) { return setObject(buf[3], "notification", value.c_str(), false, true); }
        Message &android(const AndroidConfig &value) { return setObject(buf[4], "android", value.c_str(), false, true); }
        Message &webpush(const WebpushConfig &value) { return setObject(buf[5], "webpush", value.c_str(), false, true); }
        Message &apns(const ApnsConfig &value) { return setObject(buf[6], "apns", value.c_str(), false, true); }
        Message &fcm_options(const FcmOptions &value) { return setObject(buf[7], "fcm_options", value.c_str(), false, true); }

        Message &token(const String &value)
        {
            // Union field
            if (buf[9].length() == 0 && buf[10].length() == 0)
                return setObject(buf[8], "token", value, true, true);
            return *this;
        }
        Message &topic(const String &value)
        {
            // Union field
            if (buf[8].length() == 0 && buf[10].length() == 0)
                return setObject(buf[9], "topic", value, true, true);
            return *this;
        }
        Message &condition(const String &value)
        {
            // Union field
            if (buf[8].length() == 0 && buf[9].length() == 0)
                return setObject(buf[10], "condition", value, true, true);
            return *this;
        }
        const char *c_str() const { return buf[0].c_str(); }
        size_t printTo(Print &p) const { return p.print(buf[0].c_str()); }
        void clear() { owriter.clearBuf(buf, bufSize); }
    };

    class Parent
    {
        friend class Messaging;

    private:
        String projectId;

    public:
        Parent() {}
        Parent(const String &projectId)
        {
            this->projectId = projectId;
        }
        String getProjectId() const { return projectId; }
    };

    class DataOptions
    {
        friend class Messaging;

    public:
        String payload, extras;
        Messages::Parent parent;
        firebase_cloud_messaging_request_type requestType = firebase_cloud_messaging_request_type_undefined;
        unsigned long requestTime = 0;

        void copy(DataOptions &rhs)
        {
            this->extras = rhs.extras;
            this->payload = rhs.payload;
        }

    private:
    };

    struct async_request_data_t
    {
    public:
        AsyncClientClass *aClient = nullptr;
        String path;
        String uid;
        async_request_handler_t::http_request_method method = async_request_handler_t::http_undefined;
        slot_options_t opt;
        DataOptions *options = nullptr;
        AsyncResult *aResult = nullptr;
        AsyncResultCallback cb = NULL;
        async_request_data_t() {}
        async_request_data_t(AsyncClientClass *aClient, const String &path, async_request_handler_t::http_request_method method, slot_options_t opt, DataOptions *options, AsyncResult *aResult, AsyncResultCallback cb, const String &uid = "")
        {
            this->aClient = aClient;
            this->path = path;
            this->method = method;
            this->opt = opt;
            this->options = options;
            this->aResult = aResult;
            this->cb = cb;
            this->uid = uid;
        }
    };

}

#endif