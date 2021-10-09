#include "hw_map.h"


void Pins::setup() {
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

void Pins::Shift::shift_out(uint8_t data){  
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