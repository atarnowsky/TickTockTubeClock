#include "timing.h"

#include <DS3232RTC.h>

// TODO: Using the RTC library is pretty (memory-) inefficient
// since we are not using calendar functions at all.
// Additionally, querying the current time via I2C from the RTC
// directly every few seconds probably is easier than all this
// implicit syncing mechanisms...

namespace {
    void set_time(int8_t minutes, int8_t hours) {
        // We do not use any calendar functions so we
        // can save some bytes of flash by NOT using all these TimeLib method.
        // Let's assume we live in the year 1970:
        time_t t = minutes * 60UL + hours * 3600UL;
        RTC.set(t);
        setTime(t);
    }
}

void RTCSync::initialize() {
    bool needs_init = RTC.oscStopped(true);
    setSyncProvider(RTC.get);
    if (timeStatus() != timeSet) {
        // Show error number '0000'
        return;
    }

    if(needs_init) {   
        set_time(0, 0);
    }  

    setSyncInterval(10);

    // Disable unused RTC features
    RTC.squareWave(SQWAVE_NONE);
}

void RTCSync::process() {
    // TODO:: Backup Date and Time to EEPROM and load it on initializtion
}

uint16_t RTCSync::current_time() {
    time_t t = now();  
    int8_t minutes = minute(t);
    int16_t hours = hour(t);
    return hours * 100 + minutes;
}


void RTCSync::increment_minutes(int8_t amount) {
    time_t t = now();  
    int8_t minutes = minute(t) + amount;  
    int8_t hours = hour(t);
    
    // Normalize time
    if(minutes > 59) {
        minutes -= 60;
        hours = hours + 1;
    }

    if(minutes < 0) {
        minutes += 60;
        hours = hours - 1;
    }

    if(hours > 23)
        hours -= 24;
    
    if(hours < 0)
        hours += 24;

    set_time(minutes, hours);
}

void RTCSync::increment_hours(int8_t amount) {
    time_t t = now();  
    int8_t minutes = minute(t);  
    int8_t hours = hour(t) + amount;  
    
    // Normalize time
    if(hours > 23)
        hours -= 24;
    
    if(hours < 0)
        hours += 24;

    set_time(minutes, hours);
}