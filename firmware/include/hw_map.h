#pragma once

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

        void shift_out(uint8_t data);
    }

    namespace Anode {
        constexpr ppin_t MuxA = 3_D;
        constexpr ppin_t MuxB = 4_D;
    }

    void setup();
}
