
Revision `2024-10-27T20:40:15Z`

# Async Firebase Client library for Arduino Frequently Asked Questions.

## Table of Contents

- [Q1: Why I get an error `"TCP connection failed"`?](#q1-why-i-get-an-error-tcp-connection-failed)
- [Q2: Why I get an error `"TCP send failed"`?](#q1-why-i-get-an-error-tcp-connection-failed)
- [Q3: Why I get an error `"TCP receive timed out"`?](#q3-why-i-get-an-error-tcp-receive-timed-out)
- [Q4: Why I get an error `"TCP disconnected"`?](#q4-why-i-get-an-error-tcp-disconnected)
- [Q5: What should I do when I get the error `"error opening file"`?](#q5-what-should-i-do-when-i-get-the-error-error-opening-file)
- [Q6: What should I do when I get the error `"error reading file"` or `"error writng file"`?](#q6-what-should-i-do-when-i-get-the-error-error-reading-file-or-error-writng-file)
- [Q7: What should I do when ESP32 crashed with the error `"Exception was unhandled"`?](#q7-what-should-i-do-when-esp32-crashed-with-the-error-exception-was-unhandled)
- [Q8: What should I do when I get the error `"unauthenticate"`?](#q8-what-should-i-do-when-i-get-the-error-unauthenticate)
- [Q9: What should I do when I get the error `"JWT, private key parsing fail"`?](#q9-what-should-i-do-when-i-get-the-error-jwt-private-key-parsing-fail)
- [Q10: What should I do when I get the error `"JWT, token signing fail"`?](#q10-what-should-i-do-when-i-get-the-error-jwt-token-signing-fail)
- [Q11: What should I do when I get the error `"too low sketch space"`?](#q11-what-should-i-do-when-i-get-the-error-too-low-sketch-space)
- [Q12: What should I do when I get the error `"firmware write failed"` or `"firmware end failed"`?](#q12-what-should-i-do-when-i-get-the-error-firmware-write-failed-or-firmware-end-failed)
- [Q13: What should I do when I get the error `"stream connection timed out"`?](#q13-what-should-i-do-when-i-get-the-error-stream-connection-timed-out)
- [Q14: What should I do when I get the error `"auth revoked"`?](#q14-what-should-i-do-when-i-get-the-error-auth-revoked)
- [Q15: Why I get the error `"app was not assigned"`?](#q15-why-i-get-the-error-app-was-not-assigned)
- [Q16: Why I get the error `"operation was cancelled"`?](#q16-why-i-get-the-error-operation-was-cancelled)
- [Q17: Why I get the error `"JWT, time was not set or not valid"`?](#q17-why-i-get-the-error-jwt-time-was-not-set-or-not-valid)
- [Q18: Why sometimes the Realtime Database Stream does not work when WiFi was disconnected and reconnected and I get the error `"stream connection timed out"`?](#q18-why-sometimes-the-realtime-database-stream-does-not-work-when-wifi-was-disconnected-and-reconnected-and-i-get-the-error-stream-connection-timed-out)
- [Q19: Can I use delay in this library?](#q19-can-i-use-delay-in-this-library)
- [Q20: Can I change the send timeout?](#q20-can-i-change-the-send-timeout)
- [Q21: Can I change the read timeout?](#q21-can-i-change-the-read-timeout)
- [Q22: Why my device crashed?](#q22-why-my-device-crashed)
- [Q23: How I restart the Realtime Database stream?](#q23-how-can-i-restart-the-realtime-database-stream)
- [Q24: How can I stop all tasks?](#q24-how-can-i-stop-all-tasks)
- [Q25: How can I stop authentication task?](#q25-how-can-i-stop-authentication-task)
- [Q26: How can I run more than one Realtime Database Stream with the same async client?](#q26-how-can-i-run-more-than-one-realtime-database-stream-with-the-same-async-client)
- [Q27: How can I run Realtime Database task and Cloud Firestore Database task using the same async client?](#q27-how-can-i-run-realtime-database-task-and-cloud-firestore-database-task-using-the-same-async-client)
- [Q28: Why my Arduino MKR board failed to connect to the server?](#q28-why-my-arduino-mkr-board-failed-to-connect-to-the-server)
- [Q29: Why my SIM7600 failed to connect to the server?](#q29-why-my-sim7600-failed-to-connect-to-the-server)
- [Q30: How to stop the authentication task?](#q30-how-to-stop-the-authentication-task)
- [Q31: How to remove authentication task handler from Firebase service app?](#q31-how-to-remove-authentication-task-handler-from-firebase-service-app)

### Q1: Why I get an error `"TCP connection failed"`?

#### A1: The server was disconnected or the SSL client is unable to connect to the server.

It should be noted that the user defined SSL client that assigned with async client was used for server connection, sending and receiving the data to/from server.

Library also provides the option to resume the network connection in case of network connection disconnected.

In case WiFi, if the Core SDK provides reconnection function, library will reconnect to WiFi using that function otherwise the WiFi AP credentials are required.

In case Ethernet, if external Ethernet client was used, library will provided the configuarations to allow Ethernet module enabling/resetting and initialization.

In case GSM, it requires the configurations for initialize the TinyGSM modem.

In case generic client, it required the callback function to handle the network connection/reconnection.

Then there are many possible causes of connection issue that cause the SSL client fail to connect to server.

#### Possible WiFi issues

- If you are using ESP32 device, and the reconnect option in `DefaultNetwork` was enabled. The bugs in outdated ESP32 Core WiFi will cause the unknown network connection issue when library is trying to reconnect your WiFi using `WiFi.reconnect()`. You have to update the ESP32 Arduino Core SDK to latest version or at least v2.x.x.

- When the WiFi was managed for connection and reconnection by user code or third party library e.g. WiFiManager, you have to set the reconnect option in `DefaultNetwork` to false otherwise you are unable to re-connect to the WiFi due to interferences operations.

- The voltage drops or noisy power supply cable can cause the server connection failure even WiFi was connected. Replace the power cable or usb cable with shorter length. Connect the large capacitor to your board Vcc and GND or use robust power supply.

- The poor WiFi signal and WiFi channels interference. In fist case you have to move your device close to the WiFi hotspot or extend the range of your WiFi. The later case, you can change the WiFi channel of your WiFi hotspot.

- WiFi AP was not responding because of static IP. You can connect your device to WiFi AP using DHCP instead.

- In ESP32, ADC channel 2 cannot be used while using WiFi.

- ESP8266/ESP32 ESP-Now conflicts. You can't use WiFi while using ESP-Now.

- ESP32 Bluetooth conflicts. You can't use WiFi while using its Bluetooth.

#### Possible WiFi firmware issues

- If you are using `Arduino UNO R4 WiFi`, `Arduino Portenta C33`, `Arduino MKR WiFi 1010`, `Arduino NANO 33 IoT`, and `Arduino Nano RP2040`, please make sure you have already done the following.
  - Add the `Realtime Database host root certificate` to the board firmware. Plese see [Upload SSL root certificates](https://support.arduino.cc/hc/en-us/articles/360016119219-Upload-SSL-root-certificates) for how to.
  - Install the WiFiNINA library.

- If you are using `Arduino® MKR 1000 WIFI`, please make sure you have already done the following.
  - Add the `Realtime Database host root certificate` to the board firmware. 
  - Install the WiFi101 library and uninstall or remove the WiFiNINA library from the libraries folder if it was installed.

#### Possible Ethernet issues

- The cable problem. Check your Ethernet cable.

- Ethernet module failure. Change your Ethernet module.

- The router was not responding because of static IP. You can connect your device to your router using DHCP instead.

- The Ethernet library issue. The buffer allocated in Ethernet library is too snall and cause the SSL Client fail to decrypt the incomplete SSL data. You should increase the buffer size or reduce the number of socket connections.

#### Possible GSM issues

- Incorrect TinyGSMClient library configuration. The GSM module macros e.g. `TINY_GSM_MODEM_SIM7600` should be defined in two locations.
   1. In [src/Config.h](/src/Config.h) or in your own defined config at [src/UserConfig.h](/src) or adding `TINY_GSM_MODEM_SIM7600` in compiler build flags.
   2. In your sketch.

- GSM library issue. Follow the GSM library examples. Post your issue in the GSM library repository.

- GSM module failure, SIM failure and the mobile network problems. Change your GSM module, SIM and contact Mobile network operator.

#### Memory issues

- Memory allocation failure due to out of memory. The SSL client may fail to allocate memory required during SSL/TLS handshake. You have to reduce the memory used by your code and inspect memory usage for memory leaks.


### Q2: Why I get an error `"TCP send failed"`?

#### A2: The time that used while sending the request (header or payload) is greater than the send timeout.

- The delay function and user blocking code are used in the same loop that async task handler is running and they blocked the async task handler to work. You have to remove the delay function from loop function or minimize the time that used by your blocking code.

- The poor WiFi signal, voltage drops, noisy power supply and the network problems. See [Q1: Why I get an error `"TCP connection failed"`?](#q1-why-i-get-an-error-tcp-connection-failed).

- Poor internet condition. Many devices connected on the same network may shere the internet connection. Change the router, AP or network.

- In case of upload, the file read operation may take longer time because some harware problemsor memory issue in filesystems. Change your media card its adapter and wirings between your device and SD/MMC module or adapter. If you are working with SD and you are not using ESP8266, please try the [SdFat](https://github.com/greiman/SdFat) library instead. 

- The send timeout is too small. In case sync task, the send timeout can be set via `AsyncClientClass::setSyncSendTimeout`. The async task send timeout is 30 seconds and cannot be changed and you have to fix the possible issues instead of increasing the send timeout.

### Q3: Why I get an error `"TCP receive timed out"`?

#### A3: The time that used while processing the server response (header or payload) is greater than the read timeout.

- The delay function and user blocking codes are used in the same loop that async task handler is running and they blocked the async task handler to work. You have to remove the delay function from loop function or minimize the time that used by your blocking code.

- The poor WiFi signal, voltage drops, noisy power supply and the network problems. See [Q1: Why I get an error `"TCP connection failed"`?](#q1-why-i-get-an-error-tcp-connection-failed).

- Poor internet condition. Many devices connected on the same network may shere the internet connection. Change the router, AP or network.

- In case of download, the file write operation may take longer time because some harware problems or memory issue in filesystems. Change your media card its adapter and wirings between your device and SD/MMC module or adapter. If you are working with SD and you are not using ESP8266, please try the [SdFat](https://github.com/greiman/SdFat) library instead. 

- The read timeout is too small. In case sync task, the read timeout can be set via `AsyncClientClass::setSyncReadTimeout`. The async task read timeout is 30 seconds and cannot be changed and you have to fix the above issues instead of increasing the timeout.

### Q4: Why I get an error `"TCP disconnected"`?

#### A4: The network was disconnected.

Read [Q1: Why I get an error `"TCP connection failed"`?](#q1-why-i-get-an-error-tcp-connection-failed).

### Q5: What should I do when I get the error `"error opening file"`?

#### A5: The error occurred while opening the file.

- File does not exist for reading. Check for the file existent when open the file for reading.

- The filesytem is not mounted. Mount your filesystem and try to reformat your filesystems.

- The SD card and its adapter failure or wirings between your device and card problems. Change your SD card, adapter and check the wirings.

- Memory allocation failure due to out of memory. The filesystem may not work in this case and you have to solve the memory issue. You have to reduce the memory used by your code and inspect memory usage for memory leaks.


### Q6: What should I do when I get the error `"error reading file"` or `"error writng file"`?

#### A6: The error occurred while reading from or writing to filesystem.

If filesytem was mounted successfully.

- The SD card and its adapter failure or wirings between your device and card problems. Change your SD card, adapter and check the wirings.

- Memory allocation failure due to out of memory. The filesystem may not work in this case and you have to solve the memory issue. You have to reduce the memory used by your code and inspect memory usage for memory leaks.

### Q7: What should I do when ESP32 crashed with the error `"Exception was unhandled"`?

```
13:07:18.043 -> Guru Meditation Error: Core  1 panic'ed (LoadProhibited). Exception was unhandled.
13:07:18.043 -> 
13:07:18.043 -> Core  1 register dump:
13:07:18.043 -> PC      : 0x400ed967  PS      : 0x00060d30  A0      : 0x800eda90  A1      : 0x3ffdc7f0  
13:07:18.043 -> A2      : 0x3ffdc854  A3      : 0x00000000  A4      : 0x3ffdc8c4  A5      : 0x3ffdc8cc  
13:07:18.075 -> A6      : 0x3ffdc8dc  A7      : 0x00000000  A8      : 0x00000000  A9      : 0xcccccccd  
13:07:18.075 -> A10     : 0x00000000  A11     : 0x00000000  A12     : 0x00000000  A13     : 0x0000ff00  
13:07:18.075 -> A14     : 0x00ff0000  A15     : 0xff000000  SAR     : 0x00000020  EXCCAUSE: 0x0000001c  
13:07:18.075 -> EXCVADDR: 0x00000002  LBEG    : 0x4008b215  LEND    : 0x4008b225  LCOUNT  : 0xffffffff 
``` 

#### A7: It is the dangling pointer issue.

The dangling pointer issue occurred when you are trying to use the object that point by the pointer that was deconstructed or not existed.

The memory allocation failure due to out of memory can cause the dangling pointer as the pointer is still point to the unallocated memory.


### Q8: What should I do when I get the error `"unauthenticate"`?

#### A8: The auth token is not ready while processing the request.

You have to wait for the authentication task to be ready before calling the Firebase functions.

After [App Initialization](https://github.com/mobizt/FirebaseClient#app-initialization), you can wait (blocking operation) or checking the return value of `FirebaseApp::ready` in the loop.


### Q9: What should I do when I get the error `"JWT, private key parsing fail"`?

#### A9: The RSA private key that uses in JWT token signing process is not valid.

The private key can be obtained from the service account credentials. Please read [Service Account](https://github.com/mobizt/FirebaseClient#service-account) for more details.

### Q10: What should I do when I get the error `"JWT, token signing fail"`?

#### A10: The JWT token signing with RSA private key was failed.

This can be the out of memory issue. You have to reduce the memory usage and check for memory leaks. 

### Q11: What should I do when I get the error `"too low sketch space"`?

#### A11: The flash space in the current partition is not enough for use in OTA update process.

Please check the partition size. 

### Q12: What should I do when I get the error `"firmware write failed"` or `"firmware end failed"`?

#### A12: The OTA download process is unterruped and cause the data wrtting failure or the library unknown error.

In case download process interrupted, see [Q1: Why I get an error `"TCP connection failed"`?](#q1-why-i-get-an-error-tcp-connection-failed). 

In case unkbown error, please verify your free flash and do not write any file to the filesytem that decreased the free flash space. 

### Q13: What should I do when I get the error `"stream connection timed out"`?

#### A13: The error can be occured when no keep-alive event received during the stream timeout period.

To check whether Stream is timed out or not, use `AsyncResult::to<RealtimeDatabaseResult>()::eventTimeout()`.

For the reasons, please read [Possible WiFi issues](#possible-wifi-issues).

### Q14: What should I do when I get the error `"auth revoked"`?

#### A14: The auth token is expired.

The delay function and user blocking code are used in the same loop that authentication task handler is running and they blocked the authentication task handler to work. You have to remove the delay function from loop function or minimize the time that used by your blocking code.

### Q15: Why I get the error `"app was not assigned"`?

#### A15: The FirebaseApp was not applied to the Firebase Service apps.

You have to apply the FirebaseApp by using `FirebaseApp::getApp`. Please read [App Initialization](https://github.com/mobizt/FirebaseClient#app-initialization) for more details.

### Q16: Why I get the error `"operation was cancelled"`?

#### A16: The async client's queue is full due to too many new tasks are added while the old tasks that waiting in the queue are not yet processed.

The currently running task in the queue may be slow because of the delay function and user blocking codes are used in the same loop that async task handler is running and they blocked the async task handler to work. You have to remove the delay function from loop function or minimize the time that used by your blocking code.

Some network problem can be the cause, see [Q1: Why I get an error `"TCP connection failed"`?](#q1-why-i-get-an-error-tcp-connection-failed).

When more than one `SSE mode (HTTP Streaming)`task was add to the queue. You cannot run more than one stream per async client. Please see [Q26: How can I run more than one Realtime Database Stream with the same async client?](#q26-how-can-i-run-more-than-one-realtime-database-stream-with-the-same-async-client).

When no more memory to be allocated for new task. You have to reduce the memory usage.

### Q17: Why I get the error `"JWT, time was not set or not valid"`?

#### A17: The time is not set or not valid

The timestamp that you set in the uint32_t variable of time status callback function is not valid or it was not set.

### Q18: Why sometimes the Realtime Database Stream does not work when WiFi was disconnected and reconnected and I get the error `"stream connection timed out"`?

#### A18: The problem is due to the WiFi reconnection interferences.

Please read [Possible WiFi issues](#possible-wifi-issues).

### Q19: Can I use delay in this library?

#### A19: Yes, but it's not recommend.

If the delay was used in the loop it causes many errors e.g. read and send timed out and stream connection timed out.

### Q20: Can I change the send timeout?

#### A20: Yes.

The sync task send timeout can be set via `AsyncClientClass::setSyncSendTimeout`. The async task send timeout is 30 seconds and cannot be changed.

### Q21: Can I change the read timeout?

#### A21: Yes.

The sync task read timeout can be set via `AsyncClientClass::setSyncReadTimeout`. The async task read timeout is 30 seconds and cannot be changed.

### Q22: Why my device crashed?

#### A22: There are many possible causes.

- Out of memory. Reduce memory usage and check memory usage for memory leaks.

- Stack over flow. When you define class objects, variables and calling the function inside the callback function, they use stack memory

Then you should avoid the stack overflow problem by moving all codes in the callback to run in the main loop function instead.

- The dangling pointer issue.

It is because you are trying to use the object that point by the pointer that was destroyed or not existed.

The memory allocation failure due to out of memory can cause the dangling pointer as the pointer is still point to unallocated memory.


### Q23: How can I restart the Realtime Database stream?

#### A23: Normally Realtime Database stream task is running and stay in the queue in all conditions.

If network disconnected and re-connected, network connected but internet access was missing, and no events included keep-alive event received within 40 seconds, the Stream will be closed and restarted automatically as long as the async client's queue was running.

The `stream connection timed out`error will show in case timed out (no events received within 40 seconds).

If you are intend to totally remove the steam task from the queue, and stream is running, use `AsyncClientClass::stopAsync()`. If you are not sure that stream was running or stopped because of other tasks, use `AsyncClientClass::stopAsync(true)` to cancell all tasks and remove all tasks from the queue.

The `AsyncClientClass::stopAsync()` will stop only the running task and remove from queue while `AsyncClientClass::stopAsync(true)` will stop the running task and remove all tasks in the queue.

### Q24: How can I stop all tasks?
#### A24: You can use `AsyncClientClass::stopAsync(true)` to stop all tasks in the queue.

### Q25: How can I stop authentication task?
#### A25: Normally you cannot stop the authentication task unless you can remove the queue from loop.

You have to remove the functions; `FirebaseApp::loop()` and/or `FirebaseServicesApps::loop()`(if the same async client was used in `FirebaseServicesApps` and `FirebaseApp`) from the `loop`.

### Q26: How can I run more than one Realtime Database Stream with the same async client?
#### A26: No, it is not possible by design.

The `SSE mode (HTTP Streaming)` task is a kind of `infinite task` which the server connection was kept alive and waiting for the incoming event data.

You cannot run more than one `infinite task` in the same async client's queue as one `infinite task` is never finished, and another `infinite task` is never started.

To run many `SSE mode (HTTP Streaming)` tasks, you have to run each task in different async client. Please note that `SSE mode (HTTP Streaming)` task consumes memory all the time while it is running. Running many `SSE mode (HTTP Streaming)` tasks may not possible because of out of memory.

For running more tasks concurrency, see [Running Many Tasks Concurrency Using Different Async Clients (In Different SSL Clients)](https://github.com/mobizt/FirebaseClient#running-many-tasks-concurrency-using-different-async-clients-in-different-ssl-clients).

### Q27: How can I run Realtime Database task and Cloud Firestore Database task using the same async client?
#### A27: Yes, you can run different Firebase Products using the same async client or different async client.

When you run different Firebase products using the same async client, all tasks are in the same queue and it needs to start the new server connection for different host of Firebase products. This takes time when establish the new server connection.

When you run different Firebase products using the different async clients, all tasks are in the different queues. When tasks are running concurrency, it consumed more memory which may not suitable for low memory devices.

For running more tasks concurrency, see [Running Many Tasks Concurrency Using Different Async Clients (In Different SSL Clients)](https://github.com/mobizt/FirebaseClient#running-many-tasks-concurrency-using-different-async-clients-in-different-ssl-clients).

### Q28: Why my Arduino MKR board failed to connect to the server?
#### A28: Please see, [Possible WiFi issues](#possible-wifi-issues) and [Possible WiFi firmware issues](#possible-wifi-firmware-issues).

### Q29: Why my SIM7600 failed to connect to the server?
#### A29: Please see, [Possible GSM issues](#possible-gsm-issues).

### Q30: How to stop the authentication task?
#### A30: Normally the authentication task was unable to stop after initialized unless it can deinitialize.

You can use `deinitializeApp(<FirebaseApp>)` to deinitalize the `FirebaseApp` that was initialized.

You also can block the `FirebaseApp::loop()` from the running in loop to pause the authentication task but it is not recommended.

### Q31: How to remove authentication task handler from Firebase service app?
#### A31: You can use `<FirebaseServiceApp>::resetApp()` to unbind or remove the `FirebaseApp` from the Firebase service app.
