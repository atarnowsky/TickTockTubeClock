#pragma once

#include "scheduler.h"
#include "io_tools.h"
#include "hw_map.h"

class BaseLightDimmer : public CriticalTask<255> {
 public:
    static void initialize(uint16_t update_rate) {
        (void)update_rate;
        //digitalWrite(Pins::Light.arduino_pin, HIGH);
        IO::PortC::high(Pins::Light);
    }

    static void process(uint16_t cycle_count) {

    }

 private:
    static constexpr uint8_t pwm_width = 255;
};