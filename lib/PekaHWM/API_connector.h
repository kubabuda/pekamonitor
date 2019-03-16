#ifndef API_CONNECTOR_H
#define API_CONNECTOR_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESPRotary.h>


const uint8_t fingerprint[20] =  { 0xBA, 0xED, 0xB9, 0xEB, 0xE4, 0x46, 0xD3, 0x16, 0x49, 0x40, 0x34, 0xDC, 0x88, 0x66, 0x76, 0x81, 0x28, 0x68, 0x8B, 0x1D };
const String postEndpoint = "https://www.peka.poznan.pl/vm/method.vm";

const size_t MAX_RESPONSE_SIZE = 4097;

const unsigned long SECOND_MS = 1000;
const unsigned long BOLLARD_RELOAD_BREAK = 60 * SECOND_MS;


// control flow control methods
bool isReloadNeeded();
void setLastReloadTimestamp(unsigned long last);

// API connection methods 
int reloadBollard(String symbol, JsonDocument& response);

// load current bollard from collection, switch it
String getCurrentBollard();
void reloadCurrentBollard();
void incrementCurrentBollard(ESPRotary& r);
void decrementCurrentBollard(ESPRotary& r);

// display methods. TODO pack it to class
void displaySetup();
void displaySetupDone();
void displayResponse(JsonDocument& response);
void displayLoading(String symbol);
void displayLoadingFailed(String symbol);


#endif /* API_CONNECTOR_H */