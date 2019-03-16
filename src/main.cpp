#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESPRotary.h>
#include "API_connector.h"
#include "secrets.h" // const char* ssid, password are outside Git. Change lib/Secrets/_secret.h name and values


ESP8266WiFiMulti WiFiMulti;

StaticJsonDocument<MAX_RESPONSE_SIZE> response;

const int clkPin = D7;	 // CLK
const int dtPin = D4; 	  // DT
const int buttonPin = D3; // SW

ESPRotary rotary = ESPRotary(dtPin, clkPin);


// methods declarations

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


void loop()
{
	String symbol = getCurrentBollard();
  
  	if (isReloadNeeded()) {
      	// if ((WiFiMulti.run() == WL_CONNECTED)) {
		if (true) {
			auto start = millis();
      		Serial.printf("[%lu] Loading bollard info for ", start);
			Serial.println(symbol); 
			displayLoading(symbol);
      
			int statusCode = reloadBollard(symbol, response);
			yield();
			
			if(statusCode > 0) {
				displayResponse(response);
				setLastReloadTimestamp(start);
			} else
			{
				displayLoadingFailed(symbol);
			}
			
			Serial.printf("[%lu] Loading done\n", millis());
    	} else {
      		Serial.printf("[WARN] Load request omited, wifi not connected\n");
			// todo display WiFi conn problem
    	}
  	}

	rotary.loop();
}
