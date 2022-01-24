#pragma once

#include "scheduler.h"

class AmbientLight : public RelaxedTask<1000> {
 public:
    static void initialize();
    static void process();
    
    static uint32_t milli_lux();
    
 private:
    static constexpr uint8_t i2c_address = 0x44;
};

