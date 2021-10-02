#pragma once

#include "scheduler.h"
#include "io_tools.h"
#include "hw_map.h"

class BaseLightDimmer : public CriticalTask<32> {
 public:
    static void initialize(uint16_t update_rate) {
        (void)update_rate;
        //digitalWrite(Pins::Light.arduino_pin, HIGH);        
    }

    static void process(uint16_t cycle_count) {
        // TODO: Would be less flickery if we distribute
        // on and off states across the cycles...
        // Looks okay, as long as the cycle_count is not larger than 32
        if(cycle_count < pwm_width) {
            IO::PortC::high(Pins::Light);
        } else {
            IO::PortC::low(Pins::Light);
        }
    }

 private:
    static constexpr uint8_t pwm_width = 4;
};