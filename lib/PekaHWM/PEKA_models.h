#ifndef PEKA_MODELS_H
#define PEKA_MODELS_H

#include <Arduino.h>

// REST API models

struct PEKA_bollard {
  char* symbol;
  char* tag;
  char* name;
  boolean* mainBollard;
} typedef PEKA_bollard;

struct PEKA_times {
    int minutes;
    boolean realTime;
    char* line;
    char* direction;
    boolean onStopPoint;
    char* departure;
} typedef PEKA_times;

struct PEKA_success {
    PEKA_bollard bollard;
    PEKA_times times[10];
} typedef PEKA_success;

struct PEKA_response {
    PEKA_success success;
} typedef PEKA_response;


// model for bollards to display

struct bollard_request_t {
  String symbol;
} typedef bollard_request_t;

// collection of boards to display
const int bollardsCount = 4; // TODO rename to bollardMaxCount and implement

bollard_request_t bollards[bollardsCount] = {
  { .symbol = "SWRZ01", },
  { .symbol = "SWRZ02", },
  { .symbol = "RYCE04", },
  { .symbol = "RKAP71", },
};

#endif /* PEKA_MODELS_H */