#include "timing.h"

#include <DS3232RTC.h>

void RTCSync::initialize() {
    bool needs_init = RTC.oscStopped(true);
    setSyncProvider(RTC.get);
    if (timeStatus() != timeSet) {
        // Show error number '0000'
        return;
    }

    if(needs_init) {
        // Show error number '0001'        
        time_t t;
        tmElements_t tm;

        tm.Year = CalendarYrToTm(2021);
        tm.Month = 1;
        tm.Day = 1;
        tm.Hour = 0;
        tm.Minute = 0;
        tm.Second = 0;

        t = makeTime(tm);
        RTC.set(t);
        setTime(t);
    }  

    setSyncInterval(10);
}

void RTCSync::process() {
    // TODO:: Backup Date and Time to EEPROM and load it on initializtion
}

uint16_t RTCSync::current_time() {
    time_t t = now();  
    int minutes = minute(t);
    int hours = hour(t);
    return hours * 100 + minutes;
}


void RTCSync::increment_minutes(int8_t amount) {
    time_t t = now();  
    int minutes = minute(t) + amount;  
    int hours = hour(t);
    
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

    tmElements_t tm;

    tm.Year = CalendarYrToTm(2021);
    tm.Month = 1;
    tm.Day = 1;
    tm.Hour = hours;
    tm.Minute = minutes;
    tm.Second = 0;
    t = makeTime(tm);
    RTC.set(t);
    setTime(t);
}

void RTCSync::increment_hours(int8_t amount) {
    time_t t = now();  
    int minutes = minute(t);  
    int hours = hour(t) + amount;  
    
    // Normalize time
    if(hours > 23)
        hours -= 24;
    
    if(hours < 0)
        hours += 24;

    tmElements_t tm;

    tm.Year = CalendarYrToTm(2021);
    tm.Month = 1;
    tm.Day = 1;
    tm.Hour = hours;
    tm.Minute = minutes;
    tm.Second = 0;
    t = makeTime(tm);
    RTC.set(t);
    setTime(t);
}