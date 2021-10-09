#include "sound.h"
#include "hw_map.h"

namespace {
    TickTockSound tick_tock = TickTockSound::None;
    //AlarmSound SoundGenerator::alarm_sound = AlarmSound::DoubleHigh;
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
        tone(Pins::Speaker.arduino_pin(), freq, length);
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
        tone(Pins::Speaker.arduino_pin(), ((bits & (1 << i)) > 0 ? 800 : 400), 250);            
        delay(500);
    }
    delay(500);
}

void SoundGenerator::ack(uint8_t times){
    for(uint8_t i = 0; i < times; i++)
    {
        tone(Pins::Speaker.arduino_pin(), 4000, 50);
        delay(100);
    }
}
