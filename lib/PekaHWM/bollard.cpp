#include "API_connector.h"
#include "PEKA_models.h"


// ************ control flow  ***************** 

volatile bool wasTriggered = false;
volatile unsigned long prevReloadTimestamp = 0;


bool wasManuallyTriggered() {
    return wasTriggered;
}


void setButtonPressed(bool newValue) {
    wasTriggered = newValue;
}


reload_state isReloadNeeded() {
	reload_state result = RELOAD_NOT_NEEDED;
	
	if (wasManuallyTriggered()) {
		result = RELOAD_TRIGGERED;
	} else if (millis() > prevReloadTimestamp + BOLLARD_RELOAD_BREAK)
	{
		result = RELOAD_REFRESH;
	}
	
    if(result) {
        setButtonPressed(false);
    }

    return result;
}


void setLastReloadTimestamp(unsigned long last) {
    prevReloadTimestamp = last;
}


// *****************  bollard symobl get, switch  *****************

static volatile uint32_t prevTriggerTimestamp = 0;
const uint32_t debounce_time_ms = 200;



String getCurrentBollard() {
	return bollards[currentBollard].symbol;
}


void incrementCurrentBollard(ESPRotary& r) {
	auto now = millis();

	// HACK: rotary encoder lib 
	
	if (now > prevTriggerTimestamp + debounce_time_ms) {
		prevTriggerTimestamp = now;
		wasTriggered = true;
		
		++currentBollard;
		currentBollard %= bollardsCount;

		Serial.printf("[%lu] Switching bollard # up to ", now);
		Serial.println(getCurrentBollard());
	}
}


void reloadCurrentBollard() {
	auto now = millis();
	
	if (now > prevTriggerTimestamp + debounce_time_ms) {
		prevTriggerTimestamp = now;
		wasTriggered = true;
		
		Serial.printf("[%lu] Reloading bollard info for ", now);
		Serial.println(getCurrentBollard());
	}	
}


void decrementCurrentBollard(ESPRotary& r) {
	auto now = millis();
	
	if (now > prevTriggerTimestamp + debounce_time_ms) {
		prevTriggerTimestamp = now;
		wasTriggered = true;

		--currentBollard;
		if (currentBollard < 0) {
			currentBollard = bollardsCount - 1;
		}

		Serial.printf("[%lu] Switching bollard # down to ", now);
		Serial.println(getCurrentBollard());
	}
}
