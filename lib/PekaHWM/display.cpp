#include <Arduino.h>
#include <ArduinoJson.h>
#include <Wire.h>    // Include Wire for using I2C
// #include <SFE_MicroOLED.h>  // Include the SFE_MicroOLED library
#include "API_connector.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


//The library assumes a reset pin is necessary. The Qwiic OLED has RST hard-wired, so pick an arbitrarty IO pin that is not being used
#define PIN_RESET 255  
//The DC_JUMPER is the I2C Address Select jumper. Set to 1 if the jumper is open (Default), or set to 0 if it's closed.
#define DC_JUMPER 0


const int displayLinesCount = 6; // how many lines of text fits on display

void displayCleanup() {
    oled.clearDisplay(); // Clear the display's internal memory
    // oled.(PAGE); // Clear the buffer.
    oled.setCursor(0, 0);
}

void displaySetup() {
	// Wire.begin();	// TODO benchmark and compare with SPI
	
	// SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
	if(!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
		Serial.println(F("SSD1306 allocation failed"));
		for(;;); // Don't proceed, loop forever
	}

	// Show initial display buffer contents on the screen --
	// the library initializes this with an Adafruit splash screen.
	oled.display();
	delay(2000); // Pause for 2 seconds

	// oled.begin();   // Initialize the OLED
	displayCleanup();
	
	oled.println("");
	oled.println("PEKA HWM");
	oled.println("");
	oled.println("start...");
	
	oled.display();
}

void displaySetupDone() {
	displayCleanup();

	oled.println("");
	oled.println("");
	oled.println("  Setup");
	oled.println("  done.");
	
	oled.display();
}


void displayLoading(String symbol) {
	displayCleanup();
	
	oled.println("");
	oled.println("");
	oled.println("  Loading");
	oled.print("  ");
	oled.println(symbol);
	oled.println("   ...");

	oled.display();
}


void displayResponse(JsonDocument& response) {

    // parse monitor header
    const char* name =  response["success"]["bollard"]["name"];
	const char* symbol =  response["success"]["bollard"]["symbol"];
    // parse display monitor header
	Serial.printf("Przystanek %s\n %s", symbol, name);
	displayCleanup();
	oled.println(symbol);

    // iterate over departure times
    JsonArray times = response["success"]["times"].as<JsonArray>();
    int lineNo = 0;
    for(JsonVariant v : times) {
		// parse time properties
        ++lineNo;
        const char* line = v["line"];
        const char* direction = v["direction"];
        int minutes = v["minutes"].as<int>();
        bool realTime = v["realTime"].as<bool>();

		// display deaparture time details on serial
        Serial.printf(" - %s w kierunku %s za %d min%s\n", line, direction, minutes,
            realTime ? "" : " [wg rozkladu]");
		// display deaparture time details on display
		if(lineNo <= displayLinesCount) {
			oled.print(line);
			oled.print(" - ");
			oled.print(minutes);
			oled.println(realTime ? "m" : "*");
		}

        yield();
    }

    oled.display();
}


void displayLoadingFailed(String symbol) {
	displayCleanup();
	
	oled.println("");
	oled.println("");
	oled.println("  Loading");
	oled.print("  ");
	oled.println(symbol);
	oled.println("  failed!");

	oled.display();
};
