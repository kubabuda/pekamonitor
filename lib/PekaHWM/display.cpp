#include <Arduino.h>
#include <ArduinoJson.h>
#include <Wire.h>    // Include Wire for using I2C
#include "API_connector.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


#define I2C_ADDRESS 	0x3C
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


void testdrawchar(void) {
  display.clearDisplay();

  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner
  display.cp437(true);         // Use full 256 char 'Code Page 437' font

  // Not all the characters will fit on the display. This is normal.
  // Library will draw what it can and the rest will be clipped.
  for(int16_t i=0; i<256; i++) {
    if(i == '\n') display.write(' ');
    else          display.write(i);
  }

  display.display();
  delay(2000);
}

void testdrawstyles(void) {
  display.clearDisplay();

  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  display.println(F("Hello, world!"));

  display.setTextColor(BLACK, WHITE); // Draw 'inverse' text
  display.println(3.141592);

  display.setTextSize(2);             // Draw 2X-scale text
  display.setTextColor(WHITE);
  display.print(F("0x")); display.println(0xDEADBEEF, HEX);

  display.display();
  delay(2000);
}

void testscrolltext(void) {
  display.clearDisplay();

  display.setTextSize(2); // Draw 2X-scale text
  display.setTextColor(WHITE);
  display.setCursor(10, 0);
  display.println(F("scroll"));
  display.display();      // Show initial text
  delay(100);

  // Scroll in various directions, pausing in-between:
  display.startscrollright(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
  delay(1000);
  display.startscrollleft(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
  delay(1000);
  display.startscrolldiagright(0x00, 0x07);
  delay(2000);
  display.startscrolldiagleft(0x00, 0x07);
  delay(2000);
  display.stopscroll();
  delay(1000);
}


const int displayLinesCount = 6; // how many lines of text fits on display

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

	display.setTextSize(1);      // Normal 1:1 pixel scale
	display.setTextColor(WHITE); // Draw white text
	display.setCursor(0, 0);     // Start at top-left corner
	display.cp437(true);         // Use full 256 char 'Code Page 437' font

	display.println(F("PEKA HW Monitor"));
	
  display.display();
}

void displaySetupDone() {
	displayCleanup();

	display.setCursor(0, 0);
	display.println("Setup");
	display.setCursor(20, 6);
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
