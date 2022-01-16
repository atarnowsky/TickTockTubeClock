
#pragma once

#include "scheduler.h"
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

// TODO: Use a non-blocking tone function and split class up into SoundGenerator and SoundManager
class SoundGenerator : public RelaxedTask<1000> {
 public:
    static void initialize();
    static void process();
    static void set_tick_tock(TickTockSound v);    

    static void error(uint8_t bits);
    static void ack(uint8_t times = 1);

    static void ack_short();
    static void ack_long();
    static void ack_reset();


    template<typename... Seq>
    inline static void play_sequence(Seq... sequence) {
        auto note = [](const Note& n) {
            tone(Pins::Speaker.arduino_pin(), n[0], n[1]);
            delay(n[1]);
            noTone(Pins::Speaker.arduino_pin());
            delay(n[2]);
        };

        (play_sequence(sequence), ...);
    }

 private:
    static constexpr uint16_t speaker_resonant_frequency = 4000; // [Hz]
};

