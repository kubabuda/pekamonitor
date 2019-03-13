#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include "API_connector.h"


String getPayload(String symbol) {
  String payload = "method=getTimes&p0={'symbol':'" + symbol + "'}";
  return payload;
}

int connect_dummy(String symbol, JsonDocument& response) {
    // pretend connection, read from hardcoded JSON
    const String responsePayload = "{\"success\":\{\"bollard\":{\"symbol\":\"RKAP71\",\"tag\":\"RKAP01\",\"name\":\"Rondo Kaponiera\",\"mainBollard\":false},\"times\":[{\"realTime\":false,\"minutes\":13,\"direction\":\"Rondo Kaponiera\",\"onStopPoint\":false,\"departure\":\"2019-03-12T00:21:00.000Z\",\"line\":\"249\"},{\"realTime\":true,\"minutes\":16,\"direction\":\"Rondo Kaponiera\",\"onStopPoint\":false,\"departure\":\"2019-03-12T00:24:00.000Z\",\"line\":\"232\"},{\"realTime\":true,\"minutes\":17,\"direction\":\"Rondo Kaponiera\",\"onStopPoint\":false,\"departure\":\"2019-03-12T00:25:00.000Z\",\"line\":\"238\"},{\"realTime\":false,\"minutes\":22,\"direction\":\"Szwajcarska Szpital\",\"onStopPoint\":false,\"departure\":\"2019-03-12T00:30:00.000Z\",\"line\":\"232\"},{\"realTime\":false,\"minutes\":22,\"direction\":\"Szwajcarska Szpital\",\"onStopPoint\":false,\"departure\":\"2019-03-12T00:30:00.000Z\",\"line\":\"238\"},{\"realTime\":false,\"minutes\":22,\"direction\":\"Dębiec\",\"onStopPoint\":false,\"departure\":\"2019-03-12T00:30:00.000Z\",\"line\":\"249\"}]}}";
    // response payload is large, keep it and deserialization in one scope
    DeserializationError error = deserializeJson(response, responsePayload);
    
    if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.c_str());
        return -1;
    }

    return 0;
}

int connect(String symbol, JsonDocument& response) {
    int result = -1;
    std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);

    client->setFingerprint(fingerprint);

    HTTPClient https;

    Serial.print("[HTTPS] begin...\n");
    if (https.begin(*client, postEndpoint)) {  // HTTPS
        https.addHeader("Content-Type", "application/x-www-form-urlencoded");

        Serial.print("[HTTPS] POST...\n");
        // start connection and send HTTP header
        String payload = getPayload(symbol);
        int httpCode = https.POST(payload);
        yield();

        // httpCode will be negative on error
        if (httpCode > 0) {
            // HTTP header has been send and Server response header has been handled
            Serial.printf("[HTTPS] POST... code: %d\n", httpCode);

            // file found at server
            if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
                                
                // deserializing stream by ArduinoJSON is not supported on ESP8266
                String responsePayload = https.getString();    
                // deserialize JSON response
                DeserializationError error = deserializeJson(response, responsePayload);
                if (error) {
                    Serial.print(F("deserializeJson() failed: "));
                    Serial.println(error.c_str());
                } else {
                    // succes
                }
            }
        } else {
            Serial.printf("[HTTPS] POST... failed, error: %s [%d]\n", https.errorToString(httpCode).c_str(), httpCode);
        }
        // cleanup
        https.end();
        result = httpCode;
    } else {
        Serial.printf("[HTTPS] Unable to connect\n");
    }

    return result;
}
