#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include "API_connector.h"
#include "PEKA_models.h"
#include "secrets.h" // const char* ssid, password are outside Git. Change lib/Secrets/_secret.h name and values


ESP8266WiFiMulti WiFiMulti;

StaticJsonDocument<MAX_RESPONSE_SIZE> response;

const int buttonPin = D7;
volatile bool wasPressed = false;


// methods declarations
void handleKeyPress();


void setup()
{
	Serial.begin(115200);
	Serial.flush();
	Serial.println("  \r");

	displaySetup();
	
	Serial.printf("\rConnecting to %s ", ssid);
	delay(500);
	WiFi.mode(WIFI_STA);
	WiFiMulti.addAP(ssid, password);	
	Serial.println("finished");
	
	pinMode(buttonPin, INPUT); // TODO use internal pullup to omit ext resistor
	attachInterrupt(digitalPinToInterrupt(buttonPin), handleKeyPress, RISING);
	
	displaySetupDone();
}

void loop()
{
	static volatile unsigned long prevReload = 0;
  	String symbol = bollards[currentBollard].symbol;
  
  	if (isReloadNeeded(wasPressed, prevReload)) {
      	if ((WiFiMulti.run() == WL_CONNECTED)) {
			unsigned long start = millis();
      		Serial.printf("[%lu] Reloading bollard info for ", start);
			Serial.println(symbol);
      
			int statusCode = reloadBollard(symbol, response);
			yield();
			
			if(statusCode > 0) {
				displayResponse(response);
				prevReload = start;
			}
		
			Serial.printf("[%lu] Reload done\n", millis());
    	} else {
      		Serial.printf("[WARN] Request omited, wifi not connected\n");
    	} 
    	wasPressed = false;
  	}
}


void handleKeyPress() {
	static volatile uint32_t prev = 0;
	auto debounce_time_ms = 200;
	auto now = millis();   // TODO there must be lib for this
	
	if(now > prev + debounce_time_ms) { // TODO add guard
		prev = now;
		currentBollard = (currentBollard + 1) % bollardsCount; // TODO race condition wigh symbol selection, or debounce error
		wasPressed = true;
	}
}
