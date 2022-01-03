#pragma once

#include "scheduler.h"

struct time_pair {
    uint8_t hours = 0;
    uint8_t minutes = 0;
};

// This task allows to access the RTC by increasing/decreasing the
// relative time (no absolute time setting needed for now...)
// Once a second, the time gets read out via I2C, so current_time
// might be queried without introducing additional overhead
class RTCSync : public RelaxedTask<1000> {
 public:
    static void initialize();
    static void process();

    // Returns the current time as a single 4-digit number
    static time_pair current_time();    
    static void increment_minutes(int8_t amount = 1);
    static void increment_hours(int8_t amount = 1);
};


