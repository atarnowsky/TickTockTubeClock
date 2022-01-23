#pragma once

#include "scheduler.h"

class AmbientLight : public RelaxedTask<2500> {
 public:
    static void initialize();
    static void process();
    
    static uint32_t milli_lux();

    // TODO: Add nice non-linear mapping from lux to 0..99
    // static uint8_t brightness();

 private:
    static constexpr uint8_t i2c_address = 0x44;
};

