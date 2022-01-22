#include "ambient_light.h"
#include "simple_i2c.h"

namespace {
    float current_lux = 0.0f;
}

void AmbientLight::initialize() {
    Wire.beginTransmission(i2c_address);
    Wire.write(0x1);        
    Wire.write(0b11001110);
    Wire.write(0b00000000);
    // Assume everything went well.
    // No error handling for now...
    Wire.endTransmission(); 
}

void AmbientLight::process() {
    uint16_t raw_result = I2C::read_16(i2c_address, 0x0);
    uint16_t fractional = raw_result & 0xFFF;
    uint8_t exponent = uint8_t(raw_result >> (16 - 4));

    current_lux = 0.01 * pow(2, exponent) * fractional;
}

float AmbientLight::lux() {
    return current_lux;
}