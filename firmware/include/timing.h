#pragma once

#include <Wire.h>
#include <DS3232RTC.h>

#include "scheduler.h"
#include "hw_map.h"

// Missing features:
//  - Proper integration with scheduler
//  - 12/24 hour mode
//  - Alarm timer
//  - Error display
//  - Calendar support
//  - Periodic EEPROM backup for calendar to reduce user effort on power loss

class RTCSync : public RelaxedTask<UINT_MAX> {
 public:
    static void initialize() {
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

    static void process() {

    }
    
    static uint16_t current_time() {
        time_t t = now();  
        int minutes = minute(t);
        int hours = hour(t);
        return hours * 100 + minutes;
    }

    
    static void increment_minutes(int8_t amount = 1) {
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

    static void increment_hours(int8_t amount = 1) {
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
};


