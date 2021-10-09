
#pragma once

#include "scheduler.h"
#include "hw_map.h"

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
    static void initialize() {
        
    }

    static void process(){
        auto short_click = [](uint8_t length) {
            noInterrupts();
            for(uint8_t i = 0; i < length; i++)
                IO::high(Pins::Speaker);
            IO::low(Pins::Speaker);
            interrupts();  
        };

        auto short_beep = [](uint8_t length = 50, int16_t freq = 320) {
            tone(Pins::Speaker.arduino_pin(), freq, length);
        };

        switch (tick_tock) {
            case TickTockSound::None:
                break;
            
            case TickTockSound::ClickSilent:
                short_click(16); 
                break;

            case TickTockSound::ClickMedium:
                short_click(32); 
                break;

            case TickTockSound::ClickLoud:
                short_click(64); 
                break;

            case TickTockSound::LowBeep:
                short_beep(20, speaker_resonant_frequency >> 4);
                break;                

            case TickTockSound::HighBeep:
                short_beep(20, speaker_resonant_frequency);
                break;                
        }
    }

    static void set_tick_tock(TickTockSound v) 
    {
        tick_tock = v;
    }
    
    // static void alarm(AlarmSound v) {
    //     // TODO: Not implemented yet
    // }

    // static void alarm() {
    //     alarm(alarm_sound);
    // }   

    // static void set_alarm_sound(AlarmSound v) 
    // {
    //     alarm_sound = v;
    // }

    static void error(uint8_t bits)
    {        
        for(int i = 0; i < 4; i++)
        {            
            tone(Pins::Speaker.arduino_pin(), ((bits & (1 << i)) > 0 ? 800 : 400), 250);            
            delay(500);
        }
        delay(500);
    }

    static void ack(uint8_t times = 1){
        for(uint8_t i = 0; i < times; i++)
        {
            tone(Pins::Speaker.arduino_pin(), 4000, 50);
            delay(100);
        }
    }

 private:
    static constexpr uint16_t speaker_resonant_frequency = 4000; // [Hz]
    static TickTockSound tick_tock;
    //static AlarmSound alarm_sound;    
};

TickTockSound SoundGenerator::tick_tock = TickTockSound::None;
//AlarmSound SoundGenerator::alarm_sound = AlarmSound::DoubleHigh;
