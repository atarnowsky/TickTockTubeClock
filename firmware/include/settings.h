#pragma once
#include "etl_fix.h"
#include <EEPROM.h>

enum class Setting : uint8_t {
    LED_BRIGHTNESS,
    TUBE_BRIGHTNESS,
    TICK_SOUND,
    TRANSITION_EFFECT,
    AMBIENT_EFFECT,
    NIGHT_MODE_THRESHOLD,
    SHUTDOWN_BEGIN,
    SHUTDOWN_END,
    FIRST_START     // Determines, whether the clock has been initialized or not
                    // Has to be the last entry in the enum since it is also used
                    // to query the number of available settings.
};

struct SettingSet {
    uint8_t led_brightness;
    uint8_t tube_brightness;
    uint8_t tick_sound;
    uint8_t transition_effect;
    uint8_t ambient_effect;
    uint8_t night_mode_threshold;
    uint8_t shutdown_begin;
    uint8_t shutdown_end;
};


// Assure enum and struct are in-sync
// FIRST_START entry allways has to denote the very last entry!
static_assert(sizeof(SettingSet) == static_cast<uint8_t>(Setting::FIRST_START));

namespace Settings {
    inline void set(Setting setting, uint8_t value) {
        EEPROM[static_cast<uint8_t>(setting)] = value;
    }

    inline uint8_t get(Setting setting) {
        return EEPROM[static_cast<uint8_t>(setting)];
    }

    inline SettingSet get() {
        SettingSet result;
        // Settings set does not include the FIRST_START entry, hence the -1
        array<uint8_t, static_cast<uint8_t>(Setting::FIRST_START) - 1>& raw =
            reinterpret_cast<array<uint8_t, static_cast<uint8_t>(Setting::FIRST_START) - 1>&>(result);

        // Assure the reinterpret cast actually is allowed.
        // Unfortunately, ArxTypeTraits does not support is_standard_layout, yet.
        // static_assert(std::is_standard_layout(etl::array<uint8_t, static_cast<uint8_t>(Settings::FIRST_START)>)); 
        
        for(uint8_t index = 0; index < static_cast<uint8_t>(Setting::FIRST_START); index++) {
            raw[index] = EEPROM[index];
        }

        return result;
    }

    inline bool on_first_start() {
        if(get(Setting::FIRST_START) == 1) {
            set(Setting::FIRST_START, 0);
            return true;
        }
        return false;        
    }

    inline void reset() {
        set(Setting::FIRST_START, 1);
        set(Setting::LED_BRIGHTNESS, 32);
        set(Setting::TUBE_BRIGHTNESS, 255);
        set(Setting::TICK_SOUND, 1);   
        set(Setting::TRANSITION_EFFECT, 1);
        set(Setting::AMBIENT_EFFECT, 0);
        set(Setting::NIGHT_MODE_THRESHOLD, 0);
        set(Setting::SHUTDOWN_BEGIN, 0);
        set(Setting::SHUTDOWN_END, 0);
    }
}