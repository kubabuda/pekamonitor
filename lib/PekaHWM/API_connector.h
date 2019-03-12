#ifndef API_CONNECTOR_H
#define API_CONNECTOR_H

#include <Arduino.h>
#include <ArduinoJson.h>


const uint8_t fingerprint[20] =  { 0xBA, 0xED, 0xB9, 0xEB, 0xE4, 0x46, 0xD3, 0x16, 0x49, 0x40, 0x34, 0xDC, 0x88, 0x66, 0x76, 0x81, 0x28, 0x68, 0x8B, 0x1D };
const String postEndpoint = "https://www.peka.poznan.pl/vm/method.vm";

const size_t MAX_RESPONSE_SIZE = 4097;


int connect(String symbol, StaticJsonDocument<MAX_RESPONSE_SIZE>* responsePtr);

void displaySetup();
void displaySetupDone();
void displaySymbol(String symbol);
void displayResponse(StaticJsonDocument<MAX_RESPONSE_SIZE>* responsePtr);

#endif /* API_CONNECTOR_H */