
#pragma once

#include <Arduino.h>
#include <stdint.h>
#include <math.h>
#include "hw_map.h"
#include "scheduler.h"

#include "etl_fix.h"

#include <digitalWriteFast.h>

namespace Display {
    constexpr ppin_t SHIFT_LATCH_DATA_D = {Port::D, Pins::Shift::Data.pin_mask | Pins::Shift::Latch.pin_mask, 0};
    constexpr ppin_t SHIFT_MUX_BOTH_D = {Port::D, Pins::Anode::MuxA.pin_mask | Pins::Anode::MuxB.pin_mask, 0};

    // There are a total of four buffer used, two for showing numbers (MUXA and MUXB)
    // the other two for dots only. Both are shown in alternating order.
    // This effectively cuts the maximum brightness in half, but
    // the blank frame between numbers reduces ghosting a lot!
    // Additionally, putting the dots into the same buffer as the numbers go
    // would result into a slight but noticable change in the according
    // numbers brightness due to the common anode.
    // Displaying numbers and dots in alternating order assures both get the
    // same amount of current.
    constexpr uint8_t register_count = 3;
    static volatile uint8_t register_buffers[4][register_count] = 
    {
        //     BANK0       BANK1       BANK2
        { 0b00000000, 0b00000000, 0b00000000}, // Ones
        { 0b00000000, 0b00000000, 0b00000000}, // Blank / Ones Dot
        { 0b00000000, 0b00000000, 0b00000000}, // Tens
        { 0b00000000, 0b00000000, 0b00000000}  // Tens Blank / Tens Dot
    };

        
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
        static void initialize(uint16_t update_rate) {
            blank(); // Clear registers to assure no random numbers show up on startup
        } 

        static void process(uint16_t cycle_count) {
            (void) cycle_count; // TODO: Use calls instead of cnt
            static uint8_t cnt = 0;    
            if(cnt < 8) 
                show_ones(0);
            else if (cnt < 16)
                show_ones(1);
            else if(cnt < 24)
                show_tens(2);
            else 
                show_tens(3);
            //if(cnt > brightness) 
            //  blank();    
            //else if(cnt >= (brightness>>1))     
            //  show_tens();
            //else
            //  show_ones();
            ++cnt &= 0b00011111;
        }

     private:
        static void blank()
        {
            IO::PortD::low(SHIFT_LATCH_DATA_D);
            for (uint8_t i = 0; i < 24; i++)
            {
                IO::PortD::pulse(Pins::Shift::Clock);
            }
            IO::PortD::high(Pins::Shift::Latch);
            IO::PortD::low(SHIFT_MUX_BOTH_D);
        }
        
        static void clear() {
            for(uint8_t i = 0; i < register_count; i++)
                for(uint8_t j = 0; j < 4; j++)
                    register_buffers[j][i] = 0b00000000;
            blank();
        }
        
        static void show_ones(uint8_t index)
        {
            IO::PortD::low(Pins::Shift::Latch);   
            Pins::Shift::shift_out(register_buffers[index][0]);
            Pins::Shift::shift_out(register_buffers[index][1]);
            Pins::Shift::shift_out(register_buffers[index][2]);    
            IO::PortD::low(Pins::Anode::MuxA);
            IO::PortD::high(Pins::Anode::MuxB);    
            IO::PortD::high(Pins::Shift::Latch); 
        }

