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
  String name;
} typedef bollard_request_t;

// collection of boards to display
const int bollardsCount = 2;

bollard_request_t bollards[bollardsCount + 1] = {
  { .symbol = "SWRZ01", .name = "Świerzawska->Ławica" },
  { .symbol = "SWRZ02", .name = "Świerzawska->Kaponiera" },
  { .symbol = "RYCE04", .name = "Rycerska->Kaponiera" },
};

volatile int currentBollard = 0;
