#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include "API_connector.h"

const size_t RESPONSE_SIZE = 2048;

StaticJsonDocument<RESPONSE_SIZE> response;

String getPayload() {
  String payload = "method=getTimes&p0={'symbol':'SWRZ01'}";
  return payload;
}


void displayResponse() {
    // Show results, for now on serial port
    // display monitor header
    const char* street = response["success"]["bollard"]["name"];
    Serial.printf("Przystanek %s\n", street);
    // iterate over times. commenting it out causes connection refused(-1), IDK why
    JsonArray times = response["success"]["times"].as<JsonArray>();
    
    for(JsonVariant v : times) {
        const char* line = v["line"];
        const char* direction = v["direction"];
        int minutes = v["minutes"].as<int>();
        bool realTime = v["realTime"].as<bool>();
    
        Serial.printf(" - %s w kierunku %s za %d min%s\n", line, direction, minutes,
            realTime ? "" : " [wg rozkladu]");
    }
}


void connect() {
    String responsePayload = "{\"success\":\{\"bollard\":{\"symbol\":\"RKAP71\",\"tag\":\"RKAP01\",\"name\":\"Rondo Kaponiera\",\"mainBollard\":false},\"times\":[{\"realTime\":false,\"minutes\":13,\"direction\":\"Rondo Kaponiera\",\"onStopPoint\":false,\"departure\":\"2019-03-12T00:21:00.000Z\",\"line\":\"249\"},{\"realTime\":true,\"minutes\":16,\"direction\":\"Rondo Kaponiera\",\"onStopPoint\":false,\"departure\":\"2019-03-12T00:24:00.000Z\",\"line\":\"232\"},{\"realTime\":true,\"minutes\":17,\"direction\":\"Rondo Kaponiera\",\"onStopPoint\":false,\"departure\":\"2019-03-12T00:25:00.000Z\",\"line\":\"238\"},{\"realTime\":false,\"minutes\":22,\"direction\":\"Szwajcarska Szpital\",\"onStopPoint\":false,\"departure\":\"2019-03-12T00:30:00.000Z\",\"line\":\"232\"},{\"realTime\":false,\"minutes\":22,\"direction\":\"Szwajcarska Szpital\",\"onStopPoint\":false,\"departure\":\"2019-03-12T00:30:00.000Z\",\"line\":\"238\"},{\"realTime\":false,\"minutes\":22,\"direction\":\"Dębiec\",\"onStopPoint\":false,\"departure\":\"2019-03-12T00:30:00.000Z\",\"line\":\"249\"}]}}";
    DeserializationError error = deserializeJson(response, responsePayload);
    if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.c_str());
    } else {
        displayResponse();
    }
    return;
  
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
                // maybe deserialize using ArduinoJSON stream?
                String responsePayload = https.getString();
                DeserializationError error = deserializeJson(response, responsePayload);
                if (error) {
                    Serial.print(F("deserializeJson() failed: "));
                    Serial.println(error.c_str());
                } else {
                    displayResponse();
                }
            }
        } else {
            Serial.printf("[HTTPS] POST... failed, error: %s [%d]\n", https.errorToString(httpCode).c_str(), httpCode);
        }
        https.end();
    } else {
        Serial.printf("[HTTPS] Unable to connect\n");
    } 
}
