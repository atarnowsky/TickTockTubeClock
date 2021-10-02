#pragma once

#include <ClosedCube_OPT3001.h>
#include "scheduler.h"

class AmbientLight : public RelaxedTask<2500> {
 public:
    static void initialize() {
        opt3001.begin(i2c_address);
        OPT3001_Config config;  
        config.RangeNumber = B1100;  
        config.ConvertionTime = B1;
        config.Latch = B0;
        config.ModeOfConversionOperation = B11;

        OPT3001_ErrorCode error = opt3001.writeConfig(config);
        if (error != NO_ERROR) {
            // Show error code
            return;
        }    
    }

    static void process() {
        OPT3001 result = opt3001.readResult();
        // TODO: What to do in case of error? Maybe show once and disable sensor?
        if(result.error == NO_ERROR)  {
            current_lux = result.lux;
        }
    }

    static float lux() {
        return current_lux;
    }

 private:
    static constexpr uint8_t i2c_address = 0x44;
    static ClosedCube_OPT3001 opt3001;
    static float current_lux;
};

ClosedCube_OPT3001 AmbientLight::opt3001;
float AmbientLight::current_lux = 0.0f;
