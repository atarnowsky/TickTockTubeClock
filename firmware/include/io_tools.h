#pragma once 

#include <Arduino.h>
#include "etl_fix.h"


using pin_t = uint8_t;

struct IO {

 private:    
    template<int MODE>
    static inline void configurePin(pin_t pin) 
    { 
        ::pinMode(pin, MODE); 
    }

    template<int MODE, unsigned int COUNT>
    static inline void configurePin(const array<pin_t, COUNT>& pins) 
    { 
        for(pin_t pin : pins)
            ::pinMode(pin, MODE); 
    }

 public:
    template<int MODE, typename... PINS>
    static inline void configure(PINS const&... args) {
        return (configurePin<MODE>(args), ...);
    }

    // PORTD -> Shift registers
    // PORTC / PC0 -> Light
        
    struct PortD {
        __attribute__((always_inline))
        static inline void high(pin_t pin){
        PORTD = PORTD | pin;
        }
        
        __attribute__((always_inline))
        static inline void low(pin_t pin){
        PORTD = PORTD & (~pin);
        }
        
        __attribute__((always_inline))
        static inline void pulse(pin_t pin){
        PORTD = PORTD | pin;
        PORTD = PORTD & (~pin);
        }
    };

    struct sn74hc595 {        
        template<unsigned char PORT_CLOCK,
                 unsigned char PORT_DATA>
        __attribute__((always_inline))
        __attribute__((optimize("unroll-loops")))
        static inline void shift_out(uint8_t data){  
            for(uint8_t i = 0; i < 8; i++){   
                PortD::low(PORT_CLOCK);
                if((data & 0b00000001))
                    PortD::high(PORT_DATA);
                else
                    PortD::low(PORT_DATA);
                PortD::high(PORT_CLOCK);
                data >>= 1;
            }
        }
    };
};

