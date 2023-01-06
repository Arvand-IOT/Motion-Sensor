#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include "env.h"

int inputPin = 16;  // ESP8266 - D0
const String userAgent = "Arvand-IOT/Motion-Sensor";
int detected = 0;
bool alarm = false;
bool sent = true;

ESP8266WiFiMulti WiFiMulti;

void setup() {
  pinMode(inputPin, INPUT);
  Serial.begin(115200);

  // Connect to WiFi
  setup_wifi();
}

void setup_wifi() {
  delay(10);

  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(wifi_ssid, wifi_password);

  Serial.println("");
  Serial.print("[SETUP] Connecting ");
  while (WiFiMulti.run() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("[SETUP] Connected to ");
  Serial.println(wifi_ssid);
  Serial.print("[SETUP] IP address: ");
  Serial.println(WiFi.localIP());
}

void alert() {
  if (WiFiMulti.run() == WL_CONNECTED) {
    Serial.println("[ALARM] Send alert to server");

    std::unique_ptr<BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);
    client->setFingerprint(alert_url_fingerprint);

    HTTPClient https;
    https.setUserAgent(userAgent);

    Serial.println("[ALARM] Begin");
    if (https.begin(*client, alert_url)) {
      Serial.println("[ALARM] POST");

      int httpCode = https.POST("");
      if (httpCode > 0) {
        Serial.printf("[ALARM] POST ... code: %d\n", httpCode);

        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          // String payload = https.getString();
          Serial.println("[ALARM] Done");
          sent = true;
        }
      } else {
        Serial.printf("[ALARM] POST ... failed, code: %d - error: %s\n", httpCode, https.errorToString(httpCode).c_str());
      }

      https.end();
    } else {
      Serial.println("[ALARM] Unable to connect");
    }
  } else {
    Serial.println("[ALARM] Check WiFi connection");
  }
}

void loop() {
  int val = digitalRead(inputPin);

  if (val == HIGH) {
    detected = detected + 1;
  } else {
    detected = 0;
    alarm = false;
  }

  delay(10);

  if (detected > 10 && !alarm && sent) {
    Serial.println("[SENSOR] Motion detected !");
    alarm = true;
    sent = false;
    alert();
  }

  if (!sent) {
    Serial.println("[ALARM] Try again");
    alert();
  }
}
