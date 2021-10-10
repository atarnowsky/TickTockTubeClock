#include "base_light.h"

#include "io_tools.h"
#include "hw_map.h"

namespace {
    uint16_t fade_state = 0;
    uint16_t fade_target = 0;
    uint8_t fade_increment = 0;
}

void BaseLightDimmer::initialize(uint16_t update_rate) {
    (void)update_rate; 
}

void BaseLightDimmer::process(uint8_t cycle_count) {
    // TODO: Would be less flickery if we distribute
    // on and off states across the cycles...
    // Looks okay, as long as the cycle_count is not larger than 32

    if(fade_state < fade_target)
        fade_state += fade_increment;
    else if(fade_state > fade_target)
        fade_state -= fade_increment;
    
    
    //                                  16bit    ->       8bit        -> 0..31
    const uint8_t pwm_width = uint8_t(fade_state >> slowmotion_factor >> 3);

    if(cycle_count < pwm_width) {
        IO::high(Pins::Light);
    } else {
        IO::low(Pins::Light);
    }
}

void BaseLightDimmer::set_brightness(uint8_t target) {
    fade_target = target << slowmotion_factor;
    fade_state = target << slowmotion_factor;
}

void BaseLightDimmer::set_fade_target(uint8_t target) {
    fade_target = target << slowmotion_factor;
}

void BaseLightDimmer::set_fade_speed(uint8_t speed) {
    fade_increment = speed;
}