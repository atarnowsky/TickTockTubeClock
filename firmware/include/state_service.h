#pragma once

#include "state.h"
#include "timing.h"
#include "display.h"
#include "sound.h"
#include "ds3232.h"

// State definition for "hidden" service menu
// Press 'o' in clock state for at least 15 seconds.
// On state enter, a special sound is played and the
// current temperature (0-99, right digits) 
// gets displayed.

// Further actions:
// 'o' short -> Clock
// '+' short -> 5 minutes ACP
// '+' long -> 30 minutes ACP
// '-' long -> Reset to factory defaults

class Clock;

class ServiceMenu : public State<0> {
public: 
  static void initialize() {        
    SoundGenerator::set_tick_tock(TickTockSound::None);
    Display::ShiftPWMProcessor::set_brightness(255);
    Effects::Transition::set_effect(Effects::NumberTransition::FADE_CROSS, 7);
    Effects::Ambient::set_effect(Effects::AmbientEffect::NONE);    
    Display::SeparatorDot::disable();    
    SoundGenerator::ack_reset();        
  }

  static void process() {
      static uint8_t delay_counter = 0;
      static constexpr uint8_t delay_maximum = 1000 / state_update_rate;
      if(delay_counter == delay_maximum) {
          DS3232M::temperature_t temperature = DS3232M::temperature();
          Effects::Transition::display(255, temperature, {false, false, false, false});
          delay_counter = 0;
          if(acp_countdown > 0) {
            Display::AntiCathodePoisoning::enable();
            acp_countdown--;
          } else {
            Display::AntiCathodePoisoning::disable();
          }
      }      
      delay_counter++;
  }

  static void on_plus_short() {      
      SoundGenerator::ack_short();
      acp_countdown = 5 * 60 * 1000;
  }

  static void on_plus_long() {      
      SoundGenerator::ack_long();
      acp_countdown = 30 * 60 * 1000;
  }

  static void on_minus_long() {      
      SoundGenerator::ack_reset();
      Settings::reset();
      Display::SeparatorDot::disable();
      SoundGenerator::set_tick_tock(TickTockSound::None);
      RTCSync::reset();
    
      // Stop program completely and wait
      // for new power cycle
      cli();
      IO::low(Pins::Anode::MuxA, Pins::Anode::MuxB); 
      for(;;);
  }

private:
  static uint16_t acp_countdown;

};

uint16_t ServiceMenu::acp_countdown = 0;

