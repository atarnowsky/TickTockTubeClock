#pragma once
#include "etl_fix.h"
#include <EEPROM.h>

enum class Setting : uint8_t {
    LED_BRIGHTNESS,
    TUBE_BRIGHTNESS,
    TICK_SOUND,
    TRANSITION_EFFECT,
    AMBIENT_EFFECT,
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
};


// Assure enum and struct are in-sync
// FIRST_START entry allways has to denote the very last entry!
static_assert(sizeof(SettingSet) == static_cast<uint8_t>(Setting::FIRST_START));

namespace Settings {
    void set(Setting setting, uint8_t value) {
        EEPROM[static_cast<uint8_t>(setting)] = value;
    }

    uint8_t get(Setting setting) {
        return EEPROM[static_cast<uint8_t>(setting)];
    }

    SettingSet get() {
        SettingSet result;
        array<uint8_t, static_cast<uint8_t>(Setting::FIRST_START)>& raw =
            reinterpret_cast<array<uint8_t, static_cast<uint8_t>(Setting::FIRST_START)>&>(result);

        // Assure the reinterpret cast actually is allowed.
        // Unfortunately, ArxTypeTraits does not support is_standard_layout, yet.
        // static_assert(std::is_standard_layout(etl::array<uint8_t, static_cast<uint8_t>(Settings::FIRST_START)>)); 
        
        // TODO: Make constexpr
        for(uint8_t index = 0; index < static_cast<uint8_t>(Setting::FIRST_START); index++) {
            raw[index] = EEPROM[index];
        }

        return result;
    }
}