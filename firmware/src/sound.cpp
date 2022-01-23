#include "sound.h"
#include "hw_map.h"
#include "toneAC2.h"

namespace {
    TickTockSound tick_tock = TickTockSound::None;
}

void SoundGenerator::initialize() {    
   
}

void SoundGenerator::process(){
    auto short_click = [](uint8_t length) {
        noInterrupts();
        for(uint8_t i = 0; i < length; i++)
            IO::high(Pins::Speaker);
        IO::low(Pins::Speaker);
        interrupts();  
    };

    auto short_beep = [](uint8_t length = 50, int16_t freq = 320) {
        toneAC2(Pins::Speaker.arduino_pin(), Pins::Unused.arduino_pin(), freq, length, false);
    };

    switch (tick_tock) {
        case TickTockSound::None:
            break;
        
        case TickTockSound::ClickSilent:
            short_click(16); 
            break;

        case TickTockSound::ClickMedium:
            short_click(32); 
            break;

        case TickTockSound::ClickLoud:
            short_click(64); 
            break;

        case TickTockSound::LowBeep:
            short_beep(20, speaker_resonant_frequency >> 4);
            break;                

        case TickTockSound::HighBeep:
            short_beep(20, speaker_resonant_frequency);
            break;                
    }
}

void SoundGenerator::set_tick_tock(TickTockSound v) 
{
    tick_tock = v;
}

void SoundGenerator::error(uint8_t bits)
{        
    for(int i = 0; i < 4; i++)
    {            
        toneAC2(Pins::Speaker.arduino_pin(), Pins::Unused.arduino_pin(), ((bits & (1 << i)) > 0 ? 800 : 400), 250, true);            
        delay(500);
    }
    delay(500);
}

void SoundGenerator::ack_short(){
    toneAC2(Pins::Speaker.arduino_pin(), Pins::Unused.arduino_pin(), 1000, 75, true);
    delay(75);    
}

void SoundGenerator::ack_long() {
    toneAC2(Pins::Speaker.arduino_pin(), Pins::Unused.arduino_pin(), 1000, 75, true);
    delay(75 + 150);
    toneAC2(Pins::Speaker.arduino_pin(), Pins::Unused.arduino_pin(), 2000, 75, true);
    delay(75);
}

void SoundGenerator::ack_reset() {
    toneAC2(Pins::Speaker.arduino_pin(), Pins::Unused.arduino_pin(), 1000, 75, true);
    delay(75 + 150);
    toneAC2(Pins::Speaker.arduino_pin(), Pins::Unused.arduino_pin(), 2000, 75, true);
    delay(150);
    for(uint8_t i = 0; i < 3; i++) {
        toneAC2(Pins::Speaker.arduino_pin(), Pins::Unused.arduino_pin(), 1000, 250, true);
        delay(500);
    }
}

void SoundGenerator::play(const Note& n) {
    toneAC2(Pins::Speaker.arduino_pin(), Pins::Unused.arduino_pin(), n[0], n[1]);
    delay(n[2]);
}