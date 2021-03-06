#include "sound.h"
#include "hw_map.h"
//#include "toneAC2.h"
#include "scheduler.h"

namespace {
    TickTockSound tick_tock = TickTockSound::None;    
}

namespace SoundGenerator {

    void tick() {
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

    void set_tick_tock(TickTockSound v) 
    {
        tick_tock = v;
    }

    void nack(){
        pause_critical();
        tone(Pins::Speaker.arduino_pin(), 500, 75);
        delay(75*3/2);    
        tone(Pins::Speaker.arduino_pin(), 500, 75);
        delay(75*3/2);    
        tone(Pins::Speaker.arduino_pin(), 500, 75);
        delay(75*3/2);    
        resume_critical();
    }

    void ack_short(){
        pause_critical();
        tone(Pins::Speaker.arduino_pin(), 1000, 75);
        delay(75);    
        resume_critical();
    }

    void ack_dynamic(uint8_t value, int16_t delta){
        pause_critical();
        tone(Pins::Speaker.arduino_pin(), 1000 + value * delta, 75);
        delay(75);    
        resume_critical();
    }

    void ack_long() {
        pause_critical();
        tone(Pins::Speaker.arduino_pin(), 1000, 75);
        delay(75 + 150);
        tone(Pins::Speaker.arduino_pin(), 2000, 75);
        delay(75);
        resume_critical();
    }

    void ack_reset() {
        pause_critical();
        tone(Pins::Speaker.arduino_pin(), 1000, 75);
        delay(75 + 150);
        tone(Pins::Speaker.arduino_pin(), 2000, 75);
        delay(150);
        for(uint8_t i = 0; i < 3; i++) {
            tone(Pins::Speaker.arduino_pin(), 1000, 250);
            delay(500);
        }
        resume_critical();
    }

    void play(const Note& n) {        
        tone(Pins::Speaker.arduino_pin(), n[0], n[1]);
        delay(n[2]);        
    }
}