#pragma once

#include <Arduino.h>
#include "etl_fix.h"
#include "io_tools.h"

// This file contains all pin mappings and is included by almost all other files.
// In case of new hardware revisions, make sure this file gets adjusted accordingly.


namespace Pins {    
    constexpr array<pin_t, 3> Buttons = {PIN_PD1, PIN_PD2, PIN_PD0};
    constexpr pin_t Speaker = PIN_PC1;
    constexpr pin_t Light = PIN_PC0;

    namespace I2C {
        constexpr pin_t SDA = 18;
        constexpr pin_t SCL = 19;
    }

    namespace Shift {
        constexpr pin_t Data = PIN_PD7;
        constexpr pin_t Clock = PIN_PD6;
        constexpr pin_t Latch = PIN_PD5;
    }

    namespace Anode {
        constexpr pin_t MuxA = PIN_PD3;
        constexpr pin_t MuxB = PIN_PD4;
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
