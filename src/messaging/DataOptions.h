/**
 * Created March 19, 2024
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

// https://firebase.google.com/docs/reference/fcm/rest/v1/projects.messages

#if defined(ENABLE_MESSAGING)

namespace Messages
{
    enum firebase_cloud_messaging_request_type
    {
        firebase_cloud_messaging_request_type_undefined,
        firebase_cloud_messaging_request_type_send
    };

    // Priority of a message to send to Android devices. Note this priority is an FCM concept that controls when the message is delivered. See FCM guides. Additionally, you can determine notification display priority on targeted Android devices using AndroidNotification.NotificationPriority.
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
    enum Visibility
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
    /**
     * Represents a color in the RGBA color space. This representation is designed for simplicity of conversion to/from color representations in various languages over compactness.
     */
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
        // The amount of red in the color as a value in the interval [0, 1].
        Color &red(float value)
        {
            if (value > 1)
                value = 1;
            else if (value < 0)
                value = 0;
            return setObject(buf[1], "red", String(value), false, true);
        }
        // The amount of green in the color as a value in the interval [0, 1].
        Color &green(float value)
        {
            if (value > 1)
                value = 1;
            else if (value < 0)
                value = 0;
            return setObject(buf[2], "green", String(value), false, true);
        }
        // The amount of blue in the color as a value in the interval [0, 1].
        Color &blue(float value)
        {
            if (value > 1)
                value = 1;
            else if (value < 0)
                value = 0;
            return setObject(buf[3], "blue", String(value), false, true);
        }
        // The fraction of this color that should be applied to the pixel. That is, the final pixel color is defined by the equation:
        // pixel color = alpha * (this color) + (1.0 - alpha) * (background color)
        // This means that a value of 1.0 corresponds to a solid color, whereas a value of 0.0 corresponds to a completely transparent color.
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
    /**
     * Settings to control notification LED.
     */
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
        // Required. Set color of the LED with google.type.Color.
        LightSettings &color(Color color) { return setObject(buf[1], "color", color.c_str(), false, true); }
        // Required. Along with light_off_duration, define the blink rate of LED flashes. Resolution defined by proto.Duration
        // A duration in seconds with up to nine fractional digits, ending with 's'. Example: "3.5s".
        LightSettings &light_on_duration(const String &value) { return setObject(buf[2], "light_on_duration", value, true, true); }
        // Required. Along with light_on_duration, define the blink rate of LED flashes. Resolution defined by proto.Duration
        // A duration in seconds with up to nine fractional digits, ending with 's'. Example: "3.5s".
        LightSettings &light_off_duration(const String &value) { return setObject(buf[3], "light_off_duration", value, true, true); }
        const char *c_str() const { return buf[0].c_str(); }
        size_t printTo(Print &p) const { return p.print(buf[0].c_str()); }
        void clear() { owriter.clearBuf(buf, bufSize); }
    };

    /**
     * Notification to send to android devices.
     */
    struct AndroidNotification : public Printable
    {
    private:
        size_t bufSize = 25;
        String buf[25];
        ObjectWriter owriter;
        JSONUtil jut;
        AndroidNotification &setObject(String &buf_n, const String &key, const String &value, bool isString, bool last)
        {
            owriter.setObject(buf, bufSize, buf_n, key, value, isString, last);
            return *this;
        }

    public:
        // The notification's title. If present, it will override google.firebase.fcm.v1.Notification.title.
        AndroidNotification &title(const String &value) { return setObject(buf[1], "title", value, true, true); }
        // The notification's body text. If present, it will override google.firebase.fcm.v1.Notification.body.
        AndroidNotification &body(const String &value) { return setObject(buf[2], "body", value, true, true); }
        // The notification's icon. Sets the notification icon to myicon for drawable resource myicon. If you don't send this key in the request, FCM displays the launcher icon specified in your app manifest.
        AndroidNotification &icon(const String &value) { return setObject(buf[3], "icon", value, true, true); }
        // The notification's icon color, expressed in #rrggbb format.
        AndroidNotification &color(const String &value) { return setObject(buf[4], "color", value, true, true); }
        // The sound to play when the device receives the notification. Supports "default" or the filename of a sound resource bundled in the app. Sound files must reside in /res/raw/.
        AndroidNotification &sound(const String &value) { return setObject(buf[5], "sound", value, true, true); }
        // Identifier used to replace existing notifications in the notification drawer. If not specified, each request creates a new notification. If specified and a notification with the same tag is already being shown, the new notification replaces the existing one in the notification drawer.
        AndroidNotification &tag(const String &value) { return setObject(buf[6], "tag", value, true, true); }
        // The action associated with a user click on the notification. If specified, an activity with a matching intent filter is launched when a user clicks on the notification.
        AndroidNotification &click_action(const String &value) { return setObject(buf[7], "click_action", value, true, true); }
        // The key to the body string in the app's string resources to use to localize the body text to the user's current localization. See String Resources (https://goo.gl/NdFZGI) for more information.
        AndroidNotification &body_loc_key(const String &value) { return setObject(buf[8], "body_loc_key", value, true, true); }
        // Variable string values to be used in place of the format specifiers in body_loc_key to use to localize the body text to the user's current localization. See Formatting and Styling (https://goo.gl/MalYE3) for more information.
        // The item or value will be added to the array or list.
        // To add more items, call this function again.
        AndroidNotification &body_loc_args(const String &value)
        {
            owriter.addMapArrayMember(buf, bufSize, buf[9], "body_loc_args", value, true);
            return *this;
        }
        // The key to the title string in the app's string resources to use to localize the title text to the user's current localization. See String Resources (https://goo.gl/NdFZGI) for more information.
        AndroidNotification &title_loc_key(const String &value) { return setObject(buf[10], "title_loc_key", value, true, true); }
        // Variable string values to be used in place of the format specifiers in title_loc_key to use to localize the title text to the user's current localization. See Formatting and Styling (https://goo.gl/MalYE3) for more information.
        // The item or value will be added to the array or list.
        // To add more items, call this function again.
        AndroidNotification &title_loc_args(const String &value)
        {
            owriter.addMapArrayMember(buf, bufSize, buf[11], "title_loc_args", value, true);
            return *this;
        }
        // The notification's channel id (https://developer.android.com/guide/topics/ui/notifiers/notifications#ManageChannels) (new in Android O). The app must create a channel with this channel ID before any notification with this channel ID is received. If you don't send this channel ID in the request, or if the channel ID provided has not yet been created by the app, FCM uses the channel ID specified in the app manifest.
        AndroidNotification &channel_id(const String &value) { return setObject(buf[12], "channel_id", value, true, true); }
        // Sets the "ticker" text, which is sent to accessibility services. Prior to API level 21 (Lollipop), sets the text that is displayed in the status bar when the notification first arrives.
        AndroidNotification &ticker(const String &value) { return setObject(buf[13], "ticker", value, true, true); }
        // When set to false or unset, the notification is automatically dismissed when the user clicks it in the panel. When set to true, the notification persists even when the user clicks it.
        AndroidNotification &sticky(bool value) { return setObject(buf[14], "sticky", owriter.getBoolStr(value), false, true); }
        // Set the time that the event in the notification occurred. Notifications in the panel are sorted by this time. A point in time is represented using protobuf.Timestamp (https://developers.google.com/protocol-buffers/docs/reference/java/com/google/protobuf/Timestamp).
        // A timestamp in RFC3339 UTC "Zulu" format, with nanosecond resolution and up to nine fractional digits. Examples: "2014-10-02T15:01:23Z" and "2014-10-02T15:01:23.045123456Z".
        AndroidNotification &event_time(const String &value) { return setObject(buf[15], "event_time", value, true, true); }
        // Set whether or not this notification is relevant only to the current device. Some notifications can be bridged to other devices for remote display, such as a Wear OS watch. This hint can be set to recommend this notification not be bridged. See Wear OS guides (https://developer.android.com/training/wearables/notifications/bridger#existing-method-of-preventing-bridging)
        AndroidNotification &local_only(bool value) { return setObject(buf[16], "local_only", owriter.getBoolStr(value), false, true); }
        // Set the relative priority for this notification. Priority is an indication of how much of the user's attention should be consumed by this notification. Low-priority notifications may be hidden from the user in certain situations, while the user might be interrupted for a higher-priority notification. The effect of setting the same priorities may differ slightly on different platforms. Note this priority differs from AndroidMessagePriority. This priority is processed by the client after the message has been delivered, whereas AndroidMessagePriority (https://firebase.google.com/docs/reference/fcm/rest/v1/projects.messages#androidmessagepriority) is an FCM concept that controls when the message is delivered.
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
        // If set to true, use the Android framework's default sound for the notification. Default values are specified in config.xml (https://android.googlesource.com/platform/frameworks/base/+/master/core/res/res/values/config.xml).
        AndroidNotification &default_sound(bool value) { return setObject(buf[18], "default_sound", owriter.getBoolStr(value), false, true); }
        // If set to true, use the Android framework's default vibrate pattern for the notification. Default values are specified in config.xml (https://android.googlesource.com/platform/frameworks/base/+/master/core/res/res/values/config.xml). If default_vibrate_timings is set to true and vibrate_timings is also set, the default value is used instead of the user-specified vibrate_timings.
        AndroidNotification &default_vibrate_timings(bool value) { return setObject(buf[19], "default_vibrate_timings", owriter.getBoolStr(value), false, true); }
        // If set to true, use the Android framework's default LED light settings for the notification. Default values are specified in config.xml (https://android.googlesource.com/platform/frameworks/base/+/master/core/res/res/values/config.xml). If default_light_settings is set to true and light_settings is also set, the user-specified light_settings is used instead of the default value.
        AndroidNotification &default_light_settings(bool value) { return setObject(buf[20], "default_light_settings", owriter.getBoolStr(value), false, true); }
        // Set the vibration pattern to use. Pass in an array of protobuf.Duration (https://developers.google.com/protocol-buffers/docs/reference/google.protobuf#google.protobuf.Duration) to turn on or off the vibrator. The first value indicates the Duration to wait before turning the vibrator on. The next value indicates the Duration to keep the vibrator on. Subsequent values alternate between Duration to turn the vibrator off and to turn the vibrator on. If vibrate_timings is set and default_vibrate_timings is set to true, the default value is used instead of the user-specified vibrate_timings.
        // A duration in seconds with up to nine fractional digits, ending with 's'. Example: "3.5s".
        // The item or value will be added to the array or list.
        // To add more items, call this function again.
        AndroidNotification &vibrate_timings(const String &value)
        {
            owriter.addMapArrayMember(buf, bufSize, buf[21], "vibrate_timings", value, true);
            return *this;
        }
        // Set the Notification.visibility (https://developer.android.com/reference/android/app/Notification.html#visibility) of the notification.
        AndroidNotification &visibility(Visibility value)
        {
            if (value == VISIBILITY_UNSPECIFIED)
                return setObject(buf[22], "visibility", "VISIBILITY_UNSPECIFIED", true, true);
            else if (value == Visibility::PRIVATE)
                return setObject(buf[22], "visibility", "PRIVATE", true, true);
            else if (value == Visibility::PUBLIC)
                return setObject(buf[22], "visibility", "PUBLIC", true, true);
            else if (value == Visibility::SECRET)
                return setObject(buf[22], "visibility", "SECRET", true, true);
            return *this;
        }
        // Sets the number of items this notification represents. May be displayed as a badge count for launchers that support badging.See Notification Badge (https://developer.android.com/training/notify-user/badges). For example, this might be useful if you're using just one notification to represent multiple new messages but you want the count here to represent the number of total new messages. If zero or unspecified, systems that support badging use the default, which is to increment a number displayed on the long-press menu each time a new notification arrives.
        AndroidNotification &notification_count(int value) { return setObject(buf[23], "notification_count", String(value), false, true); }
        // Settings to control the notification's LED blinking rate and color if LED is available on the device. The total blinking time is controlled by the OS.
        AndroidNotification &light_settings(const LightSettings &value) { return setObject(buf[24], "light_settings", value.c_str(), false, true); }
        // Contains the URL of an image that is going to be displayed in a notification. If present, it will override google.firebase.fcm.v1.Notification.image.
        AndroidNotification &image(const String &value) { return setObject(buf[25], "image", value, true, true); }
        const char *c_str() const { return buf[0].c_str(); }
        size_t printTo(Print &p) const { return p.print(buf[0].c_str()); }
        void clear() { owriter.clearBuf(buf, bufSize); }
    };

    /**
     *  Options for features provided by the FCM SDK for Android.
     */
    struct AndroidFcmOptions : public Printable
    {
    private:
        String buf;
        JSONUtil jut;

    public:
        // Label associated with the message's analytics data.
        AndroidFcmOptions &analytics_label(const String &value)
        {
            clear();
            jut.addObject(buf, "analytics_label", value, true, true);
            return *this;
        }
        const char *c_str() const { return buf.c_str(); }
        size_t printTo(Print &p) const { return p.print(buf.c_str()); }
        void clear() { buf.remove(0, buf.length()); }
    };

    /**
     * Options for features provided by the FCM SDK for Web.
     */
    struct WebpushFcmOptions
    {
    private:
        size_t bufSize = 3;
        String buf[3];
        ObjectWriter owriter;
        JSONUtil jut;
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

    /**
     * Platform independent options for features provided by the FCM SDKs.
     */
    struct FcmOptions : public Printable
    {
    private:
        String buf;
        JSONUtil jut;

    public:
        // Label associated with the message's analytics data.
        FcmOptions &analytics_label(const String &value)
        {
            clear();
            jut.addObject(buf, "analytics_label", value, true, true);
            return *this;
        }
        const char *c_str() const { return buf.c_str(); }
        size_t printTo(Print &p) const { return p.print(buf.c_str()); }
        void clear() { buf.remove(0, buf.length()); }
    };

    /**
     * Webpush protocol (https://tools.ietf.org/html/rfc8030) options.
     */
    struct WebpushConfig : public Printable
    {
    private:
        size_t bufSize = 5;
        String buf[5];
        ObjectWriter owriter;
        JSONUtil jut;
        WebpushConfig &setObject(String &buf_n, const String &key, const String &value, bool isString, bool last)
        {
            owriter.setObject(buf, bufSize, buf_n, key, value, isString, last);
            return *this;
        }

    public:
        // HTTP headers defined in webpush protocol. Refer to Webpush protocol for supported headers, e.g. "TTL": "15".
        // An object containing a list of "key": value pairs. Example: { "name": "wrench", "mass": "1.3kg", "count": "3" }.
        WebpushConfig &headers(const object_t &value) { return setObject(buf[1], "headers", value.c_str(), false, true); }
        // Arbitrary key/value payload. If present, it will override google.firebase.fcm.v1.Message.data.
        // An object containing a list of "key": value pairs. Example: { "name": "wrench", "mass": "1.3kg", "count": "3" }.
        WebpushConfig &data(const object_t &value) { return setObject(buf[2], "data", value.c_str(), false, true); }
        // Web Notification options as a JSON object. Supports Notification instance properties as defined in Web Notification API. If present, "title" and "body" fields override google.firebase.fcm.v1.Notification.title and google.firebase.fcm.v1.Notification.body.
        WebpushConfig &notification(const object_t &value) { return setObject(buf[3], "notification", value.c_str(), false, true); }
        // Options for features provided by the FCM SDK for Web.
        WebpushConfig &fcm_options(const WebpushFcmOptions &value) { return setObject(buf[4], "fcm_options", value.c_str(), false, true); }
        const char *c_str() const { return buf[0].c_str(); }
        size_t printTo(Print &p) const { return p.print(buf[0].c_str()); }
        void clear() { owriter.clearBuf(buf, bufSize); }
    };

    /**
     * Options for features provided by the FCM SDK for iOS.
     */
    struct ApnsFcmOptions : public Printable
    {
    private:
        size_t bufSize = 3;
        String buf[3];
        ObjectWriter owriter;
        JSONUtil jut;
        ApnsFcmOptions &setObject(String &buf_n, const String &key, const String &value, bool isString, bool last)
        {
            owriter.setObject(buf, bufSize, buf_n, key, value, isString, last);
            return *this;
        }

    public:
        // Label associated with the message's analytics data.
        ApnsFcmOptions &analytics_label(const object_t &value) { return setObject(buf[1], "analytics_label", value.c_str(), false, true); }
        // Contains the URL of an image that is going to be displayed in a notification. If present, it will override google.firebase.fcm.v1.Notification.image.
        ApnsFcmOptions &image(const object_t &value) { return setObject(buf[2], "image", value.c_str(), false, true); }
        const char *c_str() const { return buf[0].c_str(); }
        size_t printTo(Print &p) const { return p.print(buf[0].c_str()); }
        void clear() { owriter.clearBuf(buf, bufSize); }
    };

    /**
     * Apple Push Notification Service (https://goo.gl/MXRTPa) specific options.
     */
    struct ApnsConfig : public Printable
    {
    private:
        size_t bufSize = 4;
        String buf[4];
        ObjectWriter owriter;
        JSONUtil jut;
        ApnsConfig &setObject(String &buf_n, const String &key, const String &value, bool isString, bool last)
        {
            owriter.setObject(buf, bufSize, buf_n, key, value, isString, last);
            return *this;
        }

    public:
        // HTTP request headers defined in Apple Push Notification Service. Refer to APNs request headers for supported headers such as apns-expiration and apns-priority.
        // The backend sets a default value for apns-expiration of 30 days and a default value for apns-priority of 10 if not explicitly set.
        // An object containing a list of "key": value pairs. Example: { "name": "wrench", "mass": "1.3kg", "count": "3" }.
        ApnsConfig &headers(const object_t &value) { return setObject(buf[1], "headers", value.c_str(), false, true); }
        // APNs payload as a JSON object, including both aps dictionary and custom payload. See Payload Key Reference. If present, it overrides google.firebase.fcm.v1.Notification.title and google.firebase.fcm.v1.Notification.body.
        ApnsConfig &payload(const object_t &value) { return setObject(buf[2], "payload", value.c_str(), false, true); }
        // Options for features provided by the FCM SDK for iOS.
        ApnsConfig &fcm_options(const ApnsFcmOptions &value) { return setObject(buf[3], "fcm_options", value.c_str(), false, true); }
        const char *c_str() const { return buf[0].c_str(); }
        size_t printTo(Print &p) const { return p.print(buf[0].c_str()); }
        void clear() { owriter.clearBuf(buf, bufSize); }
    };

    /**
     * Android specific options for messages sent through FCM connection server (https://goo.gl/4GLdUl).
     */
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
        // An identifier of a group of messages that can be collapsed, so that only the last message gets sent when delivery can be resumed. A maximum of 4 different collapse keys is allowed at any given time.
        AndroidConfig &collapse_key(const String &value) { return setObject(buf[1], "collapse_key", value, true, true); }
        // Message priority. Can take "normal" and "high" values. For more information, see Setting the priority of a message.
        AndroidConfig &priority(AndroidMessagePriority value)
        {
            if (value == AndroidMessagePriority::NORMAL)
                return setObject(buf[2], "priority", "NORMAL", true, true);
            else if (value == AndroidMessagePriority::_HIGH)
                return setObject(buf[2], "priority", "HIGH", true, true);
            return *this;
        }
        // How long (in seconds) the message should be kept in FCM storage if the device is offline. The maximum time to live supported is 4 weeks, and the default value is 4 weeks if not set. Set it to 0 if want to send the message immediately. In JSON format, the Duration type is encoded as a string rather than an object, where the string ends in the suffix "s" (indicating seconds) and is preceded by the number of seconds, with nanoseconds expressed as fractional seconds. For example, 3 seconds with 0 nanoseconds should be encoded in JSON format as "3s", while 3 seconds and 1 nanosecond should be expressed in JSON format as "3.000000001s". The ttl will be rounded down to the nearest second.
        // A duration in seconds with up to nine fractional digits, ending with 's'. Example: "3.5s".
        AndroidConfig &ttl(const String &value) { return setObject(buf[3], "ttl", value, true, true); }
        // Package name of the application where the registration token must match in order to receive the message.
        AndroidConfig &restricted_package_name(const String &value) { return setObject(buf[4], "restricted_package_name", value, true, true); }
        //	Arbitrary key/value payload. If present, it will override google.firebase.fcm.v1.Message.data.
        // An object containing a list of "key": value pairs. Example: { "name": "wrench", "mass": "1.3kg", "count": "3" }.
        AndroidConfig &data(const object_t &value) { return setObject(buf[5], "data", value.c_str(), false, true); }
        // Notification to send to android devices.
        AndroidConfig &notification(const AndroidNotification &value) { return setObject(buf[6], "notification", value.c_str(), false, true); }
        // Options for features provided by the FCM SDK for Android.
        AndroidConfig &fcm_options(const AndroidFcmOptions &value) { return setObject(buf[7], "fcm_options", value.c_str(), false, true); }
        // If set to true, messages will be allowed to be delivered to the app while the device is in direct boot mode. See Support Direct Boot mode (https://developer.android.com/training/articles/direct-boot).
        AndroidConfig &direct_boot_ok(bool value) { return setObject(buf[8], "direct_boot_ok", owriter.getBoolStr(value), false, true); }
        const char *c_str() const { return buf[0].c_str(); }
        size_t printTo(Print &p) const { return p.print(buf[0].c_str()); }
        void clear() { owriter.clearBuf(buf, bufSize); }
    };

    /**
     * Message to send by Firebase Cloud Messaging Service.
     */
    struct Message : public Printable
    {
    private:
        size_t bufSize = 10;
        String buf[10];
        ObjectWriter owriter;
        Message &setObject(String &buf_n, const String &key, const String &value, bool isString, bool last)
        {
            owriter.setObject(buf, bufSize, buf_n, key, value, isString, last);
            return *this;
        }

    public:
        // Input only. Arbitrary key/value payload, which must be UTF-8 encoded. The key should not be a reserved word ("from", "message_type", or any word starting with "google" or "gcm"). When sending payloads containing only data fields to iOS devices, only normal priority ("apns-priority": "5") is allowed in ApnsConfig.
        // An object containing a list of "key": value pairs. Example: { "name": "wrench", "mass": "1.3kg", "count": "3" }.
        Message &data(const object_t &value) { return setObject(buf[1], "data", value.c_str(), false, true); }
        // Input only. Basic notification template to use across all platforms.
        Message &notification(const Notification &value) { return setObject(buf[2], "notification", value.c_str(), false, true); }
        // Input only. Android specific options for messages sent through FCM connection server (https://goo.gl/4GLdUl).
        Message &android(const AndroidConfig &value) { return setObject(buf[3], "android", value.c_str(), false, true); }
        // Input only. Webpush protocol (https://tools.ietf.org/html/rfc8030) options.
        Message &webpush(const WebpushConfig &value) { return setObject(buf[4], "webpush", value.c_str(), false, true); }
        // Input only. Apple Push Notification Service (https://goo.gl/MXRTPa) specific options.
        Message &apns(const ApnsConfig &value) { return setObject(buf[5], "apns", value.c_str(), false, true); }
        // Input only. Template for FCM SDK feature options to use across all platforms.
        Message &fcm_options(const FcmOptions &value) { return setObject(buf[6], "fcm_options", value.c_str(), false, true); }
        // Registration token to send a message to.
        // Union field target.
        Message &token(const String &value)
        {
            // Union field target.
            if (buf[8].length() == 0 && buf[9].length() == 0)
                return setObject(buf[7], "token", value, true, true);
            return *this;
        }
        // Topic name to send a message to, e.g. "weather". Note: "/topics/" prefix should not be provided.
        // Union field target.
        Message &topic(const String &value)
        {
            // Union field target.
            if (buf[7].length() == 0 && buf[9].length() == 0)
                return setObject(buf[8], "topic", value, true, true);
            return *this;
        }
        // Condition to send a message to, e.g. "'foo' in topics && 'bar' in topics".
        // Union field target.
        Message &condition(const String &value)
        {
            // Union field target.
            if (buf[7].length() == 0 && buf[8].length() == 0)
                return setObject(buf[9], "condition", value, true, true);
            return *this;
        }
        void setContent(const String &content)
        {
            owriter.clearBuf(buf, bufSize);
            buf[0]=content;
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

#endif