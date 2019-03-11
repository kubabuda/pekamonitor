#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>

// const char* host = "https://www.peka.poznan.pl/vm/method.vm";
const char* host = "https://www.peka.poznan.pl/vm/";
const char* fingerprintPeka = "BA:ED:B9:EB:E4:46:D3:16:49:40:34:DC:88:66:76:81:28:68:8B:1D";

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
 
  for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(ssid, password);
  
  Serial.println(" connected");
}


void loop()
{
  if (wasPressed) {
    connect();
  }
  
  delay(5000);
}

void connect() {
  const uint8_t fingerprint[20] = {0x5A, 0xCF, 0xFE, 0xF0, 0xF1, 0xA6, 0xF4, 0x5F, 0xD2, 0x11, 0x11, 0xC6, 0x1D, 0x2F, 0x0E, 0xBC, 0x39, 0x8D, 0x50, 0xE0};
  
  if ((WiFiMulti.run() == WL_CONNECTED)) {

    std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);

    client->setFingerprint(fingerprintPeka);

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

  Serial.println("Wait 10s before next round...");
  delay(10000);
}


void connect_4() {
  const uint8_t fingerprint[20] = {0x5A, 0xCF, 0xFE, 0xF0, 0xF1, 0xA6, 0xF4, 0x5F, 0xD2, 0x11, 0x11, 0xC6, 0x1D, 0x2F, 0x0E, 0xBC, 0x39, 0x8D, 0x50, 0xE0};
  
  if ((WiFiMulti.run() == WL_CONNECTED)) {

    std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);

    client->setFingerprint(fingerprint);

    HTTPClient https;

    Serial.print("[HTTPS] begin...\n");
    if (https.begin(*client, "https://jigsaw.w3.org/HTTP/connection.html")) {  // HTTPS

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

  Serial.println("Wait 10s before next round...");
  delay(10000);
}

void connect_3() {
  const char* host = "api.github.com";
  const int httpsPort = 443;

// Use web browser to view and copy
// SHA1 fingerprint of the certificate
  const char fingerprint[] = "5F F1 60 31 09 04 3E F2 90 D2 B0 8A 50 38 04 E8 37 9F BC 76";

  WiFiClientSecure client;
  Serial.print("connecting to ");
  Serial.println(host);

  Serial.printf("Using fingerprint '%s'\n", fingerprint);
  client.setFingerprint(fingerprint);

  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }

  String url = "/repos/esp8266/Arduino/commits/master/status";
  Serial.print("requesting URL: ");
  Serial.println(url);

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: BuildFailureDetectorESP8266\r\n" +
               "Connection: close\r\n\r\n");

  Serial.println("request sent");
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      Serial.println("headers received");
      break;
    }
  }
  String line = client.readStringUntil('\n');
  if (line.startsWith("{\"state\":\"success\"")) {
    Serial.println("esp8266/Arduino CI successfull!");
  } else {
    Serial.println("esp8266/Arduino CI has failed");
  }
  Serial.println("reply was:");
  Serial.println("==========");
  Serial.println(line);
  Serial.println("==========");
  Serial.println("closing connection");
}

void connect_second() {
  const char* GOOGLE_HOST = "script.google.com";
  const char* FINGERPRINT = "29 40 55 83 01 36 FA 5D BD 5E 6B 91 58 29 EA 8F 26 7E 22 0B";
  String GOOGLE_URL = "/macros/s/{script_id}/exec";
  #define TAG_NAME "SoilMoistureLevel"
  #define SOIL_PIN 0

  String url = GOOGLE_URL + "?tag=" + TAG_NAME + "&value=" + 20;

  Serial.print("[-] Sending GET to URL : ");
  Serial.println(url);

  int retCode = httpsGET(GOOGLE_HOST, FINGERPRINT, url);
  if (retCode == 200 || retCode == 201) {
      Serial.println("[-] Reporting succeeded.");
  } else {
        Serial.print("[!] Reporting failed: Code ");
      Serial.println(retCode);          
  }
  Serial.println("[-] Sleeping");
  // ESP.deepSleep(36e8); // 3600 sec / 1 hr
  ESP.deepSleep(36e6); // 36 sec
}

void connect_first() {
  WiFiClient client;
  Serial.printf("\n[Connecting to %s ... ", host);
  if (client.connect(host, 80))
  {
    Serial.println("connected]");

    Serial.println("[Sending a request]");
    client.print(String("GET /") + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n" +
                 "\r\n"
                );

    Serial.println("[Response:]");
    while (client.connected() || client.available())
    {
      if (client.available())
      {
        String line = client.readStringUntil('\n');
        Serial.println(line);
      }
    }
    client.stop();
    Serial.println("\n[Disconnected]");
  }
  else
  {
    Serial.println("connection failed!]");
    client.stop();
  }
}

int httpsGET(const char *host, const char *fingerprint, String url) {
    WiFiClientSecure client;
    
    Serial.print("Connecting to: ");
    Serial.println(host);
    if (!client.connect(host, 443)) {
        Serial.println("connection failed");
        return -1;
    }
    
    if (client.verify(fingerprint, host)) {
        Serial.println("certificate matches");
    } else {
        Serial.println("certificate doesn't match");
    }

    // Send GET
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: ESP8266\r\n" +
               "Connection: close\r\n\r\n");
    
    Serial.println("GET sent");
    while (client.connected()) {
        String line = client.readStringUntil('\n');
        if (line.startsWith("HTTP/1.1")) {
            // Get HTTP return code
            return line.substring(9,12).toInt();
        }
    }
    
    return -1;
}

void handleKeyPress() {
  // currentStop = (currentStop + 1) % stopsCount;
  wasPressed = true;
}