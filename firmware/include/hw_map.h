#pragma once

#include "etl_fix.h"
#include "io_tools.h"

// This file contains all pin mappings and is included by almost all other files.
// In case of new hardware revisions, make sure this file gets adjusted accordingly.

namespace Pins { 
    constexpr array<pin_t, 3> Buttons = {PIN_PD1, PIN_PD2, PIN_PD0};
    //constexpr array<ppin_t, 3> Buttons = {1_D, 2_D, 0_D};
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

        template<typename... DATAs>
        void shift_out(DATAs... data);
    }

    namespace Anode {
        constexpr ppin_t MuxA = 3_D;
        constexpr ppin_t MuxB = 4_D;
    }

    void setup();
}

namespace Timings {
    constexpr uint32_t button_long_press = 1000;
    constexpr uint32_t button_reset_press = 10000;
}


template<typename... DATAs>
void Pins::Shift::shift_out(DATAs... data){  
    auto unpack = [&](uint8_t data) {
        for(uint8_t i = 0; i < 8; i++){   
            IO::low(Clock);
            if((data & 0b00000001))
                IO::high(Data);
            else
                IO::low(Data);
            IO::high(Clock);
            data >>= 1;
        }
    };

    (void)(unpack(data), ...);
}