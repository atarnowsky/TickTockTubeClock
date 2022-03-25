
#pragma once

#include "hw_map.h"

using Note = array<uint16_t, 3>;

enum class TickTockSound : uint8_t {
    None = 0,
    ClickSilent = 1,
    ClickMedium = 2,
    ClickLoud = 3,
    LowBeep = 4, 
    HighBeep = 5, 
};

namespace SoundGenerator  {
    void set_tick_tock(TickTockSound v);    

    void error(uint8_t bits);

    void nack();
    void ack_dynamic(uint8_t value, int16_t delta);
    void ack_short();
    void ack_long();
    void ack_reset();
    void tick();

    void play(const Note& n);

    static constexpr uint16_t speaker_resonant_frequency = 4000; // [Hz]
};


