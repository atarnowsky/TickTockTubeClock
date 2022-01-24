#pragma once

#include "configuration.h"
#include "display.h"
#include "state.h"
#include "timing.h"
#include "settings.h"
#include "sound.h"


class Clock : public State<0>{
public:
  static void initialize() {
    settings = Settings::get();

    BaseLightDimmer::set_fade_target(settings.led_brightness);
    Display::ShiftPWMProcessor::set_brightness(settings.tube_brightness);
    Effects::Transition::set_max_brightness(settings.tube_brightness);
    Effects::Ambient::set_max_brightness(settings.tube_brightness);    

    SoundGenerator::set_tick_tock(static_cast<TickTockSound>(settings.tick_sound));    

    Effects::NumberTransition transition = static_cast<Effects::NumberTransition>(settings.transition_effect);
    switch(transition) {
      case Effects::NumberTransition::NONE:
        Effects::Transition::set_effect(Effects::NumberTransition::NONE, 0);
        break;
      case Effects::NumberTransition::FADE_BLACK:
        Effects::Transition::set_effect(Effects::NumberTransition::FADE_BLACK, 11);
        break;
      case Effects::NumberTransition::FADE_CROSS:
        Effects::Transition::set_effect(Effects::NumberTransition::FADE_CROSS, 10);
        break;
      case Effects::NumberTransition::FLICKER:
        Effects::Transition::set_effect(Effects::NumberTransition::FLICKER, 13);
        break;
    }
    
    Effects::Ambient::set_effect(static_cast<Effects::AmbientEffect>(settings.ambient_effect));  
  }

  static void process() {    
    static int cnt = 0;
    constexpr static uint16_t second_timeout = 1000/state_update_rate;    
    
    if(timer++ >= second_timeout) {
      timer = 0;
      dot = !dot;
      SoundGenerator::tick();
      cnt++;

      time_pair time = RTCSync::current_time();
      Effects::Transition::display(time.hours, time.minutes, {false, false, dot, false});  

    // Load settings from EEPROM
    //      - Tube saveing timer (off-time span, disable tubes and clicks)
    //      - Night time mode (ambient light below threshold, reduce light to minimum, disable leds, disable clicks and effects)
    // Call initialize when one of the modes are left
    }
  }

  // Increment hours by one (summer-/wintertime)
  static void on_plus_long() {
    SoundGenerator::ack_long();
    RTCSync::increment_hours(1);
  }

  static void on_minus_long() {
    SoundGenerator::ack_long();
    RTCSync::increment_hours(-1);
  }

  static void on_select_reset() {
    UI::next<ServiceMenu>();    
  }

  static void on_select_short() {
    UI::next<TimeSet>();
  }

  static void on_select_long() {
    UI::next<SettingInit>();
  }

private:
  static uint16_t timer;    
  static bool dot;
  static SettingSet settings;
};
