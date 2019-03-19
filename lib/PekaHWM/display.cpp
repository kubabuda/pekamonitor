#include <Arduino.h>
#include <ArduinoJson.h>
#include <Wire.h>    // Include Wire for using I2C
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <U8g2_for_Adafruit_GFX.h>
#include "API_connector.h"


#define I2C_ADDRESS 	0x3C
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

const int displayLinesCount = 6;    // how many lines of text fits on display
const int lineHeight = 9;           // monitor line h, in px
const int directionShortSize = 12;  // how many characters of direction to display
const int linePaddedSize = 5;       // tram/bus line no can be up to 3 digits
                                    // + N symbol for night connections + \0 str end

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
U8G2_FOR_ADAFRUIT_GFX u8g2_for_adafruit_gfx;


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
    u8g2_for_adafruit_gfx.begin(display);                 // connect u8g2 procedures to Adafruit GFX

    u8g2_for_adafruit_gfx.setFont(u8g2_font_helvB08_tf);  // select u8g2 font from here: https://github.com/olikraus/u8g2/wiki/fntlistall
    u8g2_for_adafruit_gfx.setFontMode(1);                 // use u8g2 transparent mode (this is default)
    u8g2_for_adafruit_gfx.setFontDirection(0);            // left to right (this is default)
    u8g2_for_adafruit_gfx.setForegroundColor(WHITE);      // apply Adafruit GFX color

    const char s[] = "gfx LCD";
    /* width and height of the text */
    int16_t height;
    int16_t width;
    /* desired position of the text */
    int16_t x = 4;
    int16_t y = 25;
    display.clearDisplay();                               // clear the graphcis buffer  
    u8g2_for_adafruit_gfx.setCursor(x, y);                // start writing at this position
    u8g2_for_adafruit_gfx.print(s);
    u8g2_for_adafruit_gfx.setCursor(0,50);                // start writing at this position
    u8g2_for_adafruit_gfx.print("Zażółć gęślą jaźń");            // UTF-8 string with german umlaut chars
    /* calculate the size of the box into which the text will fit */
    height = u8g2_for_adafruit_gfx.getFontAscent() - u8g2_for_adafruit_gfx.getFontDescent();
    width = u8g2_for_adafruit_gfx.getUTF8Width(s);

    /* draw the box around the text*/
    display.drawRect(x, y-u8g2_for_adafruit_gfx.getFontAscent(), width, height, WHITE);
    display.display();                                    // make everything visible
    delay(4000);



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


void displayMonitorHeader(const char *name, const char* symbol) {
  	Serial.printf("Przystanek %s\n %s", symbol, name);
	displayCleanup();
    u8g2_for_adafruit_gfx.setCursor(0, 8);
    u8g2_for_adafruit_gfx.println(name);
}


void displayMonitorLine(JsonVariant& time, int lineNo) {
    const char* line = time["line"];
    const char* direction = time["direction"];
    int minutes = time["minutes"].as<int>();
    bool realTime = time["realTime"].as<bool>();

    // display deaparture time details on serial
    Serial.printf(" - %s w kierunku %s za %d min%s\n", line, direction, minutes,
        realTime ? "" : " [wg rozkladu]");
    // display deaparture time details on display
    if(lineNo <= displayLinesCount) {
        char directionShort[directionShortSize + 1]; // allowance for \0
        char linePadded[linePaddedSize] = { ' ', ' ', ' ', ' ', '\0' }; 
        
        // prepare bus/tram line # padded with spaces, TODO: can it be done with string format?
        strncpy(linePadded, line, strlen(line));
        // prepare direction shortened to predefined size, todo: padding as with line
        strlcpy(directionShort, direction, directionShortSize);

        display.setCursor(0, lineNo * lineHeight);
        display.printf("%s%s: %d%c", linePadded, directionShort, minutes, realTime ? 'm' : '*');
    }
}


void displayResponse(JsonDocument& response) {
    // parse monitor header
    const char* name =  response["success"]["bollard"]["name"];
	const char* symbol =  response["success"]["bollard"]["symbol"];
    // display monitor header
    displayMonitorHeader(name, symbol);

    // iterate over departure times
    JsonArray times = response["success"]["times"].as<JsonArray>();
    int lineNo = 0;
    for(JsonVariant time : times) {
		// parse time properties, TODO: to struct?
        ++lineNo;
        displayMonitorLine(time, lineNo);

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

