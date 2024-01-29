#ifndef FIREBASE_CONFIG_H
#define FIREBASE_CONFIG_H

/** 📌 Predefined Build Options
 * ⛔ Use following build flag to disable all predefined options.
 * -D FIREBASE_DISABLE_ALL_OPTIONS
 */

/**📍 For RTDB class compilation
 * ⛔ Use following build flag to disable.
 * -D DISABLE_DATABASE
 */
#define ENABLE_DATABASE

/**📍 For Firestore compilation
 * ⛔ Use following build flag to disable.
 * -D DISABLE_FIRESTORE
 */
// #define ENABLE_FIRESTORE

/**📍 For Firebase Cloud Messaging compilation
 * ⛔ Use following build flag to disable.
 * -D DISABLE_MESSAGING
 */
// #define ENABLE_MESSAGING

/**📍 For Firebase Storage compilation
 * ⛔ Use following build flag to disable.
 * -D DISABLE_STORAGE
 */
// #define ENABLE_STORAGE

/**📍 For Google Cloud Storage compilation
 * ⛔ Use following build flag to disable.
 * -D DISABLE_CLOUD_STORAGE
 */
// #define ENABLE_CLOUD_STORAGE

/**📍 For Functions for Firebase compilation
 * ⛔ Use following build flag to disable.
 * -D DISABLE_FUNCTIONS
 */
// #define ENABLE_FUNCTIONS

/**📍 For enabling PSRAM support
 * ⛔ Use following build flag to disable.
 * -D DISABLE_PSRAM
 */
#define ENABLE_PSRAM

/**📍 For enabling OTA updates support via RTDB, Firebase Storage and Google Cloud Storage buckets
 * ⛔ Use following build flag to disable.
 * -D DISABLE_OTA
 */
#define ENABLE_OTA

/**📍 For enabling filesystem
 * ⛔ Use following build flag to disable.
 * -D DISABLE_FS
 */
#define ENABLE_FS

/**📍 For enabling authentication and token
 * ⛔ Use following build flag to disable.
 * -D DISABLE_SERVICE_AUTH
 * -D DISABLE_CUSTOM_AUTH
 * -D DISABLE_USER_AUTH
 * -D DISABLE_ACCESS_TOKEN
 * -D DISABLE_CUSTOM_TOKEN
 * -D DISABLE_ID_TOKEN
 * -D DISABLE_LEGACY_TOKEN
 */
#define ENABLE_SERVICE_AUTH
#define ENABLE_CUSTOM_AUTH
#define ENABLE_USER_AUTH
#define ENABLE_ACCESS_TOKEN
#define ENABLE_CUSTOM_TOKEN
#define ENABLE_ID_TOKEN
#define ENABLE_LEGACY_TOKEN

#include "core/Options.h"

#endif