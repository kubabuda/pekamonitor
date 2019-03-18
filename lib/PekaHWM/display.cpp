#include <Arduino.h>
#include <ArduinoJson.h>
#include <Wire.h>    // Include Wire for using I2C
#include "API_connector.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


#define I2C_ADDRESS 	0x3C
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

const int displayLinesCount = 6; // how many lines of text fits on display
const int lineHeight = 9;
const int directionShortSize = 12;
const int linePaddedSize = 5;

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


void displayCleanup() {
    display.clearDisplay();
    display.setCursor(0, 0);
}

void displaySetup() {
	// SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    if(!display.begin(SSD1306_SWITCHCAPVCC, I2C_ADDRESS, false)) {
      	Serial.println(F("SSD1306 allocation failed"));
      	for(;;); // Don't proceed, loop forever
    }
    display.clearDisplay();

    display.setTextColor(WHITE); // Draw white text
    display.setTextSize(2); 
    display.cp437(true);

    display.setCursor(40, 20);
    display.println(F("PEKA"));
    display.setCursor(5, 36);
    display.println(F("HW Monitor"));

    display.setTextSize(1);
    
    display.display();
}

void displaySetupDone() {
    displayCleanup();
    display.setTextSize(2); 

    display.setCursor(36, 16);
    display.println("Setup");
    display.setCursor(36, 36);
    display.println("done.");
    display.setTextSize(1);
    
    display.display();
}


void displayLoading(String symbol) {
	displayCleanup();
	
  display.setTextSize(1);

	display.setCursor(0, 16); 
	display.print("Loading ");
	display.println(symbol);
	display.print("...");

	display.display();
}


void displayResponse(JsonDocument& response) {
    // parse monitor header
    const char* name =  response["success"]["bollard"]["name"];
	const char* symbol =  response["success"]["bollard"]["symbol"];
    // parse display monitor header
  	Serial.printf("Przystanek %s\n %s", symbol, name);
	displayCleanup();
	display.println(name);

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
			char directionShort[directionShortSize + 1]; // allowance for \0
			
			char linePadded[linePaddedSize] = { ' ', ' ', ' ', ' ', '\0' }; 
			
			// prepare line no padded with spaces, TODO: can it be done with string format?
			strncpy(linePadded, line, strlen(line));
			// prepare direction shortened to predefined size, todo: padding as with line
			strlcpy(directionShort, direction, directionShortSize);

			display.setCursor(0, lineNo * lineHeight);
			display.printf("%s%s: %d", linePadded, directionShort, minutes);
			display.print(realTime ? "m" : "*");
		}

        yield();
    }

    display.display();
}


void displayLoadingFailed(String symbol) {
	displayCleanup();
	
	display.println("Loading ");
	display.println(symbol);
	display.println(" failed!");

	display.display();
};

