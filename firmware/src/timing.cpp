#include "timing.h"
#include "ds3232.h"

time_pair last_query;

void time_pair::normalize() {
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
}

void RTCSync::initialize() {
    if(DS3232M::initialize() == DS3232M::Status::OSCILLATOR_FAILURE)
        DS3232M::set_time({0, 0});     
}

void RTCSync::process() {
    last_query = DS3232M::time();    
}

time_pair RTCSync::current_time() {
    return last_query;   
}

void RTCSync::increment_minutes(int8_t amount) {
    time_pair t = DS3232M::time();
    t.minutes += amount;
    t.normalize();
    DS3232M::set_time(t);
    process(); // Actualize internal timer
}

void RTCSync::increment_hours(int8_t amount) {   
    time_pair t = DS3232M::time();
    t.hours += amount;
    t.normalize();
    DS3232M::set_time(t);
    process(); // Actualize internal timer
}