        static void show_tens(uint8_t index) 
        {
            IO::PortD::low(Pins::Shift::Latch);   
            Pins::Shift::shift_out(register_buffers[index][0]);
            Pins::Shift::shift_out(register_buffers[index][1]);
            Pins::Shift::shift_out(register_buffers[index][2]);    
            IO::PortD::high(Pins::Anode::MuxA);
            IO::PortD::low(Pins::Anode::MuxB);    
            IO::PortD::high(Pins::Shift::Latch); 
        }                 
    };

    class BufferControl {
     public:
        static void show_time(uint16_t value){
            auto number = [](uint16_t n) -> array<uint8_t, 4> 
            {
                if(n > 9999) return {255, 255, 255, 255};
                array<uint8_t, 4>  result;
                result[3] = (n / 1000) % 10;
                result[2] = (n / 100) % 10;
                result[1] = (n / 10) % 10;
                result[0] = n % 10;
                return result;
            };

            set_numbers(number(value));  
        }

        static void show_number(uint16_t value) {
            auto number = [](uint16_t n) -> array<uint8_t, 4> 
            {
                if(n > 9999) return {255, 255, 255, 255};
                array<uint8_t, 4>  result;
                result[3] = (n / 1000) % 10;
                result[2] = (n / 100) % 10;
                result[1] = (n / 10) % 10;
                result[0] = n % 10;
                return result;
            };

            set_numbers(number(value));  
            set_dots({false, false, false, false});  
        }

        static void show_number(float value) {
            auto number = [](uint16_t n) -> array<uint8_t, 4> 
            {
                if(n > 9999) return {255, 255, 255, 255};
                array<uint8_t, 4>  result;
                result[3] = (n / 1000) % 10;
                result[2] = (n / 100) % 10;
                result[1] = (n / 10) % 10;
                result[0] = n % 10;
                return result;
            };

            // TODO: There is definitely a more elegant way to achieve this!
            auto decimals = [&number](float v) -> array<uint8_t, 4> 
            {
                if(v > 999.9)
                    return number(lroundf(v));
                if(v > 99.99)
                    return number(lroundf(v*10.0f));
                if(v > 9.999)
                    return number(lroundf(v*100.0f));
                return number(lroundf(v*1000.0f));                
            };

            // TODO: There is definitely a more elegant way to achieve this!
            auto dots = [](float v) -> array<bool, 4> 
            {
                if(v > 999.9)
                    return {true, false, false, false};
                if(v > 99.99)
                    return {false, true, false, false};
                if(v > 9.999)
                    return {false, false, true, false};
                return {false, false, false, true};
            };

            set_numbers(decimals(value));  
            set_dots(dots(value));  
        }
        
        static void show_dots(const array<bool, 4>& dots = {false, false, false, false})
        {  
            set_dots(dots);
        }

        static void clear()
        {  
            set_numbers({255, 255, 255, 255});  
            set_dots({false, false, false, false});
        }
        
     private:          
        static void set_numbers(const array<uint8_t, 4>& numbers) {
            // Clear number buffer            
            for(uint8_t i = 0; i < register_count; i++) {
                register_buffers[0][i] = 0b00000000;
                register_buffers[2][i] = 0b00000000;
            }

            for(uint8_t d = 0; d < numbers.size(); d++)
            {
                const uint8_t number = numbers[d];    
                const uint8_t deci = d % 2; // 0 -> minutes/hours, 1 -> deciminutes/decihours
                const uint8_t lr = d / 2;   // 0 -> right block,   1 -> left block
                volatile auto& buf = register_buffers[deci*2];
                // Every number > 9 will be interpreted as blank
                if(number < 10)
                    buf[number_mapping[number][lr][0]] |= (1 << number_mapping[number][lr][1]);
            }
        }

        static void set_dots(const array<bool, 4>& dots)
        {  
            // Clear dot buffer
            for(uint8_t i = 0; i < register_count; i++) {
                register_buffers[1][i] = 0b00000000;
                register_buffers[3][i] = 0b00000000;
            }

            for(uint8_t d = 0; d < dots.size(); d++)
            {
                const bool dot = dots[d];
                const uint8_t deci = d % 2; // 0 -> minutes/hours, 1 -> deciminutes/decihours
                const uint8_t lr = d / 2;   // 0 -> right block,   1 -> left block
                volatile auto& buf = register_buffers[deci*2 + 1];
                // Place dot if appropriate
                if(dot)
                    buf[dot_mapping[lr][0]] |= (1 << dot_mapping[lr][1]);
            }
        }
    };

    class SeparatorDot : public RelaxedTask<1000> {
     public:
        static void initialize() {

        }

        static void process() {
            static bool separator_visible = false;
            separator_visible = !separator_visible;        
            BufferControl::show_dots({false, separator_visible, false, false});                
        }
    };


    // __attribute__((always_inline))
    // static inline void ledHigh(){
    // //PORTC = PORTC | PC0;
    // digitalWriteFast(PIN_PC0, HIGH);
    // }            

    // __attribute__((always_inline))
    // static inline void ledLow(){
    // //PORTC = PORTC & (~PC0);
    // digitalWriteFast(PIN_PC0, LOW);
    // }            


}