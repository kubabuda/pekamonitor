#include <Wire.h>    // Include Wire for using I2C
#include <SFE_MicroOLED.h>  // Include the SFE_MicroOLED library
#include "API_connector.h"

//////////////////////////
// MicroOLED Definition //
//////////////////////////
//The library assumes a reset pin is necessary. The Qwiic OLED has RST hard-wired, so pick an arbitrarty IO pin that is not being used
#define PIN_RESET 255  
//The DC_JUMPER is the I2C Address Select jumper. Set to 1 if the jumper is open (Default), or set to 0 if it's closed.
#define DC_JUMPER 0
//////////////////////////////////
// MicroOLED Object Declaration //
//////////////////////////////////
MicroOLED oled(PIN_RESET, DC_JUMPER);    // I2C declaration


void displaySetup() {
  Wire.begin();
  oled.begin();    // Initialize the OLED
  oled.clear(ALL); // Clear the display's internal memory
  oled.clear(PAGE); // Clear the buffer.
  oled.println("");
  oled.println("PEKA monit");
  oled.println("");
  oled.println("start...");
  oled.display();
}

void displaySetupDone() {
  oled.clear(ALL); // Clear the display's internal memory
  oled.clear(PAGE); // Clear the buffer.
  oled.setCursor(0, 0);
  oled.println("");
  oled.println("");
  oled.println("  Setup");
  oled.println("  done.");
  oled.display();
}

void displaySymbol(String symbol) {
    oled.clear(ALL); // Clear the display's internal memory
    oled.clear(PAGE); // Clear the buffer.
    oled.setCursor(0, 0);
    oled.println(symbol);
    oled.display();
    
    Serial.print("Przystanek "); Serial.println(symbol);
    
    // // display monitor header
    // const char* name = response["success"]["bollard"]["name"];
    // const char* symbol = response["success"]["bollard"]["symbol"];
    // Serial.printf("Przystanek %s\n", name);
    // oled.println(symbol);

    // // iterate over times
    // JsonArray times = response["success"]["times"].as<JsonArray>();
    
    // for(JsonVariant v : times) {
    //     const char* line = v["line"];
    //     const char* direction = v["direction"];
    //     int minutes = v["minutes"].as<int>();
    //     bool realTime = v["realTime"].as<bool>();
    
    //     Serial.printf(" - %s w kierunku %s za %d min%s\n", line, direction, minutes,
    //         realTime ? "" : " [wg rozkladu]");
    //     oled.print(line);
    //     oled.print("-");
    //     oled.print(minutes);
    //     oled.println(realTime ? "m" : "m*");
    // }
}

