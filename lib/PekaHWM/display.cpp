#include <Arduino.h>
#include <ArduinoJson.h>
#include <Wire.h>    // Include Wire for using I2C
#include "API_connector.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);























const int displayLinesCount = 6; // how many lines of text fits on display

void displayCleanup() {
    display.clearDisplay();
    display.setCursor(0, 0);
}

void displaySetup() {
	// SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
	if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
		Serial.println(F("SSD1306 allocation failed"));
		for(;;); // Don't proceed, loop forever
	}

	// Show initial display buffer contents on the screen --
	// the library initializes this with an Adafruit splash screen.
	display.display();
	delay(2000); // Pause for 2 seconds

	Serial.printf("SSD1306 started OK with screen %dx%d\r\n",display.width(), display.height());

	display.clearDisplay();
	
	display.drawRect(
		0,
		0,
		display.width(),
		display.height(),
		WHITE );
	display.display();
	display.setTextSize( 1 );
	
	display.setCursor( 2, 0);
	display.println("Witajcie");
	
	display.setCursor( 2, 1);
	display.print("Witajcie");

	display.setCursor( 2, 2);
	display.printf("Witajcie");

	display.setCursor( 2, 4);
	display.printf_P("Witajcie");
	
	display.display();
	
	// display.setCursor(5, 5); 
	// display.setCursor(5, 6);
	// display.print("start...");
	Serial.printf("SSD should display hello world now");
	// display.display();
	delay(2000); // Pause for 2 seconds
}

void displaySetupDone() {
	displayCleanup();

	display.setCursor(5, 5);
	display.println("Setup");
	display.setCursor(5, 6);
	display.println("  done.");
	
	display.display();
}


void displayLoading(String symbol) {
	displayCleanup();
	
	display.setCursor(5, 5); 
	display.println("  Loading");
	display.setCursor(5, 6); 
	display.println(symbol);
	display.setCursor(5, 7);
	display.println("   ...");

	display.display();
}


void displayResponse(JsonDocument& response) {

    // parse monitor header
    const char* name =  response["success"]["bollard"]["name"];
	const char* symbol =  response["success"]["bollard"]["symbol"];
    // parse display monitor header
	Serial.printf("Przystanek %s\n %s", symbol, name);
	displayCleanup();
	display.print(symbol);

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
			display.setCursor(6, lineNo);
			display.print(line);
			display.print(" - ");
			display.print(minutes);
			display.println(realTime ? "m" : "*");
		}

        yield();
    }

    display.display();
}


void displayLoadingFailed(String symbol) {
	displayCleanup();
	
	display.println("");
	display.println("");
	display.println("  Loading");
	display.print("  ");
	display.println(symbol);
	display.println("  failed!");

	display.display();
};
