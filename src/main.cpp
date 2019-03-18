#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESPRotary.h>
#include "API_connector.h"
#include "secrets.h" // const char* ssid, password are outside Git. Change lib/Secrets/_secret.h name and values


ESP8266WiFiMulti WiFiMulti;

const int clkPin = D7;	 // CLK
const int dtPin = D4; 	  // DT
const int buttonPin = D3; // SW

ESPRotary rotary = ESPRotary(dtPin, clkPin);


// methods declarations
void rotary_loop();

void setupWifi() {
	Serial.printf("\rConnecting to %s ", ssid);
	delay(500);
	WiFi.mode(WIFI_STA);
	WiFiMulti.addAP(ssid, password);	
	Serial.println("finished");
}


void setup()
{
	Serial.begin(115200);
	Serial.flush();
	Serial.println("  \r");

	displaySetup();
	
	setupWifi();
	
	pinMode(buttonPin, INPUT);
	attachInterrupt(digitalPinToInterrupt(buttonPin), reloadCurrentBollard, FALLING);
	
	rotary.setLeftRotationHandler(incrementCurrentBollard);
  	rotary.setRightRotationHandler(incrementCurrentBollard);

	displaySetupDone();
}


void loop()
{
	StaticJsonDocument<MAX_RESPONSE_SIZE> response;
	String symbol = getCurrentBollard();
  	reload_state next_action = isReloadNeeded();
  	
	if (next_action) {
      	if ((WiFiMulti.run() == WL_CONNECTED)) {
			auto start = millis();
      		Serial.printf("[%lu] Loading bollard info for ", start);
			Serial.println(symbol); 
			
			if (next_action == RELOAD_TRIGGERED) {
				displayLoading(symbol);
			}
      
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

