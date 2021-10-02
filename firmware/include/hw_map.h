#pragma once

#include <Arduino.h>
#include "etl_fix.h"
#include "io_tools.h"

// This file contains all pin mappings and is included by almost all other files.
// In case of new hardware revisions, make sure this file gets adjusted accordingly.



namespace Pins { 
    constexpr array<pin_t, 3> Buttons = {PIN_PD1, PIN_PD2, PIN_PD0};
    constexpr ppin_t Speaker = {Port::C, 1 << 1, PIN_PC1};
    constexpr ppin_t Light = {Port::C, 1 << 0, PIN_PC0};

    namespace I2C {
        constexpr pin_t SDA = 18;
        constexpr pin_t SCL = 19;
    }

    namespace Shift {
        constexpr ppin_t Data = {Port::D, 1 << 7, PIN_PD7};
        constexpr ppin_t Clock = {Port::D, 1 << 6, PIN_PD6};
        constexpr ppin_t Latch = {Port::D, 1 << 5, PIN_PD5};

        __attribute__((always_inline))
        __attribute__((optimize("unroll-loops")))
        static inline void shift_out(uint8_t data){  
            for(uint8_t i = 0; i < 8; i++){   
                IO::PortD::low(Clock);
                if((data & 0b00000001))
                    IO::PortD::high(Data);
                else
                    IO::PortD::low(Data);
                IO::PortD::high(Clock);
                data >>= 1;
            }
        }
    }

    namespace Anode {
        constexpr ppin_t MuxA = {Port::D, 1 << 3, PIN_PD3};
        constexpr ppin_t MuxB = {Port::D, 1 << 4, PIN_PD4};
    }

    void setup() {
        IO::configure<INPUT_PULLUP>(
            I2C::SDA, I2C::SCL, // There are already 10K pullup on board, but internals won't make it worse
            Buttons             // Mandatory, since there is not enough space for external pullups
        );

        IO::configure<OUTPUT>(
            Speaker, Light,
            Shift::Data, Shift::Clock, Shift::Latch,
            Anode::MuxA, Anode::MuxB
        );
    }
}
