#pragma once

#include "scheduler.h"

class AmbientLight : public RelaxedTask<2500> {
 public:
    static void initialize();
    static void process();
    
    static float lux();

 private:
    static constexpr uint8_t i2c_address = 0x44;
};

