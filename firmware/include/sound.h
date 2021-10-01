
#include <stdint.h>
#include <digitalWriteFast.h>
#include "hw_map.h"

enum class TickTockSound : uint8_t {
    None = 0,
    MonotonousClick = 1,
    MonotonousBeep = 2,
    BitoneClick = 3,
    BitoneBeep = 4
};

enum class AlarmSound : uint8_t {
    SingleLow = 0,
    SingleHigh = 1,
    DoubleLow = 2,
    DoubleHigh = 3,
    TripleLow = 4,
    TripleHigh = 5    
};

class SoundGenerator {
public:
    SoundGenerator(int pin) : pin(pin),
    tick_tock(TickTockSound::MonotonousClick),
    alarm_sound(AlarmSound::DoubleHigh)
    {
        pinMode(pin, OUTPUT);
        load_state();
    }

    void process(){
        long diff = millis() - last_tick ;

        if(tick_tock == TickTockSound::MonotonousClick)
        {
            if(diff >= 1000)
            {
                play_sound(TickTockSound::MonotonousClick);
                last_tick = millis();
            }
        }

        if(tick_tock == TickTockSound::BitoneClick)
        {
            if(diff >=400 && !bitone_clicked)
            {
                play_sound(TickTockSound::BitoneClick);                
                bitone_clicked = true;
            }             
            else if(diff >= 1000)
            {
                play_sound(TickTockSound::MonotonousClick);
                bitone_clicked = false;
                last_tick = millis();
            }
        }        
    }

    void set_tick_tock(TickTockSound v) 
    {
        tick_tock = v;
        save_state();
    }

    void set_alarm_sound(AlarmSound v) 
    {
        alarm_sound = v;
        save_state();
    }

    void tick()
    {
        play_sound(tick_tock);
    }

    void tick(TickTockSound v)
    {
        play_sound(v);
    }

    void alarm() 
    {
        play_sound(alarm_sound);
    }

    void alarm(AlarmSound v) 
    {
        play_sound(v);
    }

    void error(uint8_t bits)
    {        
        for(int i = 0; i < 4; i++)
        {            
            tone(pin, ((bits & (1 << i)) > 0 ? 800 : 400), 250);            
            delay(500);
        }
        delay(500);
    }

    void ack(uint8_t times = 1){
        for(uint8_t i = 0; i < times; i++)
        {
            tone(pin, 4000, 50);
            delay(100);
        }
    }


private:
    void load_state() {

    }

    void save_state() {

    }

    void play_sound(TickTockSound v) {
        switch (v)
        {
        case TickTockSound::None:
            break;
        case TickTockSound::MonotonousClick:
            noInterrupts();
            for(int i = 0; i < 4; i++) {
                digitalWriteFast(pin, HIGH);            
                digitalWriteFast(pin, LOW);
            }
            interrupts();         
            break;
        case TickTockSound::MonotonousBeep:
            tone(pin, 320, 50);
            break;
        case TickTockSound::BitoneClick:            
            noInterrupts();
            for(int i = 0; i < 2; i++) {
                digitalWriteFast(pin, HIGH);            
                digitalWriteFast(pin, LOW);
            }
            interrupts();
            break;
        case TickTockSound::BitoneBeep:
            tone(pin, 320, 35);
            delay(500-35*2);
            tone(pin, 240, 35);
            break;
        }

    }

    void play_sound(AlarmSound v) {
        
    }

    int pin;
    TickTockSound tick_tock;
    AlarmSound alarm_sound;
    long last_tick = 0;
    bool bitone_clicked = false;
};