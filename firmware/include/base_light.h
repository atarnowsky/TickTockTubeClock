#pragma once

#include "scheduler.h"
#include "io_tools.h"
#include "hw_map.h"

class BaseLightDimmer : public CriticalTask<32> {
 public:
    static void initialize(uint16_t update_rate) {
        (void)update_rate; 
    }

    static void process(uint16_t cycle_count) {
        // TODO: Would be less flickery if we distribute
        // on and off states across the cycles...
        // Looks okay, as long as the cycle_count is not larger than 32

        if(fade_state < fade_target)
            fade_state += fade_increment;
        else if(fade_state > fade_target)
            fade_state -= fade_increment;
        
        
        //                           16bit    ->       8bit        ->     0..31
        const uint16_t pwm_width = fade_state >> slowmotion_factor >> 3;

        if(cycle_count < pwm_width) {
            IO::high(Pins::Light);
        } else {
            IO::low(Pins::Light);
        }
    }

    static void set_brightness(uint8_t target) {
        fade_target = target << slowmotion_factor;
        fade_state = target << slowmotion_factor;
    }

    static void set_fade_target(uint8_t target) {
        fade_target = target << slowmotion_factor;
    }

    static void set_fade_speed(uint8_t speed) {
        fade_increment = speed;
    }

 private:
    static constexpr uint8_t slowmotion_factor = 8;
    static uint16_t fade_state;
    static uint16_t fade_target;
    static uint8_t fade_increment;
};

uint16_t BaseLightDimmer::fade_state = 0;
uint16_t BaseLightDimmer::fade_target = 0;
uint8_t BaseLightDimmer::fade_increment = 0;