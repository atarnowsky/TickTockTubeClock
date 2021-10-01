
#include <Arduino.h>
#include <stdint.h>
#include <math.h>
#include "hw_map.h"

// Someone forgot to include Atmega168P...
#define TIMER1_A_PIN   9
#define TIMER1_B_PIN   10
#define TIMER1_ICP_PIN 8
#define TIMER1_CLK_PIN 5
#include <TimerOne.h>

#include "etl_fix.h"

#include <digitalWriteFast.h>

// TODO: Integrate this trickery into io_tools
constexpr pin_t SHIFT_CLOCK_D = (1 << Pins::Shift::Clock);
constexpr pin_t SHIFT_DATA_D = (1 << Pins::Shift::Data);
constexpr pin_t SHIFT_LATCH_D = (1 << Pins::Shift::Latch);
constexpr pin_t SHIFT_MUX_A_D = (1 << Pins::Anode::MuxA);
constexpr pin_t SHIFT_MUX_B_D = (1 << Pins::Anode::MuxB);

constexpr pin_t SHIFT_LATCH_DATA_D = SHIFT_DATA_D | SHIFT_LATCH_D;
constexpr pin_t SHIFT_MUX_BOTH_D = SHIFT_MUX_A_D | SHIFT_MUX_B_D;

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

class Display {

public:
    Display() = default;

    void initialize() {
        blank();
        ledLow();

        Timer1.initialize(250); // = 1ms / 100Hz
        Timer1.attachInterrupt(isr_display); // blinkLED to run every 0.15 seconds
    }

    void process() {
        long diff = millis() - last_tick ;

        if(diff >= 1000) {
            time_dot = !time_dot;                        
            last_tick = millis();
        }        
    }

    void clear() {
        for(uint8_t i = 0; i < register_count; i++)
            for(uint8_t j = 0; j < 4; j++)
                register_buffers[j][i] = 0b00000000;
        blank();
    }

    void time(uint16_t value){
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

        set_registers(number(value), {false, false, time_dot, false});  
        time_mode = true;
    }

    void number(uint16_t value) {
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

        set_registers(number(value), {false, false, false, false});  
        time_mode = false;
    }

    void number(float value) {
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

        set_registers(decimals(value), dots(value));  
        time_mode = false;
    }

    void led_on(uint8_t brightness = 255) {
        ledHigh();
    }

    void led_off() {
        ledLow();
    }


private:
    volatile uint8_t brightness = 16; 
    bool time_mode = false;
    bool time_dot = false;
    long last_tick = 0;
    

    static void isr_display()
    {
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

    __attribute__((always_inline))
    static inline void ledHigh(){
    //PORTC = PORTC | PC0;
    digitalWriteFast(PIN_PC0, HIGH);
    }            

    __attribute__((always_inline))
    static inline void ledLow(){
    //PORTC = PORTC & (~PC0);
    digitalWriteFast(PIN_PC0, LOW);
    }            

    // Only works with Port D!
    __attribute__((always_inline))
    static inline void ioHigh(uint8_t pin){
    PORTD = PORTD | pin;
    }

    __attribute__((always_inline))
    static inline void ioLow(uint8_t pin){
    PORTD = PORTD & (~pin);
    }

    __attribute__((always_inline))
    static inline void ioPulse(uint8_t pin){
    PORTD = PORTD | pin;
    PORTD = PORTD & (~pin);
    }

    __attribute__((always_inline))
    __attribute__((optimize("unroll-loops")))
    static inline void shiftOutFaster(uint8_t data){  
    for(uint8_t i = 0; i < 8; i++){   
        ioLow(SHIFT_CLOCK_D);
        if((data & 0b00000001))
            ioHigh(SHIFT_DATA_D);
        else
            ioLow(SHIFT_DATA_D);
        ioHigh(SHIFT_CLOCK_D);
        //ioPulse(SHIFT_CLOCK_D);
        data >>= 1;
    }
    }

    static void show_ones(uint8_t index)
    {
        ioLow(SHIFT_LATCH_D);   
        shiftOutFaster(register_buffers[index][0]);
        shiftOutFaster(register_buffers[index][1]);
        shiftOutFaster(register_buffers[index][2]);    
        ioLow(SHIFT_MUX_A_D);
        ioHigh(SHIFT_MUX_B_D);    
        ioHigh(SHIFT_LATCH_D); 
    }

    static void show_tens(uint8_t index) 
    {
        ioLow(SHIFT_LATCH_D);   
        shiftOutFaster(register_buffers[index][0]);
        shiftOutFaster(register_buffers[index][1]);
        shiftOutFaster(register_buffers[index][2]);    
        ioHigh(SHIFT_MUX_A_D);
        ioLow(SHIFT_MUX_B_D);    
        ioHigh(SHIFT_LATCH_D); 
    }

    static void blank()
    {
        ioLow(SHIFT_LATCH_DATA_D);
        for (uint8_t i = 0; i < 24; i++)
        {
            ioPulse(SHIFT_CLOCK_D);
        }

        //ioPulse(SHIFT_LATCH_D);  
        ioHigh(SHIFT_LATCH_D);
        ioLow(SHIFT_MUX_BOTH_D);
    }

        
    static void set_registers(const array<uint8_t, 4>& numbers, const array<bool, 4>& dots)
    {  
        // Clear complete buffer
        for(uint8_t i = 0; i < register_count; i++)
            for(uint8_t j = 0; j < 4; j++)
            register_buffers[j][i] = 0b00000000;

        for(uint8_t d = 0; d < numbers.size(); d++)
        {
            const uint8_t number = numbers[d];
           
            
            const uint8_t deci = d % 2; // 0 -> minutes/hours, 1 -> deciminutes/decihours
            const uint8_t lr = d / 2;   // 0 -> right block,   1 -> left block

            volatile auto& buf = register_buffers[deci*2];
            // Every number > 9 will be interpreted as blank
            if(number > 9) continue;

            buf[number_mapping[number][lr][0]] |= (1 << number_mapping[number][lr][1]);
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

    
    static void set_dots(const array<bool, 4>& dots)
    {  
        // Clear dot buffer
        for(uint8_t i = 1; i < register_count; i+=2)
            for(uint8_t j = 0; j < 4; j++)
            register_buffers[j][i] = 0b00000000;

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