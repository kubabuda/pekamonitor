#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESPRotary.h>
#include "API_connector.h"
#include "PEKA_models.h"
#include "secrets.h" // const char* ssid, password are outside Git. Change lib/Secrets/_secret.h name and values


ESP8266WiFiMulti WiFiMulti;

StaticJsonDocument<MAX_RESPONSE_SIZE> response;

const int clkPin = D3;
const int dtPin = D2;
const int buttonPin = D1;
volatile bool wasPressed = false;

ESPRotary rotary = ESPRotary(D2, D3);


// methods declarations
void reloadCurrentBollard();
void incrementCurrentBollard(ESPRotary& r);
void decrementCurrentBollard(ESPRotary& r);


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
	
	pinMode(buttonPin, INPUT);
	attachInterrupt(digitalPinToInterrupt(buttonPin), reloadCurrentBollard, FALLING);
	
	rotary.setLeftRotationHandler(decrementCurrentBollard);
  	rotary.setRightRotationHandler(incrementCurrentBollard);

	displaySetupDone();
}


String getCurrentBollard() {
	return bollards[currentBollard].symbol;
}

void loop()
{
	static volatile unsigned long prevReload = 0;
  	String symbol = getCurrentBollard();
  
  	if (isReloadNeeded(wasPressed, prevReload)) {
      	if ((WiFiMulti.run() == WL_CONNECTED)) {
			auto start = millis();
      		Serial.printf("[%lu] Loading bollard info for ", start);
			Serial.println(symbol);
      
			int statusCode = reloadBollard(symbol, response);
			yield();
			
			if(statusCode > 0) {
				displayResponse(response);
				prevReload = start;
			}
		
			Serial.printf("[%lu] Loading done\n", millis());
    	} else {
      		Serial.printf("[WARN] Load rRequest omited, wifi not connected\n");
    	} 
    	wasPressed = false;
  	}

	rotary.loop();
}


static volatile uint32_t prev = 0;
const uint32_t debounce_time_ms = 200;


void incrementCurrentBollard(ESPRotary& r) {
	auto now = millis();
	
	if (now > prev + debounce_time_ms) {
		prev = now;
		wasPressed = true;
		
		++currentBollard;
		currentBollard %= bollardsCount;

		Serial.printf("[%lu] Switching bollard # up to ", now);
		Serial.println(getCurrentBollard());
	}
}


void reloadCurrentBollard() {
	auto now = millis();
	
	if (now > prev + debounce_time_ms) {
		prev = now;
		wasPressed = true;
		
		Serial.printf("[%lu] Reloading bollard info for ", now);
		Serial.println(getCurrentBollard());
	}	
}


void decrementCurrentBollard(ESPRotary& r) {
	auto now = millis();
	
	if (now > prev + debounce_time_ms) {
		prev = now;
		wasPressed = true;

		--currentBollard;
		if (currentBollard < 0) {
			currentBollard = bollardsCount - 1;
		}

		Serial.printf("[%lu] Switching bollard # down to ", now);
		Serial.println(getCurrentBollard());
	}
}
