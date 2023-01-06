# Motion Sensor

It's Arduino code for NodeMCU ESP8266 client with a motion sensor. You can use any model you want.

## How-to

1. Create your env file from `env.cpp.example` :

   ```cpp
    #include <Arduino.h>
    #include "env.h"

    const char* wifi_ssid = "";
    const char* wifi_password = "";

    const String alert_url = "";
    const uint8_t alert_url_fingerprint[20] = {};
   ```

2. Set proper alert URL with it's SHA1 fingerprint. For example, you can use Telegram Bot API.

3. Upload the project to your device.

## Work process

1. Connect to WiFi network
2. Detect motion
   1. Count HIGH value from digital input pin with 10ms delay
   2. If `count > 10` we have a real motion
3. Send `POST` request to provided URL
4. Try again if `response != 200,301`
