#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <Wire.h>    // Include Wire for using I2C
#include <SFE_MicroOLED.h>  // Include the SFE_MicroOLED library


const uint8_t fingerprint[20] =  { 0xBA, 0xED, 0xB9, 0xEB, 0xE4, 0x46, 0xD3, 0x16, 0x49, 0x40, 0x34, 0xDC, 0x88, 0x66, 0x76, 0x81, 0x28, 0x68, 0x8B, 0x1D };
const char* postEndpoint = "https://www.peka.poznan.pl/vm/method.vm";
const char* payload = "method=getTimes&p0={'symbol':'SWRZ01'}";

ESP8266WiFiMulti WiFiMulti;
const char* ssid = "***REMOVED***";
const char* password = "***REMOVED***";

const int buttonPin = D7;
volatile bool wasPressed = false;

// struct bus_stop {
//   String symbol;
//   String name;
// } typedef bus_stop;

// bus_stop stops[] = {
//   { .symbol = "SWRZ01", .name = "Świerzawska->Ławica" },
//   { .symbol = "SWRZ02", .name = "Świerzawska->Kaponiera" },
//   { .symbol = "RYCE04", .name = "Rycerska->Kaponiera" },
// };
// int stopsCount = 2;
// volatile int currentStop = 0;


//////////////////////////
// MicroOLED Definition //
//////////////////////////
//The library assumes a reset pin is necessary. The Qwiic OLED has RST hard-wired, so pick an arbitrarty IO pin that is not being used
#define PIN_RESET 255  
//The DC_JUMPER is the I2C Address Select jumper. Set to 1 if the jumper is open (Default), or set to 0 if it's closed.
#define DC_JUMPER 0

//////////////////////////////////
// MicroOLED Object Declaration //
//////////////////////////////////
MicroOLED oled(PIN_RESET, DC_JUMPER);    // I2C declaration



// methods declarations
void handleKeyPress();
void connect();


void setup()
{
  Serial.begin(115200);
  Serial.println();

  pinMode(buttonPin, INPUT); // TODO use internal pullup to omit ext resistor
  attachInterrupt(digitalPinToInterrupt(buttonPin), handleKeyPress, RISING);

  Wire.begin();
  oled.begin();    // Initialize the OLED
  oled.clear(ALL); // Clear the display's internal memory
  oled.clear(PAGE); // Clear the buffer.
  oled.println("I could");
  oled.println("stick around");
  oled.println("get along with you");
  oled.println("hello.");
  oled.display();
  
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
      // https.setTimeout(100000);

      Serial.print("[HTTPS] POST...\n");
      // start connection and send HTTP header
      int httpCode = https.POST(payload);

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

void handleKeyPress() {
  // currentStop = (currentStop + 1) % stopsCount;
  wasPressed = true;
}
