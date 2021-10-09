#include "ambient_light.h"

#include <ClosedCube_OPT3001.h>

namespace {
    ClosedCube_OPT3001 opt3001;
    float current_lux = 0.0f;
}

void AmbientLight::initialize() {
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

void AmbientLight::process() {
    OPT3001 result = opt3001.readResult();
    // TODO: What to do in case of error? Maybe show once and disable sensor?
    if(result.error == NO_ERROR)  {
        current_lux = result.lux;
    }
}

float AmbientLight::lux() {
    return current_lux;
}