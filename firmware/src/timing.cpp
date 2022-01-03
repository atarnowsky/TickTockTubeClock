#include "timing.h"

#define USE_INTERNAL_LIB

#ifdef USE_INTERNAL_LIB
    #include "ds3232.h"
#else
    #include <DS3232RTC.h>
#endif

// TODO: Using the RTC library is pretty (memory-) inefficient
// since we are not using calendar functions at all.
// Additionally, querying the current time via I2C from the RTC
// directly every few seconds probably is easier than all this
// implicit syncing mechanisms...

namespace {
    void set_time(int8_t minutes, int8_t hours) {      

        #ifdef USE_INTERNAL_LIB
            DS3232M::set_time({hours, minutes});
        #else
            // We do not use any calendar functions so we
            // can save some bytes of flash by NOT using all these TimeLib method.
            // Let's assume we live in the year 1970:
            time_t t = minutes * 60UL + hours * 3600UL;
            RTC.set(t);
            setTime(t);            
        #endif
    }
}

void RTCSync::initialize() {
    #ifdef USE_INTERNAL_LIB
        if(DS3232M::initialize() == DS3232M::Status::OSCILLATOR_FAILURE)
            DS3232M::set_time({0, 0});        
    #else
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
    #endif    
}

void RTCSync::process() {
    // TODO:: Backup Date and Time to EEPROM and load it on initializtion
}

uint16_t RTCSync::current_time() {
    #ifdef USE_INTERNAL_LIB
        DS3232M::time_t t = DS3232M::time();
        return int16_t(t.hours)*100 + t.minutes;
    #else
        time_t t = now();  
        int8_t minutes = minute(t);
        int16_t hours = hour(t);
        return hours * 100 + minutes;
    #endif    
}


void RTCSync::increment_minutes(int8_t amount) {
    #ifdef USE_INTERNAL_LIB
        DS3232M::time_t t = DS3232M::time();
        int8_t minutes = t.minutes + amount;
        int8_t hours = t.hours;
    #else
        time_t t = now();  
        int8_t minutes = minute(t) + amount;  
        int8_t hours = hour(t);
    #endif
    
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
    #ifdef USE_INTERNAL_LIB
        DS3232M::time_t t = DS3232M::time();
        int8_t minutes = t.minutes;
        int8_t hours = t.hours + amount;
    #else
        time_t t = now();  
        int8_t minutes = minute(t);  
        int8_t hours = hour(t) + amount;
    #endif
    
    // Normalize time
    if(hours > 23)
        hours -= 24;
    
    if(hours < 0)
        hours += 24;

    set_time(minutes, hours);
}