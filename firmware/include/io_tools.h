#pragma once 

#include <Arduino.h>
#include "etl_fix.h"

struct IO {

 private:    
    template<int MODE>
    static inline void configurePin(uint8_t pin) 
    { 
        ::pinMode(pin, MODE); 
    }

    template<int MODE, unsigned int COUNT>
    static inline void configurePin(const array<uint8_t, COUNT>& pins) 
    { 
        for(uint8_t pin : pins)
            ::pinMode(pin, MODE); 
    }

 public:
    template<int MODE, typename... PINS>
    static inline void configure(PINS const&... args) {
        return (configurePin<MODE>(args), ...);
    }

};