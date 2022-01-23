#pragma once

#include "effects.h"
#include "base_light.h"

template<uint8_t timeout_seconds = 0>
class State {
 public:
    static constexpr uint32_t timeout = uint32_t(timeout_seconds) * 1000;

    // Will be called exactly once when the State gets active
    inline static void initialize() {}

    // Will be called periodically as long as the active state stays
    // the same. May trigger spontaneous State transistions.
    inline static void process() {}

    // May be used to write to EEPROM.
    // May NOT be used to alter StateMachine!
    inline static void finish() {}

    // Event handlers - default to no-op
    inline static void on_plus_short() {}
    inline static void on_plus_long() {}
    inline static void on_minus_short() {}
    inline static void on_minus_long() {}
    inline static void on_select_short() {}
    inline static void on_select_long() {}

    // Mainly used for the "reset to factory defaults" task
    inline static void on_select_reset() {}

    // Will be called once when no buttons have been
    // pressed for the specified timeout
    inline static void on_timeout() {}

protected:
    // Helper method to save a little bit of flash:
    // Reset the most common modes to a safe default    
    static void reset_environment() {        
        Display::ShiftPWMProcessor::set_brightness(255);
        Effects::Transition::set_effect(Effects::NumberTransition::FADE_CROSS, 7);
        Effects::Ambient::set_effect(Effects::AmbientEffect::NONE);    
        BaseLightDimmer::set_fade_target(0);        
    }
};