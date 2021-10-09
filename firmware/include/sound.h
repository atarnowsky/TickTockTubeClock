
#pragma once

#include "scheduler.h"


enum class TickTockSound : uint8_t {
    None = 0,
    ClickSilent = 1,
    ClickMedium = 2,
    ClickLoud = 3,
    LowBeep = 4, 
    HighBeep = 5, 
};

// enum class AlarmSound : uint8_t {
//     SingleLow = 0,
//     SingleHigh = 1,
//     DoubleLow = 2,
//     DoubleHigh = 3,
//     TripleLow = 4,
//     TripleHigh = 5    
// };

// TODO: Use a non-blocking tone function and split class up into SoundGenerator and SoundManager
class SoundGenerator : public RelaxedTask<1000> {
 public:
    static void initialize();
    static void process();
    static void set_tick_tock(TickTockSound v);    

    // TODO: Not implemented yet
    // static void alarm(AlarmSound v);
    // static void alarm();
    // static void set_alarm_sound(AlarmSound v);

    static void error(uint8_t bits);
    static void ack(uint8_t times = 1);

 private:
    static constexpr uint16_t speaker_resonant_frequency = 4000; // [Hz]
};

