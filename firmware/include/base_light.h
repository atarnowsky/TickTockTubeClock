#pragma once

#include "scheduler.h"

class BaseLightDimmer : public CriticalTask<32> {
 public:
    static void initialize(uint16_t update_rate);
    static void process(uint16_t cycle_count);

    static void set_brightness(uint8_t target);
    static void set_fade_target(uint8_t target);
    static void set_fade_speed(uint8_t speed);

 private:
    static constexpr uint8_t slowmotion_factor = 8;
};
