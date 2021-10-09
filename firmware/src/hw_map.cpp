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
