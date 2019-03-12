#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include "API_connector.h"


// PEKA_response response;
StaticJsonDocument<2048> response; // should be enough


String getPayload() {
  String payload = "method=getTimes&p0={'symbol':'SWRZ01'}";
  return payload;
}


void displayResponse() {
  const char* street = response["success"]["bollard"]["name"];
  Serial.printf("Ulica: %s\n", street);
  // iterate over times
  String times = response["success"]["times"];
}

void showResponse(String responsePayload) {
  DeserializationError error = deserializeJson(response, responsePayload);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
  } else {
    displayResponse();
  }
}


void connect() {
  
    std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);

    client->setFingerprint(fingerprint);

    HTTPClient https;

    Serial.print("[HTTPS] begin...\n");
    if (https.begin(*client, postEndpoint)) {  // HTTPS
        https.addHeader("Content-Type", "application/x-www-form-urlencoded");

        Serial.print("[HTTPS] POST...\n");
        // start connection and send HTTP header
        String payload = getPayload();
        int httpCode = https.POST(payload);

        // httpCode will be negative on error
        if (httpCode > 0) {
            // HTTP header has been send and Server response header has been handled
            Serial.printf("[HTTPS] POST... code: %d\n", httpCode);

            // file found at server
            if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
                String responsePayload = https.getString();
                showResponse(responsePayload);
            }
        } else {
            Serial.printf("[HTTPS] POST... failed, error: %s [%d]\n", https.errorToString(httpCode).c_str(), httpCode);
        }
        https.end();
    } else {
        Serial.printf("[HTTPS] Unable to connect\n");
    } 
}
