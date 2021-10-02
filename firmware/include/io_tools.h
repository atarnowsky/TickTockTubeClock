#pragma once 

#include <Arduino.h>
#include "etl_fix.h"


using pin_t = uint8_t;

enum class Port {
    C,
    D
};

struct ppin_t {
    Port  port;
    pin_t pin_mask;
    pin_t arduino_pin; // This is only temporary, since proper pin configuration is implemented
};


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

    template<int MODE>
    static inline void configurePin(ppin_t pin) 
    { 
        ::pinMode(pin.arduino_pin, MODE); 
    }

 public:
    template<int MODE, typename... PINS>
    static inline void configure(PINS const&... args) {
        return (configurePin<MODE>(args), ...);
    }

    // PORTD -> Shift registers
    // PORTC / PC0 -> Light / PC1 -> Speaker
    
    // FIXME: Automatically choose correct port
    struct PortD {
        __attribute__((always_inline))
        static inline void high(const ppin_t& pin){
        PORTD = PORTD | pin.pin_mask;
        }
        
        __attribute__((always_inline))
        static inline void low(const ppin_t& pin){
        PORTD = PORTD & (~pin.pin_mask);
        }
        
        __attribute__((always_inline))
        static inline void pulse(const ppin_t& pin){
        PORTD = PORTD | pin.pin_mask;
        PORTD = PORTD & (~pin.pin_mask);
        }
    };

    struct PortC {
        __attribute__((always_inline))
        static inline void high(const ppin_t& pin){
        PORTC = PORTC | pin.pin_mask;
        }
        
        __attribute__((always_inline))
        static inline void low(const ppin_t& pin){
        PORTC = PORTC & (~pin.pin_mask);
        }
        
        __attribute__((always_inline))
        static inline void pulse(const ppin_t& pin){
        PORTC = PORTC | pin.pin_mask;
        PORTC = PORTC & (~pin.pin_mask);
        }
    };
};

