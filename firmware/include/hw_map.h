#pragma once

#include <Arduino.h>
#include "etl_fix.h"
#include "io_tools.h"

// This file contains all pin mappings and is included by almost all other files.
// In case of new hardware revisions, make sure this file gets adjusted accordingly.

namespace Pins { 
    constexpr array<pin_t, 3> Buttons = {PIN_PD1, PIN_PD2, PIN_PD0};
    constexpr ppin_t Speaker = 1_C;
    constexpr ppin_t Light = 0_C;

    namespace I2C {
        constexpr pin_t SDA = 18;
        constexpr pin_t SCL = 19;
    }

    namespace Shift {
        constexpr ppin_t Data = 7_D;
        constexpr ppin_t Clock = 6_D;
        constexpr ppin_t Latch = 5_D;

        static void shift_out(uint8_t data);
    }

    namespace Anode {
        constexpr ppin_t MuxA = 3_D;
        constexpr ppin_t MuxB = 4_D;
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

// Implementation

static void Pins::Shift::shift_out(uint8_t data){  
    for(uint8_t i = 0; i < 8; i++){   
        IO::low(Clock);
        if((data & 0b00000001))
            IO::high(Data);
        else
            IO::low(Data);
        IO::high(Clock);
        data >>= 1;
    }
}

