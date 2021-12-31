#pragma once 

#include "etl_fix.h"
#include <Arduino.h>

using pin_t = uint8_t;

enum class Port {
    C,
    D
};

struct ppin_t;
constexpr inline ppin_t operator "" _C(unsigned long long);
constexpr inline ppin_t operator "" _D(unsigned long long);

struct ppin_t {
    const Port  port;
    const uint8_t pin_mask;

    constexpr inline bool operator ==(const ppin_t& rhs) const {
        return (this->port == rhs.port) && (this->pin_mask == rhs.pin_mask);
    }

    inline pin_t arduino_pin() const {
        // TODO: This is just a fallback for specific pins that rely on
        // arduino calls. Better to remove this dependency instead of
        // enlarging this method...

        if (*this == 1_C) {
            return PIN_PC1;
        }

        return 255; // Error
    }
};

constexpr inline ppin_t operator "" _C(unsigned long long pin) {
    return ppin_t {
        .port = Port::C,
        .pin_mask = uint8_t(1 << uint8_t(pin))
    };
}

constexpr inline ppin_t operator "" _D(unsigned long long pin) {
    return ppin_t {
        .port = Port::D,
        .pin_mask = uint8_t(1 << uint8_t(pin))
    };
}

struct IO {

 private:    
    template<int MODE>
    static inline void configure_pin(pin_t pin) 
    { 
        ::pinMode(pin, MODE); 
    }

    template<int MODE, unsigned int COUNT>
    static inline void configure_pin(const array<pin_t, COUNT>& pins) 
    { 
        for(pin_t pin : pins)
            ::pinMode(pin, MODE); 
    }

    template<int MODE>
    static inline void configure_pin(ppin_t pin) 
    { 
        if constexpr (MODE == OUTPUT) {
            if (pin.port == Port::C) {
                DDRC = DDRC | pin.pin_mask;
            }
            else if (pin.port == Port::D) {
                DDRD = DDRD | pin.pin_mask;
            }
        } else {
            // Configure as input
            if (pin.port == Port::C) {
                DDRC = DDRC & (~pin.pin_mask);
            }
            else if (pin.port == Port::D) {
                DDRD = DDRD & (~pin.pin_mask);
            }

            if constexpr (MODE == INPUT_PULLUP) {
                if (pin.port == Port::C) {
                    PORTC = PORTC | pin.pin_mask;
                }
                else if (pin.port == Port::D) {
                    PORTD = PORTD | pin.pin_mask;
                }
            } else {
                if (pin.port == Port::C) {
                    PORTC = PORTC & (~pin.pin_mask);
                }
                else if (pin.port == Port::D) {
                    PORTD = PORTD & (~pin.pin_mask);
                }
            }
        }
    }

 public:
    template<int MODE, typename... PINS>
    static inline void configure(PINS const&... args) {
        return (configure_pin<MODE>(args), ...);
    }
    
    // If given pins are declared as constexpr, the compiler
    // will optimize the ifs away!

    template<typename... PINs>
    __attribute__((always_inline))
    static inline void high(PINs const&... pins){        
        if ( ((Port::C == pins.port) && ...) )
            PORTC = PORTC | (pins.pin_mask | ...);
        else if ( ((Port::D == pins.port) && ...) )
            PORTD = PORTD | (pins.pin_mask | ...);
        else { 
            // TODO: Fallback -> Use regular loop
        }
    }

    template<typename... PINs>
    __attribute__((always_inline))
    static inline void low(PINs const&... pins){        
        if ( ((Port::C == pins.port) && ...) )
            PORTC = PORTC & (~(pins.pin_mask | ...));
        else if ( ((Port::D == pins.port) && ...) )
            PORTD = PORTD & (~(pins.pin_mask | ...));
        else { 
            // TODO: Fallback -> Use regular loop
        }
    }

    __attribute__((always_inline))
    static inline void pulse(const ppin_t& pin){
        if (pin.port == Port::C) {
            PORTC = PORTC | pin.pin_mask;
            PORTC = PORTC & (~pin.pin_mask);
        }
        else if (pin.port == Port::D) {
            PORTD = PORTD | pin.pin_mask;
            PORTD = PORTD & (~pin.pin_mask);
        }
    }

};

