#pragma once

#include "state.h"
#include "timing.h"
#include "display.h"
#include "sound.h"
#include "ds3232.h"
#include "ambient_light.h"

// State definition for "hidden" service menu
// Press 'o' in clock state for at least 10 seconds.
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
    reset_environment();
    SoundGenerator::ack_reset();        
  }

  static void process() {
      static uint8_t delay_counter = 0;
      static constexpr uint8_t delay_maximum = 1000 / state_update_rate;
      if(delay_counter == delay_maximum) {          
          delay_counter = 0;
          if(acp_countdown > 0) {
            Display::AntiCathodePoisoning::enable();
            acp_countdown--;
          } else {
            Display::AntiCathodePoisoning::disable();
            DS3232M::temperature_t temperature = DS3232M::temperature();
            Effects::Transition::display(255, temperature, {false, false, false, false});            
          }
      }      
      delay_counter++;
  }

  static void on_plus_short() {      
      SoundGenerator::ack_short();
      acp_countdown = 5 * 60;
  }

  static void on_plus_long() {      
      SoundGenerator::ack_long();
      acp_countdown = 30 * 60;
  }

  static void on_minus_long() {      
      SoundGenerator::ack_reset();
      Settings::reset();            
      RTCSync::reset();
    
      // Stop program completely and wait
      // for new power cycle
      cli();
      IO::low(Pins::Anode::MuxA, Pins::Anode::MuxB); 
      for(;;);
  }

  static void on_select_short() {
     UI::next<Clock>(); 
  }

private:
  static uint16_t acp_countdown;
};

uint16_t ServiceMenu::acp_countdown = 0;

