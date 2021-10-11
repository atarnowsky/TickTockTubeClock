
#pragma once

#include "scheduler.h"
#include "etl_fix.h"


namespace Display {
      
    constexpr static uint8_t number_mapping [10][2][2] = 
    {
        // Right    Left
        { {1, 6}, {2, 2} }, // Number: 0
        { {0, 4}, {2, 1} }, // Number: 1
        { {1, 4}, {1, 1} }, // Number: 2
        { {0, 1}, {2, 6} }, // Number: 3
        { {1, 3}, {2, 4} }, // Number: 4
        { {0, 5}, {2, 0} }, // Number: 5
        { {0, 6}, {1, 0} }, // Number: 6
        { {1, 5}, {2, 5} }, // Number: 7
        { {0, 2}, {1, 2} }, // Number: 8
        { {0, 3}, {2, 3} }  // Number: 9 
    };
    
    constexpr static uint8_t dot_mapping [2][2] =  {
    // Right    Left
        {0, 0}, {2,7}    // Dot
    };

    class ShiftPWMProcessor : public CriticalTask<32>  {
     public:
        static void initialize(uint16_t update_rate);
        static void process(uint8_t cycle_count);

        enum class MUXMask {
            NONE, 
            ONES,
            TENS,
            BOTH
        };

        static void configure_mux(MUXMask mask);
        
        static void set_brightness(uint8_t target);
        static void set_fade_target(uint8_t target);
        static void set_fade_speed(uint8_t speed);

        static volatile uint8_t& alter_buffer(uint8_t bank, uint8_t number);

     private:
        static void blank();        
        static void clear();        
        static void show_ones(uint8_t index);
        static void show_tens(uint8_t index);    
        static void show_nothing();

        static constexpr uint8_t slowmotion_factor = 8;
    };

    class BufferControl {
     public:
        static void show_time(uint16_t value);
        static void show_number(uint16_t value);
        static void show_direct(const array<uint8_t, 4>& numbers, const array<bool, 4>& dots);
        static void show_number(float value);
        static void show_dots(const array<bool, 4>& dots = {false, false, false, false});
        static void clear();
        
     private:          
        static void set_numbers(const array<uint8_t, 4>& numbers);
        static void set_dots(const array<bool, 4>& dots);
    };

    class SeparatorDot : public RelaxedTask<1000> {
     public:
        static void initialize();
        static void process();
    };


    // === AntiCathodePoisoning (ACP) ===

    // Tries to reduce cathode poisoning effects by cycling through
    // all available numbers (often referred to as 'slot machine' effect).
    // During these cycles, muxing is deactivated in order to allow
    // the tubes to reach their full brightness.
    // See [1] for more information wrt. cathode poisoning.

    // There are many discussions on how effective this procedure is
    // and how often it should be performed. It is also not clear how
    // long each number should be lightened up to be effective.
    // You may lower the 'acp_duration' (defaults to 10ms for now) 
    // to make the effect less visually distracting.

    // According to [2] (at least for a specific type of tube), the ratio
    // between normal operation and cycling should be about 60:0.2 meaning
    // if the clock runs for 24 hours, the refreshing phase should be about 
    // 5 minutes FOR EACH DIGIT. Since we have to cycle between two individual
    // pairs of tubes and cannot show the digits simultaneously, this adds
    // up to a total of 110 minutes per day.
    // This value seems to be a bit high, since we do not drive the tubes
    // at their maximum brightness. Probably 30 minutes a day should be
    // sufficient (???).

    // For the CD81 tubes that are used in revA however, there is
    // a report [3] that even without poisoning prevention this specific 
    // type of tube may last for several years. So it is up to you to decide
    // whether to enable ACP or not :-)

    // References:
    // [1]  http://www.tube-tester.com/sites/nixie/different/cathode%20poisoning/cathode-poisoning.htm
    // [2]  https://docs.daliborfarny.com/documentation/cathode-poisoning-prevention-routine/
    // [3]  https://www.youtube.com/watch?v=RwDEhyfa7OI - see comments section
    
    constexpr uint16_t acp_duration = 10; // [ms]    
    class AntiCathodePoisoning : public RelaxedTask<acp_duration> {
     public:
        static void initialize();
        static void process();
        static void enable();
        static void disable();

     private:
        // The stacking order of the tubes individual digits - here specific for the CD81
        // This array is used to add some kind of '3D' effect, cycling the digits
        // from back to front.
        static constexpr uint8_t cycle_order[14] = {0, 1, 9, 5, 4, 3, 2, 6, 7, 8, 255, 255, 255, 255};
    };
}