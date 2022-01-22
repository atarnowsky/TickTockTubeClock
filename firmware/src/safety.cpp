#include "safety.h"
#include "ds3232.h"
#include "hw_map.h"
#include "sound.h"

namespace Safety {
    void TemperatureCheck::initialize() {
        
    }

    void TemperatureCheck::process() {
        auto delayms = [](uint16_t ms) {
            for(uint16_t i = 0; i < ms; i++)
                delayMicroseconds(1000);
        };

        if(DS3232M::temperature() >= shutdown_temperature) {            
            // Shutdown MUXes to reduce current consumption
            cli();
            IO::low(Pins::Anode::MuxA, Pins::Anode::MuxB); 
            
            for(;;) {
                // Flash LEDS once in a while
                IO::high(Pins::Light);                
                delayms(100);
                IO::low(Pins::Light);
                delayms(100);
                IO::high(Pins::Light);
                delayms(100);
                IO::low(Pins::Light);                
                delayms(1000);
            }
        }
    }
}