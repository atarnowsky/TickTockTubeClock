#pragma once

#include "scheduler.h"

// Missing features:
//  - Proper integration with scheduler
//  - 12/24 hour mode
//  - Alarm timer
//  - Error display
//  - Calendar support
//  - Periodic EEPROM backup for calendar to reduce user effort on power loss

// Safe state to EEPROM 8 times a day. This interval
// should be save with respect to write cycles
// (Should be about ~34 years without EEPROM failure)
constexpr uint32_t every_three_hours = 3UL * (60 * 60) * 1000;

class RTCSync : public RelaxedTask<every_three_hours> {
 public:
    static void initialize();
    static void process();

    static uint16_t current_time();    
    static void increment_minutes(int8_t amount = 1);
    static void increment_hours(int8_t amount = 1);
};


