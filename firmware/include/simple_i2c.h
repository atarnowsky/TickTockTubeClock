#pragma once

#include <Wire.h> // TODO: Remove as soon interface is complete

namespace I2C
{    
    void initialize();
    uint8_t read(int address, uint8_t reg);
    uint16_t read_16(int address, uint8_t reg);
    uint8_t write(int address,uint8_t reg, uint8_t value);
} // namespace I2C
