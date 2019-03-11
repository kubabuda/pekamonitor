#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>

const char* host = "https://www.peka.poznan.pl/vm/";
const char* fingerprint = "BA:ED:B9:EB:E4:46:D3:16:49:40:34:DC:88:66:76:81:28:68:8B:1D";
const char* postEndpoint = "https://www.peka.poznan.pl/vm/method.vm";
const char* postPayload = "method=getTimes&p0={'symbol':'SWRZ01'}";

ESP8266WiFiMulti WiFiMulti;
const char* ssid = "***REMOVED***";
const char* password = "***REMOVED***";

const int buttonPin = D7;
volatile bool wasPressed = false;


// struct bus_stop {
//   String symbol;
//   String name;
// } typedef bus_stop;

/* VM API works like that:
curl -d "method=getTimes&p0={'symbol':'SWRZ01'}" \
     -H "Content-Type: application/x-www-form-urlencoded" \
     -X POST https://www.peka.poznan.pl/vm/method.vm

  Getting fingerprint:
    echo | openssl s_client -connect peka.poznan.pl:443 | openssl x509 -fingerprint -noout

  BA:ED:B9:EB:E4:46:D3:16:49:40:34:DC:88:66:76:81:28:68:8B:1D

 */

// bus_stop stops[] = {
//   { .symbol = "SWRZ01", .name = "Świerzawska->Ławica" },
//   { .symbol = "SWRZ02", .name = "Świerzawska->Kaponiera" },
//   { .symbol = "RYCE04", .name = "Rycerska->Kaponiera" },
// };
// int stopsCount = 2;
// volatile int currentStop = 0;
// String postParams = "method=getTimes&p0={'symbol':'BIAL02'}";



void handleKeyPress();
int httpsGET(const char *host, const char *fingerprint, String url);
void connect();


void setup()
{
  Serial.begin(115200);
  Serial.println();

  pinMode(buttonPin, INPUT); // TODO use internal pullup to omit ext resistor
  attachInterrupt(digitalPinToInterrupt(buttonPin), handleKeyPress, RISING);

  // display.println("I could stick around");
  // display.println("get along with you");
  // display.println("hello.");
  
  Serial.printf("Connecting to %s ", ssid);
  Serial.print("Connecting to ");
  Serial.println(ssid);
 
  Serial.flush();
  delay(1000);
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(ssid, password);
  
  Serial.println(" connected");
}


void loop()
{
  if (wasPressed) {
    connect();
    wasPressed = false;
  }
  delay(5000);
}

void connect() {
  if ((WiFiMulti.run() == WL_CONNECTED)) {

    std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);

    client->setFingerprint(fingerprint);

    HTTPClient https;

    Serial.print("[HTTPS] begin...\n");
    if (https.begin(*client, postEndpoint)) {  // HTTPS
      https.addHeader("Content-Type", "application/x-www-form-urlencoded");

      Serial.print("[HTTPS] POST...\n");
      // start connection and send HTTP header
      int httpCode = https.POST(postPayload);

      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTPS] POST... code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          String payload = https.getString();
          Serial.println(payload);
        }
      } else {
        Serial.printf("[HTTPS] POST... failed, error: %s [%d]\n", https.errorToString(httpCode).c_str(), httpCode);
      }

      https.end();
    } else {
      Serial.printf("[HTTPS] Unable to connect\n");
    }
  }
}


void connect_get() {
  if ((WiFiMulti.run() == WL_CONNECTED)) {

    std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);

    client->setFingerprint(fingerprint);

    HTTPClient https;

    Serial.print("[HTTPS] begin...\n");
    if (https.begin(*client, host)) {  // HTTPS

      Serial.print("[HTTPS] GET...\n");
      // start connection and send HTTP header
      int httpCode = https.GET();
      
      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTPS] GET... code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          String payload = https.getString();
          Serial.println(payload);
        }
      } else {
        Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
      }

      https.end();
    } else {
      Serial.printf("[HTTPS] Unable to connect\n");
    }
  }
}

void handleKeyPress() {
  // currentStop = (currentStop + 1) % stopsCount;
  wasPressed = true;
}