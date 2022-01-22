#include "simple_i2c.h"


#include <Wire.h>

namespace I2C
{    
    namespace util {
        bool wait_available(uint8_t size) {
            uint8_t cnt = 0;
            while (Wire.available() < size)
            {
                delay(10);
                if (cnt++ > 50)
                    return false;
            }
            return true;
        }
    }

    void initialize() {
        Wire.begin();
    }

    uint8_t read(int address, uint8_t reg) {
        Wire.beginTransmission(address);
        Wire.write(reg);
        Wire.endTransmission();
        Wire.requestFrom(address, 1);
        if(!util::wait_available(1)) {
            return 0;
        }
        return Wire.read();
    }

    uint16_t read_16(int address, uint8_t reg) {        
        Wire.beginTransmission(address);
        Wire.write(reg);
        Wire.endTransmission();
        Wire.requestFrom(address, 2);  
        if(!util::wait_available(2)) {
            return 0;
        }
        return (Wire.read() << 8) | Wire.read();
    }

    uint8_t write(int address,uint8_t reg, uint8_t value) {
        Wire.beginTransmission(address);
        Wire.write(reg);
        Wire.write(value);
        return Wire.endTransmission();
    }

    uint8_t write(int address,uint8_t command) {
        Wire.beginTransmission(address);
        Wire.write(command);        
        return Wire.endTransmission();
    }
} // namespace I2C
