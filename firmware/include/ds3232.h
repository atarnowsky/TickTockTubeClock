#pragma once
#include <Wire.h>
#include "timing.h"
// This is an absolute minimal interface to the RS3232M RTC IC.
// For generale usage, I highly recommend a properly implemented
// library like Jack Christensen's DS3232RTC Library
// @see https://github.com/JChristensen/DS3232RTC
// For this project, we do not need any advanced features like
// calendar and alarm functions. We only use:
//  - Set/get current hours and minutes
//  - Get current temperature
//  - Get oscillator status
// Basically, this is a stripped down version of the aforementioned library.

namespace DS3232M {
    enum class Status : uint8_t {
        OKAY,
        I2C_ERROR,
        OSCILLATOR_FAILURE
    };

    namespace util {
        inline uint8_t decode_bcd(uint8_t n) {
            return n - 6 * (n >> 4);
        }

        inline uint8_t to_bcd(uint8_t n) {
            return n + 6 * (n / 10);
        }

        uint8_t read(uint8_t reg) {
            Wire.beginTransmission(0x68);
            Wire.write(reg);
            Wire.endTransmission();
            Wire.requestFrom(0x68, 1);
            return Wire.read();
        }

        uint8_t write(uint8_t reg, uint8_t value) {
            Wire.beginTransmission(0x68);
            Wire.write(reg);
            Wire.write(value);
            return Wire.endTransmission();
        }
    }

    using temperature_t = uint8_t;

    inline Status initialize() {
        Wire.begin();

        // Set control register
        if(util::write(0x0E, 0b00100100) != 0){
            return Status::I2C_ERROR;   
        }

        // Set status register
        uint8_t stat_reg = util::read(0x0F);
        util::write(0x0F, 0b00000000);

        if((stat_reg & (1 << 7)) > 0) {
            return Status::OSCILLATOR_FAILURE;
        }
        else {
            return Status::OKAY;
        }
    }

    inline temperature_t temperature() {
        // We do not need the fractional part...
        return util::read(0x11);
    }

    inline time_pair time() {
        Wire.beginTransmission(0x68);
        Wire.write(0x00);
        uint8_t error = Wire.endTransmission();
        if(error) {
            return {0, 0};
        }
        Wire.requestFrom(0x68, 7);
        time_pair result;
        (void)Wire.read(); // Ignore seconds
        result.minutes = util::decode_bcd(Wire.read());
        result.hours = util::decode_bcd(Wire.read() & ~(1 << 6));
        // Ignore rest of query
        for(uint8_t i = 0; i < 4; i++)
            Wire.read();
        return result;
    }

    inline void set_time(const time_pair& time) {
        Wire.beginTransmission(0x68);
        Wire.write(0x00);
        Wire.write(0); // seconds
        Wire.write(util::to_bcd(time.minutes)); 
        Wire.write(util::to_bcd(time.hours)); 
        for(uint8_t i = 0; i < 4; i++)
            Wire.write(1);   
        Wire.endTransmission();
    }

}