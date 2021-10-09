
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
        static void process(uint16_t cycle_count);

     private:
        static void blank();        
        static void clear();        
        static void show_ones(uint8_t index);
        static void show_tens(uint8_t index);    
        static void show_nothing();
    };

    class BufferControl {
     public:
        static void show_time(uint16_t value);
        static void show_number(uint16_t value);
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
}