#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include "API_connector.h"
#include "PEKA_models.h"
#include "display.h"
#include "secrets.h" // const char* ssid, password are outside Git. Change lib/Secrets/_secret.h name and values


ESP8266WiFiMulti WiFiMulti;

const int buttonPin = D7;
volatile bool wasPressed = false;


// methods declarations
void handleKeyPress();


void setup()
{
  Serial.begin(115200);
  Serial.println("  \r");

  displaySetup();
  
  Serial.printf("\rConnecting to %s ", ssid);
  Serial.flush();
  delay(500);
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(ssid, password);
  
  Serial.println(" finished");
  
  pinMode(buttonPin, INPUT); // TODO use internal pullup to omit ext resistor
  attachInterrupt(digitalPinToInterrupt(buttonPin), handleKeyPress, RISING);
 
  displaySetupDone();
}


void loop()
{
  if (wasPressed) {
    if ((WiFiMulti.run() == WL_CONNECTED)) {
      connect("SWRZ01");
      displayResponse();
    } else {
      Serial.printf("[WARN] Request omited, wifi not connected\n");
    } 
    wasPressed = false;
  }
  delay(1000);
}


void handleKeyPress() {
  currentBollard = (currentBollard + 1) % bollardsCount;
  wasPressed = true;
}
